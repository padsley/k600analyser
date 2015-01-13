/* Sort code for HAGAR
 *
 *
 */ 

#include "HagarData.h"

extern int ADCsize;   // already declared in MMM.c

extern int HagarADCChannels[7];
extern int HagarTDCChannels[7];


//=======================================
HagarData::HagarData()
{
}


//=======================================
HagarData::~HagarData()
{
//   printf("Destructor called for HagarData");
}


//=======================================
void HagarData::PrintEvent()
{
      printf("HagarData::PrintEvent not yet implemented");
}



//=======================================
HagarData* HagarDataSort(float *ADC_import, int ntdc, int *TDC_channel_import,  float *TDC_value_import)
{
  HagarData *hag = new HagarData();
  multiTDC *mTDC = new multiTDC;
  mTDC->multiTDC(ntdc, TDC_channel_import, TDC_value_import);
  
  float ADCs[7] = {0,0,0,0,0,0,0};
  float TDCs[7] = {0,0,0,0,0,0,0};
  
  //for(int i=0;i<ADCsize;i++){
  //printf("ADC info: ADCchan %i   value   %f \n",i,ADC_import[i] );

  for(int i=HagarADCChannels[0];i<HagarADCChannels[6];i++){
    for(int j=0;j<7;j++){
      if(i==HagarADCChannels[j]){
	ADCs[j] = ADC_import[i];
	//printf("ADC info supposedly for HAGAR: ADCchan %i HAGARchan %i   %f  %f\n",i,j,ADCs[j],ADC_import[i] );
      }
    }
  }
  hag->SetADCValues(ADCs);
 
  for(int i=0;i<mTDC->GetSize();i++){
    if(TDC_channel_import[i]>650 && TDC_channel_import[i]<670){//limit code to only deal with hagar timing
       for(int j=0;j<7;j++){
         if(mTDC->GetChannel(i) == HagarTDCChannels[j]){
	   TDCs[j] = mTDC->GetValue(i);
         }
       }
    }
  }
  hag->SetTDCValues(TDCs);

  hag->SetEnergy(ADCs);
  
  return hag;
}


//=======================================
void HagarData::SetEnergy(float ADCs[7])
{
    extern double HagarGain[7];
    extern double HagarOffset[7];
    double result = 0;
    for(int i=0;i<7;i++)
    {
      result += HagarOffset[i] + HagarGain[i] * ADCs[i];
    }
    result /= 7;
    HagarEnergy = result;
}



