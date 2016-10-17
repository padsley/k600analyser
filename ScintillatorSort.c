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
extern int NumberOfScintillator;

extern int *ScintillatorADCChannels;
extern int *ScintillatorTDCChannels;

extern double *ADCOffsets;
extern double *ADCGains;

extern double **ADCCalibrationParameters;

TRandom3 *randyScint = new TRandom3(0);



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
          int DetNum = ScintillatorTDCIdentifyDetector(mTDC->GetChannel(k));
          if(DetNum>0)
          	{
		  for(int i=DetNum-2;i<NumberOfScintillator+1;i=i+2)
		      {
                       int j = ScintillatorADCChannels[i-1];
			 //	  printf("Det: %d; \tADCChannel: %d\n",i,j);                       
			if(ScintillatorADCTDCChannelCheck(j,mTDC->GetChannel(k)))
			{
			 //	  printf("Det: %d; \tADCChannel: %d \t TDCChannel: %d\n",i,j,mTDC->GetChannel(k));
	  			double GammaEnergy = ScintillatorEnergyCalc(j,ADC_import[j]);
	  			if(GammaEnergy>0.1)
	  			{
	    			gammy->SetEnergy(GammaEnergy);
	    			gammy->SetTime(mTDC->GetValue(k));
				gammy->SetDetectorType("Scintillator"); //the detector type will allow to choose between Hagar, Clover or Scintillator

				int label= i; //the detector number starts from 1
				gammy->SetDetectorLabel(label); 
 				gammy->SetGammaRawADC(ADC_import[j]);
 				gammy->SetGammaADCChannel(j);
 				gammy->SetGammaTDCChannel(mTDC->GetChannel(k));
 				gammy->SetGammaTDCMultiplicity(mTDC->GetMult(k));
	  			} 
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


//function to identify the clover number
int ScintillatorTDCIdentifyDetector(int TDCChannel)
{
 int testresult = -1;
 int result=0;
   for(int i=0;i<NumberOfScintillator;i++)
    {
     testresult = -1;
       if(TDCChannel==ScintillatorTDCChannels[i])
       	{
	  		 testresult = i+1;
				}
			if(testresult>0) result=testresult;
//			printf("CloverTDCChannelLimits: %d  CloverTDCChannelLimits: %d \n TDC Channel: %d\n Clovernumber: %d\n ",CloverTDCChannelLimits[i][0], CloverTDCChannelLimits[i][1], TDCChannel,i);  
// 			printf("testresult = %d\n",testresult);
   }
//  printf("result = %d\n",result);
 return result; //WATCHOUT we have the same TDC Channel associated with two ADC Channels. At this point this will always give 3 and 4 as the hit detectors.
}


double ScintillatorEnergyCalc(int Channel, double ADCValue)
{

		  
  double randNum = randyScint->Rndm();

  double RandyADCValue = ADCValue+randNum;

  int npars = ADCCalibrationParameters[Channel][0];

  double result = 0;

  for(int i=1;i<npars+1;i++)
  {
    result += ADCCalibrationParameters[Channel][i] * pow(RandyADCValue,(double)i-1.);
  }

  return result;
}

bool ScintillatorTDCTest(int TDCChannel) // to check if the TDCChannel is the one associated with the Scintillators
{
  bool result = false;
      for(int i=0;i<NumberOfScintillator;i++){
        if(TDCChannel==ScintillatorTDCChannels[i])
	{
	  result = true;
	}
      }
      if(ScintillatorTDCChannels[0]==-1)result = true;
      if(ScintillatorTDCChannels[1]==-1)result = true;
     
  return result;
}


bool ScintillatorADCTDCChannelCheck(int ADCChannel, int TDCChannel) // to consider only the TDC/ADC channel associated with a Scintillator
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);

  for(int i=0;i<NumberOfScintillator;i++)
    {
    if(ADCChannel==ScintillatorADCChannels[i] && TDCChannel==ScintillatorTDCChannels[i])//Check to see if the ADC/TDC events are in the same detector
     {
     //   printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
     if(ScintillatorADCChannels[i]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(ScintillatorTDCChannels[i]==-1)result = true;
    }
    
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
