/* Sort code for HAGAR, the NaI gamma detector
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for gamma sort codes for K600 experiments.
 * 
 * 
 */

#include "GammaData.h"
#include "HagarSort.h"
#include <math.h>
#include <stdlib.h>

extern int ADCModules;
extern int ADCsize;

extern int *HagarADCChannelLimits;
extern int *HagarTDCChannelLimits;

extern double *ADCOffsets;
extern double *ADCGains;




//GammaData *HagarSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
void HagarSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, GammaData *gammy)
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
	
	//printf("HagarADCChannelLimits[0]:  %d \t  HagarADCChannelLimits[1]:  %d \n ",HagarADCChannelLimits[0], HagarADCChannelLimits[1]);	


		 for(int i=HagarADCChannelLimits[0];i<=HagarADCChannelLimits[1];i++)
		      {
			//printf("HagarADCChannel:  %d \t  TDCChannel:  %d \n ", i, mTDC->GetChannel(k));	
	

			if(HagarADCTDCChannelCheck(i,mTDC->GetChannel(k)))
			{

		//	printf("line 49\n");
			// 	  printf("ADCChannel: %d \t TDCChannel: %d\n",i,mTDC->GetChannel(n));
	  			double GammaEnergy = HagarEnergyCalc(i,ADC_import[i]);
	  			if(GammaEnergy>0.1)
	  			{
				//printf("Hagar Hit\n");
	    			gammy->SetEnergy(GammaEnergy);
	    			gammy->SetTime(mTDC->GetValue(k));
				gammy->SetDetectorType("Hagar");
				int label=-1;
				gammy->SetDetectorLabel(label);
				//gammy->SetDetectorHit(HagarDetHitNumber(i));
	  			}
			   }
      		      }
  
   }

  gammy->SetHits(gammy->SizeOfEvent());
  mTDC->ClearEvent();
  delete mTDC;
  //return gammy;

}


double HagarEnergyCalc(int Channel, double ADCValue)
{
  //printf("Channel: %i \t ADCValue: %f\n",Channel,ADCValue);
 // printf("Offset: %f \t Gain: %f\n",ADCOffsets[Channel],ADCGains[Channel]);
  double result = ADCOffsets[Channel] + ADCGains[Channel]*ADCValue;
  return result;
}


bool HagarADCTDCChannelCheck(int ADCChannel, int TDCChannel)
{ 
  bool result = false;
 // printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
    if(ADCChannel>=HagarADCChannelLimits[0] && ADCChannel<=HagarADCChannelLimits[1] && TDCChannel>=HagarTDCChannelLimits[0] && TDCChannel<=HagarTDCChannelLimits[1])//Check to see if the ADC/TDC events are in the same detector
     {
       if(TDCChannel-865 == ADCChannel)
        {
    //    printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
          result = true;
        }
     }
  return result;
}

/*int HagarDetHitNumber(int ADCChannel)
{
  int result = 0;
  for(int i=0;i<8;i++)
  {
    if(ADCChannel>=HagarADCChannelLimits[0] && ADCChannel<=HagarADCChannelLimits[1])
      {
	result = i+1;
      }
  }
  return result;
}

*/

double HagarThetaCalc(int Channel)
{
  return 0;
}

double HagarPhiCalc(int Channel)
{
  return 0;
}
