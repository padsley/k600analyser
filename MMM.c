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

extern int **MMMADCChannelLimits;
extern int **MMMTDCChannelLimits;

extern double *ADCOffsets;
extern double *ADCGains;

TCutG *MMMFrontBackEnergyCut;

const double sigma = 30;//keV - silicon energy resolution - used for the front-back energy cut condition

SiliconData *MMMSiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  SiliconData *si = new SiliconData();

  //Loop over ADC and TDC events and do the following:
  //Check whether there are front-back coincidences for a detector and test the energies
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - TDCs are apparently in single-hit mode. Don't need to worry about multihits
  //Calculate the theta and phi for the valid events
  multiTDC mTDC;// = new multiTDC();
  mTDC.multiTDCSort(ntdc, TDC_channel_import, TDC_value_import);

  for(int k=0;k<mTDC.GetSize();k++)//Loop over all of the TDC values - there should only be a small number of these relative to the ADC values
  {
    if(MMMTDCFrontTest(mTDC.GetChannel(k)))
      {
	for(int l=0;l<mTDC.GetSize();l++)//Loop over all of the TDC values *again* but in this case, looking for the N sides
	  {
	    if(MMMTDCBackTest(mTDC.GetChannel(l)) && MMMTDCFrontBackTest(mTDC.GetChannel(k),mTDC.GetChannel(l)))
	      {
		int DetNum = MMMTDCIdentifyDetector(mTDC.GetChannel(k),mTDC.GetChannel(l));
		if(DetNum>0)
		  {	
		    for(int i=MMMADCChannelLimits[DetNum-1][0];i<=MMMADCChannelLimits[DetNum-1][1];i++)
		      {
			//Don't want to run for events w
			if(MMMADCTDCChannelTestPSide(i,mTDC.GetChannel(k)) && ADC_import[i]>0)
			  {
			    for(int j=MMMADCChannelLimits[DetNum-1][2];j<=MMMADCChannelLimits[DetNum-1][3];j++)
			      {
				if(ADC_import[j]>0)
				  {
				    double energyi = MMMEnergyCalc(i,ADC_import[i]);
				    double energyj = MMMEnergyCalc(j,ADC_import[j]);
				    
				    //Test whether the hits are in the front and back of the same detector and whether the energies are good
				    if(MMMFrontBackTest(i,j,energyi,energyj,si) && MMMADCTDCChannelTestNSide(j,mTDC.GetChannel(l)) && 0.5*(energyi+energyj)>400)
				      {
					si->SetEnergy(0.5*(energyi+energyj));
					si->SetTheta(MMMThetaCalc(i));
					si->SetPhi(MMMPhiCalc(j));
					
					si->SetTime(mTDC.GetValue(k));
					si->SetTimeFront(mTDC.GetValue(k));
					si->SetTimeBack(mTDC.GetValue(l));
					
					si->SetDetectorHit(MMMDetHitNumber(i,j));
					si->SetADCChannelFront(i);
					si->SetADCChannelBack(j);
					si->SetStripFront(MMMStripFront(i));
					si->SetStripBack(MMMStripBack(j));
					  
					si->SetTDCChannelFront(mTDC.GetChannel(k));
					si->SetTDCChannelBack(-1);
					si->SetADCValueFront(ADC_import[i]);
					si->SetADCValueBack(ADC_import[j]);
					  
					si->SetTDCValueFront(mTDC.GetValue(k));
					si->SetTDCValueBack(-1);
					si->SetEnergyFront(energyi);
					si->SetEnergyBack(energyj);
					  
					si->SetMult(mTDC.GetMult(k));
				      }
				  }
			      }
			  }
		      }
		  }
	      }
	  }
      }
  }
  
  si->SetHits(si->SizeOfEvent());
  if(!si->TestEvent())si->ClearEvent(); //If the event fails for some reason, we void it and clear it here. The number of these should be logged and, ideally, should be zero. A VOIDED EVENT IS ONE IN WHICH ALL SILICON DATA ARE THROWN AWAY BECAUSE THE RESULT IS **WRONG**. There are more energy hits than theta hits, for example. IT THEY ARE HAPPENING, THEN YOU'VE DONE IT WRONG.
  //printf("MMM.c L88");
  //MMMGhostBuster(SiliconData *si);

  mTDC.ClearEvent();
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
  double result = ADCOffsets[Channel] + ADCGains[Channel]*ADCValue;
//   printf("Energy = ADCOffsets[%d] + ADCGains[%d] * ADCValue\t %f = %f + %f * %f\n",Channel,Channel,result,ADCOffsets[Channel],ADCGains[Channel],ADCValue);
  //   if(Channel<64 || Channel>=80 && Channel<112)printf("EnergyCalc: %g\n",result);
  return result;
}

double MMMThetaCalc(int Channel)
{
  double theta = 180;
  switch (Channel%16)//The MMM detectors run 0->15, 16->31 etc. If this changes, this theta calculation section will also be forced to change
    {
    case 0:
      theta = 164.425240;
      break;
    case 1:
      theta = 161.351428;
      break;
    case 2:
      theta = 161.351428;
      break;
    case 3:
      theta = 154.813167;
      break;
    case 4:
      theta = 151.386395;
      break;
    case 5:
      theta = 147.883131;
      break;
    case 6:
      theta = 144.311153;
      break;
    case 7:
      theta = 140.718818;
      break;
    case 8:
      theta = 137.129755;
      break;
    case 9:
      theta = 133.555311;
      break;
    case 10:
      theta = 130.039133;
      break;
    case 11:
      theta = 126.603074;
      break;
    case 12:
      theta = 123.265031;
      break;
    case 13:
      theta = 120.040891;
      break;
    case 14:
      theta = 116.950276;
      break;
    case 15:
      theta = 113.998884;
      break;
    default :
      theta = 0;
      printf("Theta value not found - you doggone fucked up, lad\n");
    }
  return theta;
}

double MMMPhiCalc(int Channel)
{
  double phi = 0;
  switch ((Channel-64)%8)
    {
    case 0:
      phi = 26.270786;
      break;
    case 1:
      phi = 34.409036;
      break;
    case 2:
      phi = 42.881504;
      break;
    case 3:
      phi = 51.563195;
      break;
    case 4:
      phi = 60.289173;
      break;
    case 5:
      phi = 68.896820;
      break;
    case 6:
      phi = 77.234830;
      break;
    case 7:
      phi = 85.188565;
      break;
    default : 
      phi = 0;
      printf("Phi value not found - you doggone fucked up, lad... Phi switch case is %d\n",(Channel-80)%8);
    }
  return phi;
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
	  double diff = FrontEnergy - BackEnergy;
	  //if(diff<0)diff*=-1;
	  //if(diff/(0.5*(FrontEnergy+BackEnergy))<0.05)//Check to see if the front and back energies are approximately equal <5%
	  if(std::abs(diff)<3*sigma)
	  {
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

bool MMMADCTDCChannelTestPSide(int ADCChannel, int TDCChannel)
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(ADCChannel>=MMMADCChannelLimits[i][0] && ADCChannel<=MMMADCChannelLimits[i][1] && TDCChannel>=MMMTDCChannelLimits[i][0] && TDCChannel<=MMMTDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
   {
     if(ADCChannel%16==TDCChannel%16)
     {
       //printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
     if(MMMADCChannelLimits[i][0]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(MMMADCChannelLimits[i][1]==-1)result = true;
     if(MMMTDCChannelLimits[i][0]==-1)result = true;
     if(MMMTDCChannelLimits[i][1]==-1)result = true;
     }
  return result;
}

bool MMMADCTDCChannelTestNSide(int ADCChannel, int TDCChannel)
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(ADCChannel>=MMMADCChannelLimits[i][2] && ADCChannel<=MMMADCChannelLimits[i][3] && TDCChannel>=MMMTDCChannelLimits[i][2] && TDCChannel<=MMMTDCChannelLimits[i][3])//Check to see if the ADC/TDC events are in the same detector
   {
     if(ADCChannel%8==TDCChannel%8)
     {
//        printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
     if(MMMADCChannelLimits[i][2]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(MMMADCChannelLimits[i][3]==-1)result = true;
     if(MMMTDCChannelLimits[i][2]==-1)result = true;
     if(MMMTDCChannelLimits[i][3]==-1)result = true;
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

bool MMMTDCFrontTest(int TDCChannel)
{
  bool result = false;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCChannel>=MMMTDCChannelLimits[i][0] && TDCChannel<=MMMTDCChannelLimits[i][1])
	{
	  result = true;
	}
      if(MMMTDCChannelLimits[i][0]==-1)result = true;
      if(MMMTDCChannelLimits[i][1]==-1)result = true;
    }
 
  return result;
}

bool MMMTDCBackTest(int TDCChannel)
{
  bool result = false;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCChannel>=MMMTDCChannelLimits[i][2] && TDCChannel<=MMMTDCChannelLimits[i][3])
	{
	  result = true;
	}
      if(MMMTDCChannelLimits[i][2]==-1)result = true;
      if(MMMTDCChannelLimits[i][3]==-1)result = true;
    }
  return result;
}

bool MMMTDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel)
{
  bool result = false;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCFrontChannel>=MMMTDCChannelLimits[i][0] && TDCFrontChannel<=MMMTDCChannelLimits[i][1] && TDCBackChannel>=MMMTDCChannelLimits[i][2] && TDCBackChannel<=MMMTDCChannelLimits[i][3])
	{
	  result = true;
	}
    }
  return result;
}
  
int MMMTDCIdentifyDetector(int TDCFrontChannel, int TDCBackChannel)
{
  int result = -1;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCFrontChannel>=MMMTDCChannelLimits[i][0] && TDCFrontChannel<=MMMTDCChannelLimits[i][1] && TDCBackChannel>=MMMTDCChannelLimits[i][2] && TDCBackChannel<=MMMTDCChannelLimits[i][3])
	{
	  result = i+1;
	}
    }
  //printf("result = %d\n",result);
  return result;
}
