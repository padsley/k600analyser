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
// 
const int ADCsize = 128;
const int NumberOfDetectors = 4;

int **MMMADCChannelLimits;
int **MMMTDCChannelLimits;

TCutG *MMMFrontBackEnergyCut;

// double silicon_offset[128] = {-167.805,-214.406,-187.454,-181.633,-160.386,-149.132,-202.629,-160.261,-203.654,-173.817,-187.64,-157.683,-148.687,-92.1872,-161.218,-70.8537,-180.455,-180.758,-227.364,-152.841,-184.163,-138.119,-178.638,-167.513,-235.224,-225.14,-196.467,-234.196,-171.881,-185.131,-151.409,-145.787,258.692,-88.8767,-140.271,-101.703,-140.5,-88.484,-245.035,-77.4569,-94.7413,-146.396,-136.314,-117.08,-215.074,-105.078,-144.869,-161.288,-295.513,-297.778,-193.066,-238.863,-246.341,-266.892,-275.673,-259.938,-264.263,-320.586,-240.4,-273.869,-234.21,-175.276,-229.119,-100.009,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-153.9,-141.116,-66.7166,-103.2,-130.332,-95.8439,-112.015,-162.337,-85.2311,-104.158,-133.836,-104.588,-176.148,-125.141,-90.5022,-124.296,-29.529,-61.7865,-88.3409,-114.406,-92.1489,-5.52266,-50.7833,-84.6803,-97.1285,-49.1359,-65.4897,-101.801,-92.5068,-116.007,-49.3386,-73.9171,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// 
// double silicon_gain[128] = {2.7211,2.71494,2.73297,2.73106,2.73573,2.68548,2.76225,2.80152,2.79172,2.75188,2.80635,2.66006,2.75826,2.96099,2.78068,2.73402,2.79736,2.86712,2.84351,2.74067,2.83164,2.86948,2.87157,2.79478,2.83121,2.88493,2.84582,2.77553,2.88338,2.88706,2.93624,2.89496,2.74762,2.8514,2.89757,2.89894,2.88723,2.87429,2.90762,2.93522,2.94179,2.87595,2.90036,2.93048,2.95111,2.95414,2.97117,2.99479,3.17541,3.19887,3.14558,3.18869,3.12393,3.14539,3.17036,3.12102,3.07041,3.17293,3.2341,3.23097,3.25645,3.16729,3.24288,3.06999,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3.01396,2.90243,3.03025,2.99131,2.99434,3.03356,3.01627,2.97887,2.93761,2.98534,3.02228,3.01619,3.02228,3.0249,3.06732,2.95583,2.87665,2.86676,2.94053,2.91049,2.99978,2.91588,2.82934,2.94514,2.96318,2.88167,2.87429,3.01928,2.89797,2.81918,2.94514,2.75654,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

SiliconData *MMMSiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  SiliconData *si = new SiliconData();
  MMMInit();
  //Loop over ADC and TDC events and do the following:
  
  //Check whether there are front-back coincidences for a detector and test the energies
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - TDCs are apparently in single-hit mode. Don't need to worry about multihits
  //Calculate the theta and phi for the valid events
//   printf("ntdc: %d\n",ntdc);
//   printf("L35\n");
  multiTDC *mTDC = new multiTDC(ntdc, TDC_channel_import, TDC_value_import);
//   printf("L37\n");
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
// 		printf("TEST81\n");
		si->SetMult(mTDC->GetMult(k));
	      }
	    }
	  }
	}
      }
    }
  }
  
//      printf("L90\n");
  si->SetHits(si->SizeOfEvent());
  if(!si->TestEvent())si->ClearEvent(); //If the event fails for some reason, we void it and clear it here. The number of these should be logged and, ideally, should be zero. A VOIDED EVENT IS ONE IN WHICH ALL SILICON DATA ARE THROWN AWAY BECAUSE THE RESULT IS **WRONG**. There are more energy hits than theta hits, for example. IT THEY ARE HAPPENING, THEN YOU'VE DONE IT WRONG.

  mTDC->ClearEvent();
  
  delete mTDC;
  
  for(int i=0;i<NumberOfDetectors;i++)
  {
    delete MMMADCChannelLimits[i];
    delete MMMTDCChannelLimits[i];
  }
  delete MMMADCChannelLimits;
  delete MMMTDCChannelLimits;
  
  return si;
}

void MMMLoadCuts(SiliconData *si)
{
  //  gROOT->ProcessLine(".x FrontBackEnergyCut.C");
  //  si->SetFrontBackEnergyCut((TCutG*)gROOT->FindObjectAny("FrontBackEnergyCut"));
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
  //   printf("Start Init\n");
  MMMADCChannelLimits = new int*[NumberOfDetectors];
  for(int i=0;i<NumberOfDetectors;i++)
  {
    MMMADCChannelLimits[i] = new int[4];
  }
  //   
  //   //I set the channel address limits manually. I could do it with an automatic script but it's actually simpler this way. Get back to work!
  //   
  MMMADCChannelLimits[0][0] = 0;//First channel of the front of the first detector
  MMMADCChannelLimits[0][1] = 15;//Last channel of the front of the first detector
  MMMADCChannelLimits[0][2] = 80;//First channel of the back of the first detector
  MMMADCChannelLimits[0][3] = 87;//Last channel of the back of the first detector
  //   
  MMMADCChannelLimits[1][0] = 16;
  MMMADCChannelLimits[1][1] = 31;
  MMMADCChannelLimits[1][2] = 88;
  MMMADCChannelLimits[1][3] = 95;
  //   
  MMMADCChannelLimits[2][0] = 32;
  MMMADCChannelLimits[2][1] = 47;
  MMMADCChannelLimits[2][2] = 96;
  MMMADCChannelLimits[2][3] = 103;
  //   
  MMMADCChannelLimits[3][0] = 48;
  MMMADCChannelLimits[3][1] = 63;
  MMMADCChannelLimits[3][2] = 104;
  MMMADCChannelLimits[3][3] = 111;
  //   
  //   //Currently, the TDCs only exist for the fronts so we shall limit ourself to having an array size of 2 for these. If TDC fronts and backs are instrumented, this will need to change to something that looks like what is above
  //   
  MMMTDCChannelLimits = new int*[NumberOfDetectors];
  for(int i=0;i<NumberOfDetectors;i++)
  {
    MMMTDCChannelLimits[i] = new int[2];
  }
  
  MMMTDCChannelLimits[0][0] = 128*6+48;
  MMMTDCChannelLimits[0][1] = 128*6+48+15;
  
  MMMTDCChannelLimits[1][0] = 128*6+48+16;
  MMMTDCChannelLimits[1][1] = 128*6+48+31;
  
  MMMTDCChannelLimits[2][0] = 128*6+48+32;
  MMMTDCChannelLimits[2][1] = 128*6+48+47;
  
  MMMTDCChannelLimits[3][0] = 128*6+48+48;
  MMMTDCChannelLimits[3][1] = 128*6+48+63;
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
  double result = silicon_offset[Channel] + silicon_gain[Channel]*ADCValue;
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
  
  for(int i=0;i<NumberOfDetectors;i++)
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
  for(int i=0;i<NumberOfDetectors;i++)
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
  for(int i=0;i<NumberOfDetectors;i++)
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

