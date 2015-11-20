/* Sort code for HAGAR, the NaI gamma detector
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for gamma sort codes for K600 experiments.
 * 
 * 
 */

#include "GammaData.h"
#include "CloverSort.h"
#include <math.h>
#include <stdlib.h>
#include <TRandom3.h>

extern int ADCModules;
extern int ADCsize;
extern int NumberOfClover;

extern int **CloverADCChannelLimits;
extern int **CloverTDCChannelLimits;

extern double *ADCOffsets;
extern double *ADCGains;

TRandom3 *randy = new TRandom3(0);



//GammaData *CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
void CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, GammaData *gammy)
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
    if(CloverTDCTest(mTDC->GetChannel(k)))
      {
	int DetNum = CloverTDCIdentifyDetector(mTDC->GetChannel(k));
//  printf("+++++++++++++++++++DetNum %d\n",DetNum);
  int Segm = CloverTDCIdentifySegment(mTDC->GetChannel(k));
 // printf("-------------Segm %d\n",Segm);
	 if(DetNum>0)
		{	
		  for(int i=CloverADCChannelLimits[DetNum-1][0];i<=CloverADCChannelLimits[DetNum-1][1];i++)
		      {
			if(CloverADCTDCChannelCheck(i,mTDC->GetChannel(k)))
			{
			// 	  printf("ADCChannel: %d \t TDCChannel: %d\n",i,mTDC->GetChannel(n));
	  			double GammaEnergy = CloverEnergyCalc(i,ADC_import[i]);
	  			if(GammaEnergy>0.1)
	  			{
	    			gammy->SetEnergy(GammaEnergy);
	    			gammy->SetTime(mTDC->GetValue(k));
				gammy->SetDetectorType("Clover");

				gammy->SetDetectorLabel(DetNum);
				gammy->SetDetectorSegm(Segm);
				gammy->SetGammaRawADC(ADC_import[i]);
				gammy->SetGammaADCChannel(i);
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


double CloverThetaCalc(int Channel)
{
  return 0;
}

double CloverPhiCalc(int Channel)
{
  return 0;
}

//function to identify the clover number
int CloverTDCIdentifyDetector(int TDCChannel)
{
 int testresult = -1;
 int result=0;
   for(int i=0;i<NumberOfClover;i++)
    {
     testresult = -1;
       if(TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])
       	{
	  		 testresult = i+1;
				}
			if(testresult>0) result=testresult;
//			printf("CloverTDCChannelLimits: %d  CloverTDCChannelLimits: %d \n TDC Channel: %d\n Clovernumber: %d\n ",CloverTDCChannelLimits[i][0], CloverTDCChannelLimits[i][1], TDCChannel,i);  
// 			printf("testresult = %d\n",testresult);
   }
//  printf("result = %d\n",result);
 return result;
}

//function to identify the segment number
int CloverTDCIdentifySegment(int TDCChannel)
{
 int testresult = -1;
 int result=0;
  for(int i=0;i<NumberOfClover;i++)
    {
      testresult = -1;
      if(TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])
	    {
	     testresult = (TDCChannel-CloverTDCChannelLimits[i][0])+1;
	    }
      if(testresult>0) result=testresult;
//			printf("CloverTDCChannelLimits: %d  CloverTDCChannelLimits: %d \n TDC Channel: %d\n Clovernumber: %d\n ",CloverTDCChannelLimits[i][0], CloverTDCChannelLimits[i][1], TDCChannel,i);  
 //			printf("testresult = %d\n",testresult);;
  }
 // printf("+++++++++++++result = %d\n",result);
  return result;
}

double CloverEnergyCalc(int Channel, double ADCValue)
{

		  
      double randNum = randy->Rndm();

 // printf("Channel: %i \t ADCValue: %f\n",Channel,ADCValue);
 // printf("Offset: %f \t Gain: %f\n",ADCOffsets[Channel],ADCGains[Channel]);
// double result = ADCOffsets[Channel] + ADCGains[Channel]*ADCValue;
// double result = ADCOffsets[Channel] + ADCGains[Channel]*(ADCValue+randNum-0.5);
   double result = ADCOffsets[Channel] + ADCGains[Channel]*(ADCValue+randNum);
// printf("ADCValue: %f \n  rand(): %f \n  result: %f\n", ADCValue, randNum,result);  
  return result;
}

bool CloverTDCTest(int TDCChannel) // to check if the TDCChannel is the one associated with the Clovers
{
  bool result = false;
  for(int i=0;i<NumberOfClover;i++)
    {
      if(TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])
	{
	  result = true;
	}
      if(CloverTDCChannelLimits[i][0]==-1)result = true;
      if(CloverTDCChannelLimits[i][1]==-1)result = true;
    }
 
  return result;
}


bool CloverADCTDCChannelCheck(int ADCChannel, int TDCChannel) // to consider only the TDC/ADC channel associated with a Clover
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfClover;i++)
  {
    if(ADCChannel>=CloverADCChannelLimits[i][0] && ADCChannel<=CloverADCChannelLimits[i][1] && TDCChannel>=CloverTDCChannelLimits[i][0] && TDCChannel<=CloverTDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
   {
     if(ADCChannel-CloverADCChannelLimits[i][0]==TDCChannel-CloverTDCChannelLimits[i][0])
     {
     //   printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
     if(CloverADCChannelLimits[i][0]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(CloverADCChannelLimits[i][1]==-1)result = true;
     if(CloverTDCChannelLimits[i][0]==-1)result = true;
     if(CloverTDCChannelLimits[i][1]==-1)result = true;
     }
  return result;
}
/*
int CloverDetHitNumber(int ADCChannel) //number of hit in one Clover
{
  int result = 0;
  for(int i=0;i<NumberOfClover;i++)
  {
    if(ADCChannel>=CloverADCChannelLimits[i][0] && ADCChannel<=CloverADCChannelLimits[i][1])
      {
	result = i+1;
      }
  }
printf("result = %d\n",result);
  return result;
}
*/
