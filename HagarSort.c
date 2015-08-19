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




GammaData *HagarSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  GammaData *gammy = new GammaData();
  //Loop over ADC and TDC events and do the following:
  //Check whether there are front-back coincidences for a detector and test the energies
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - TDCs are apparently in single-hit mode. Don't need to worry about multihits
  //Calculate the theta and phi for the valid events
  multiTDC *mTDC = new multiTDC;
  mTDC->multiTDCSort(ntdc, TDC_channel_import, TDC_value_import);	

  for(int k=0;k<mTDC->GetSize();k++)//Loop over all of the TDC values - there should only be a small number of these relative to the ADC values
  {
	//printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
	//printf("HagarADCChannelLimits[0]:  HagarADCChannelLimits[1]: ",HagarADCChannelLimits[0], HagarADCChannelLimits[0]);	


		   for(int i=HagarADCChannelLimits[0];i<=HagarADCChannelLimits[1];i++)
		      {
			if(HagarADCTDCChannelCheck(i,mTDC->GetChannel(k)))
			{
			// 	  printf("ADCChannel: %d \t TDCChannel: %d\n",i,mTDC->GetChannel(n));
	  			double GammaEnergy = HagarEnergyCalc(i,ADC_import[i]);
	  			if(GammaEnergy>0.1)
	  			{
	    			gammy->SetEnergy(GammaEnergy);
	    			gammy->SetTime(mTDC->GetValue(k));
				//gammy->SetDetectorHit(HagarDetHitNumber(i));
	  			}
			   }
      		      }
  
   }

  mTDC->ClearEvent();
  delete mTDC;
  return gammy;

}


double HagarEnergyCalc(int Channel, double ADCValue)
{
  //define the calibration parameters
  double result = ADCOffsets[Channel] + ADCGains[Channel]*ADCValue;
//   printf("Energy = ADCOffsets[%d] + ADCGains[%d] * ADCValue\t %f = %f + %f * %f\n",Channel,Channel,result,ADCOffsets[Channel],ADCGains[Channel],ADCValue);
  //   if(Channel<64 || Channel>=80 && Channel<112)printf("EnergyCalc: %g\n",result);
  return result;
}

/*
bool HagarADCTDCChannelCheck(int ADCChannel, int TDCChannel)
{ 
  bool result = false;
  printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
    if(ADCChannel>=HagarADCChannelLimits[0] && ADCChannel<=HagarADCChannelLimits[1] && TDCChannel>=HagarTDCChannelLimits[0] && TDCChannel<=HagarTDCChannelLimits[1])//Check to see if the ADC/TDC events are in the same detector
     {
       if(ADCChannel%8==TDCChannel%8)
        {
//        printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
          result = true;
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
