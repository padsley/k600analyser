#include "HagarData.h"

//const int ADCsize = 160;
extern int ADCsize;
extern int HagarADCChannels[7];
extern int HagarTDCChannels[7];

extern double HagarGain[7];
extern double HagarOffset[7];
HagarData::HagarData()
{
}

HagarData::~HagarData()
{
}

void HagarData::PrintEvent()
{
      printf("HagarData::PrintEvent not yet implemented");
}

void HagarData::SetEnergy(float ADCs[7])
{
    double result = 0;
    for(int i=0;i<7;i++)
    {
      result += HagarOffset[i] + HagarGain[i] * ADCs[i];
    }
    result /= 7;
    HagarEnergy = result;
  }

HagarData* HagarDataSort(float *ADC_import, int ntdc, int *TDC_channel_import,  float *TDC_value_import)
{
  HagarData *hag = new HagarData();
  multiTDC *mTDC = new multiTDC(ntdc, TDC_channel_import, TDC_value_import);
  
  float ADCs[7] = {0,0,0,0,0,0,0};
  float TDCs[7] = {0,0,0,0,0,0,0};
  
  for(int i=0;i<ADCsize;i++)
  {
    for(int j=0;j<7;j++)
    {
      if(i==HagarADCChannels[j])
      {
	ADCs[j] = ADC_import[i];
      }
    }
  }
  hag->SetADCValues(ADCs);
 
  for(int i=0;i<mTDC->GetSize();i++)
  {
    for(int j=0;j<7;j++)
    {
      if(mTDC->GetChannel(i) == HagarTDCChannels[j])
      {
	TDCs[j] = mTDC->GetValue(i);
      }
    }
  }
  hag->SetTDCValues(TDCs);

  hag->SetEnergy(ADCs);
  
  return hag;
}