/* Sort code for HAGAR, the NaI gamma detector
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for gamma sort codes for K600 experiments.
 * 
 * 
 */

#include "GammaData.h"
#include "ScintillatorSort.h"
#include <math.h>
#include <stdlib.h>
#include <TRandom3.h>

extern int ADCModules;
extern int ADCsize;
// extern int NumberOfScintillator;

extern int *ScintillatorADCChannelLimits;
extern int *ScintillatorTDCChannelLimits;

extern double *ADCOffsets;
extern double *ADCGains;

TRandom3 *randy2 = new TRandom3(0);



//GammaData *ScintillatorSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
void ScintillatorSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, GammaData *gammy)
{
  //GammaData *gammy = new GammaData();
  //Loop over ADC and TDC events and do the following:
  //Check whether there are front-back coincidences for a detector and test the energies
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - TDCs are apparently in single-hit mode. Don't need to worry about multihits
  //Calculate the theta and phi for the valid events
  multiTDC *mTDC = new multiTDC;
  mTDC->multiTDCSort(ntdc, TDC_channel_import, TDC_value_import);	

  for(int k=0;k<mTDC->GetSize();k++)//Loop over all of the TDC values - there should only be a small number of these relative to the ADC values
   {
    if(ScintillatorTDCTest(mTDC->GetChannel(k)))
      {
		  for(int i=ScintillatorADCChannelLimits[0];i<=ScintillatorADCChannelLimits[1];i++)
		      {
			if(ScintillatorADCTDCChannelCheck(i,mTDC->GetChannel(k)))
			{
			// 	  printf("ADCChannel: %d \t TDCChannel: %d\n",i,mTDC->GetChannel(n));
	  			double GammaEnergy = ScintillatorEnergyCalc(i,ADC_import[i]);
	  			if(GammaEnergy>0.1)
	  			{
	    			gammy->SetEnergy(GammaEnergy);
	    			gammy->SetTime(mTDC->GetValue(k));
				gammy->SetDetectorType("Scintillator"); //the detector type will allow to choose between Hagar, Clover or Scintillator

				int label= 1 + i - ScintillatorADCChannelLimits[0]; //the detector number starts from 1
				gammy->SetDetectorLabel(label); 
// 				gammy->SetGammaRawADC(ADC_import[i]);
// 				gammy->SetGammaADCChannel(i);
// 				gammy->SetGammaTDCChannel(mTDC->GetChannel(k));
// 				gammy->SetGammaTDCMultiplicity(mTDC->GetMult(k));
	  			} 
			 }
      		       }
    		
	}
    }

  gammy->SetHits(gammy->SizeOfEvent());
  mTDC->ClearEvent();
  delete mTDC;
  //return gammy;

}


double ScintillatorThetaCalc(int Channel)
{
  return 0;
}

double ScintillatorPhiCalc(int Channel)
{
  return 0;
}


double ScintillatorEnergyCalc(int Channel, double ADCValue)
{

		  
      double randNum = randy2->Rndm();

 // printf("Channel: %i \t ADCValue: %f\n",Channel,ADCValue);
 // printf("Offset: %f \t Gain: %f\n",ADCOffsets[Channel],ADCGains[Channel]);
// double result = ADCOffsets[Channel] + ADCGains[Channel]*ADCValue;
// double result = ADCOffsets[Channel] + ADCGains[Channel]*(ADCValue+randNum-0.5);
   double result = ADCOffsets[Channel] + ADCGains[Channel]*(ADCValue+randNum);
// printf("ADCValue: %f \n  rand(): %f \n  result: %f\n", ADCValue, randNum,result);  
  return result;
}

bool ScintillatorTDCTest(int TDCChannel) // to check if the TDCChannel is the one associated with the Scintillators
{
  bool result = false;
      if(TDCChannel>=ScintillatorTDCChannelLimits[0] && TDCChannel<=ScintillatorTDCChannelLimits[1])
	{
	  result = true;
	}
      if(ScintillatorTDCChannelLimits[0]==-1)result = true;
      if(ScintillatorTDCChannelLimits[1]==-1)result = true;
     
  return result;
}


bool ScintillatorADCTDCChannelCheck(int ADCChannel, int TDCChannel) // to consider only the TDC/ADC channel associated with a Scintillator
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
    if(ADCChannel>=ScintillatorADCChannelLimits[0] && ADCChannel<=ScintillatorADCChannelLimits[1] && TDCChannel>=ScintillatorTDCChannelLimits[0] && TDCChannel<=ScintillatorTDCChannelLimits[1])//Check to see if the ADC/TDC events are in the same detector
   {
     if((ADCChannel-ScintillatorADCChannelLimits[0])==(TDCChannel-ScintillatorTDCChannelLimits[0]))
     {
     //   printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
     if(ScintillatorADCChannelLimits[0]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(ScintillatorADCChannelLimits[1]==-1)result = true;
     if(ScintillatorTDCChannelLimits[0]==-1)result = true;
     if(ScintillatorTDCChannelLimits[1]==-1)result = true;
    
  return result;
}
/*
int ScintillatorDetHitNumber(int ADCChannel) //number of hit in one Scintillator
{
  int result = 0;
  for(int i=0;i<NumberOfScintillator;i++)
  {
    if(ADCChannel>=ScintillatorADCChannelLimits[i][0] && ADCChannel<=ScintillatorADCChannelLimits[i][1])
      {
	result = i+1;
      }
  }
printf("result = %d\n",result);
  return result;
}
*/
