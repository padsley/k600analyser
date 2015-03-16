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

extern int HagarADCChannels[7];
extern int HagarTDCChannels[7];


//=======================================
GammaData *HagarGammaSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  GammaData *gam = new GammaData();
  multiTDC *mTDC = new multiTDC(ntdc,TDC_channel_import,TDC_value_import);

  float ADCs[7] = {0,0,0,0,0,0,0};
  float TDCs[7] = {0,0,0,0,0,0,0};
  double energyhagar;  
  float tmp=22.;

  //for(int i=0;i<ADCsize;i++){

  for(int i=HagarADCChannels[0];i<=HagarADCChannels[6];i++) {
    //printf("ADC info: ADCchan %i   value   %f \n",i,ADC_import[i] );
    for(int j=0;j<7;j++) {
      if(i==HagarADCChannels[j]) {
	ADCs[j] = ADC_import[i];
        //gam->SetGammaADCValue(tmp);
        gam->SetGammaADCValue(ADC_import[i]);
        gam->SetGammaADCChannel(j);
	//printf("ADC info supposedly for HAGAR: ADCchan %i HAGARchan %i   %f  %f\n",i,j,ADCs[j],ADC_import[i] );
	//printf("HAGAR: ADCchan/i %i HAGARchan/j %i     ",i,j );
	//printf("HAGAR: %f  %f \n",ADC_import[i], gam->GetADCValue(j) );
	//printf("HAGAR: %f  %i \n",ADC_import[i], gam->GetADCChannel(j) );
      }
    }
  }
  energyhagar=HagarEnergyCalc(ADCs);
  gam->SetEnergy(energyhagar);

  for(int i=0;i<mTDC->GetSize();i++)  {
    if(TDC_channel_import[i]>650 && TDC_channel_import[i]<670){//limit code to only deal with hagar timing
       for(int j=0;j<7;j++){
         if(mTDC->GetChannel(i) == HagarTDCChannels[j]){
	   TDCs[j] = mTDC->GetValue(i);
           gam->SetGammaTDCValue(TDCs[j]);
           gam->SetGammaTDCChannel(j);
         }
       }
    }
  }
 
  //gam->ClearEvent();
  mTDC->ClearEvent();
  delete mTDC;

  return gam;
}





//=======================================
double HagarEnergyCalc(float ADCs[7])
{
    extern double HagarPedestal[7];
    extern double HagarGain[7];
    extern double HagarOffset[7];
    double result = 0;
    for(int i=0;i<7;i++)
    {
   	if(ADCs[i]>HagarPedestal[i]){
      		result += HagarOffset[i] + HagarGain[i] * ADCs[i];
	}
    }
    result /= 7;
    return result;
}


//=======================================
double HagarThetaCalc(int Channel)
{
  return 0;
}


//=======================================
double HagarPhiCalc(int Channel)
{
  return 0;
}
