/* Sort code for MMM-type silicon detectors
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for silicon sort codes for K600 experiments.
 * 
 * The header file MMM.h defines the ADC and TDC channel mapping for the particular experiment in question. The user should modify these accordingly. How these channel mappings are defined as given in ???
 * 
 */

#include "SiliconData.h"
#include "MMM.h"

extern int ADCModules;
extern int ADCsize;
extern int NumberOfMMM;

extern int MMMADCChannelLimits[4][4];
extern int MMMTDCChannelLimits[4][4];

TCutG *MMMFrontBackEnergyCut;

SiliconData *MMMSiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  SiliconData *si = new SiliconData();
  MMMInit();
  //Loop over ADC and TDC events and do the following:
  
  //Check whether there are front-back coincidences for a detector and test the energies
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - TDCs are apparently in single-hit mode. Don't need to worry about multihits
  //Calculate the theta and phi for the valid events
  multiTDC *mTDC = new multiTDC(ntdc, TDC_channel_import, TDC_value_import);
//    printf("L37\n");
  for(int k=0;k<mTDC->GetSize();k++)//Loop over all of the TDC values - there should only be a small number of these relative to the ADC values
  {
//     if(TDC_channel_import[k]>6*128)//This limits the code to only consider TDC values in the last TDC module, which is the one that deals with the silicon timing values
    {
      for(int i=0;i<ADCsize;i++)
      {
	//Don't want to run for events which are below pedestal. Set this to be 250 generally for the moment. In future, might want to increase it a bit
	if(MMMADCTDCChannelTest(i,mTDC->GetChannel(k)) && ADC_import[i]>300)
	{
	  for(int j=0;j<ADCsize;j++)
	  {
	    if(ADC_import[j]>300)
	    {
	      double energyi = MMMEnergyCalc(i,ADC_import[i]);
	      double energyj = MMMEnergyCalc(j,ADC_import[j]);
	      
	      //Test whether the hits are in the front and back of the same detector and whether the energies are good
	      if(MMMFrontBackTest(i,j,energyi,energyj,si))
	      {
// 		printf("TEST57\n");
		si->SetEnergy(0.5*(energyi+energyj));
		si->SetTheta(MMMThetaCalc(i));
		si->SetPhi(MMMPhiCalc(i));
		si->SetTime(mTDC->GetValue(k));
// 		si->SetTime(TDC_value_import[k]);
		// 		si->SetTime(0);
		
		si->SetDetectorHit(MMMDetHitNumber(i,j));
		si->SetADCChannelFront(i);
		si->SetADCChannelBack(j);
		si->SetStripFront(MMMStripFront(i));
		si->SetStripBack(MMMStripBack(j));
		si->SetTDCChannelFront(mTDC->GetChannel(k));
// 		si->SetTDCChannelFront(TDC_channel_import[k]);
		// 		si->SetTDCChannelFront(-1);
		si->SetTDCChannelBack(-1);
		si->SetADCValueFront(ADC_import[i]);
		si->SetADCValueBack(ADC_import[j]);
		
		si->SetTDCValueFront(mTDC->GetValue(k));
// 		si->SetTDCValueFront(TDC_value_import[k]);
		// 		si->SetTDCValueFront(-1);
		si->SetTDCValueBack(-1);
		si->SetEnergyFront(energyi);
		si->SetEnergyBack(energyj);

		si->SetMult(mTDC->GetMult(k));
	      }
	    }
	  }
	}
      }
    }
  }
  
  si->SetHits(si->SizeOfEvent());
  if(!si->TestEvent())si->ClearEvent(); //If the event fails for some reason, we void it and clear it here. The number of these should be logged and, ideally, should be zero. A VOIDED EVENT IS ONE IN WHICH ALL SILICON DATA ARE THROWN AWAY BECAUSE THE RESULT IS **WRONG**. There are more energy hits than theta hits, for example. IT THEY ARE HAPPENING, THEN YOU'VE DONE IT WRONG.

  mTDC->ClearEvent();
  
  delete mTDC;
  
  return si;
}

void MMMLoadCuts(SiliconData *si)
{
  printf("Load MMM Front-Back Energy cut\n");
  TCutG *cutg = new TCutG("FrontBackEnergyCut",8);
  cutg->SetVarX("EnergyBack");
  cutg->SetVarY("EnergyFront");
  cutg->SetTitle("Graph");
  cutg->SetFillColor(1);
  cutg->SetPoint(0,8625.54,9448.89);
  cutg->SetPoint(1,9436.42,9393.27);
  cutg->SetPoint(2,9398.71,8336.6);
  cutg->SetPoint(3,8512.39,8253.18);
  cutg->SetPoint(4,610.991,300.318);
  cutg->SetPoint(5,516.703,912.076);
  cutg->SetPoint(6,8625.54,9143.01);
  cutg->SetPoint(7,8625.54,9448.89);
  
  MMMFrontBackEnergyCut = cutg;
}

void MMMInit()//Initialise function which gets the information on the DAQ channels->Physical channels
{ 
//This bit is currently doing nowt but may be called at the beginning of each 'event' to set up useful things for other bits of the code.
}

bool MMMSuppressChannel(int Channel)//If the ADC channel is one which we wish to suppress, we do that here. Use if(Channel = 12)return true to suppress channel 12. Load of else ifs for the other suppressed channels. Then else return false.
{
  if(Channel>128)
    return true;
  else
    return false;
}

double MMMEnergyCalc(int Channel, double ADCValue)
{
  //define the silicon calibration parameters
  extern double SiliconOffset[128];
  extern double SiliconGain[128];
  double result = SiliconOffset[Channel] + SiliconGain[Channel]*ADCValue;
  //   if(Channel<64 || Channel>=80 && Channel<112)printf("EnergyCalc: %g\n",result);
  return result;
}

double MMMThetaCalc(int Channel)
{
  double result = 180;
  return result;
}

double MMMPhiCalc(int Channel)
{
  double result = 0;
  return result;
}

bool MMMFrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si)
{
  bool result = false;
  //     printf("FrontBackTest Start\n");
  //   gROOT->ProcessLine(".x FrontBackEnergyCut.C");
  
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(FrontChannel>=MMMADCChannelLimits[i][0] && FrontChannel<=MMMADCChannelLimits[i][1] && BackChannel>=MMMADCChannelLimits[i][2] && BackChannel<=MMMADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	if(MMMFrontBackEnergyCut->IsInside(BackEnergy,FrontEnergy))//Check to see if the front and back energies are approximately equal
	  {
	    // 	    printf("Tust\n");
	    result = true;//They are -> it's a good event
	  }
      }
  }
  //   printf("FrontBackTest End");
  return result;
}


int MMMDetHitNumber(int FrontChannel, int BackChannel)
{
  int result = 0;
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(FrontChannel>=MMMADCChannelLimits[i][0] && FrontChannel<=MMMADCChannelLimits[i][1] && BackChannel>=MMMADCChannelLimits[i][2] && BackChannel<=MMMADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	result = i+1;
      }
  }
  return result;
}

bool MMMADCTDCChannelTest(int ADCChannel, int TDCChannel)
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(ADCChannel>=MMMADCChannelLimits[i][0] && ADCChannel<=MMMADCChannelLimits[i][1] && TDCChannel>=MMMTDCChannelLimits[i][0] && TDCChannel<=MMMTDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
   {
     if(TDCChannel<816)printf("Pass for invalid TDC value! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
     if(ADCChannel%16==TDCChannel%16)
     {
//        printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
  }
  return result;
}


int MMMStripFront(int FrontChannel)
{
  int result = 0;
  result = FrontChannel%16;
  return result;
}

int MMMStripBack(int BackChannel)
{
  int result = 0;
  result = BackChannel%8;
  return result;
}
