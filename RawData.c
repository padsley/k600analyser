#include "RawData.h"


RawData::RawData()
{
  
}

RawData::~RawData()
{
  
}

void RawData::Init(RawData *raw)
{
    for(int i=0;i<128;i++)raw->SetADC(i,0);
    
    for(int i=0;i<6*128;i++)
    {
      raw->SetTDCChannel(i,-1);
      raw->SetTDCValue(i,0);
    }
}

RawData *RawDataDump(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  RawData *raw = new RawData();
  raw->Init(raw);
  for(int i=0;i<128;i++)raw->SetADC(i,ADC_import[i]);
  int counter = 0;
  
  for(int i=0;i<7*128;i++)
    {
      
    }
  
  for(int n=0;n<ntdc;n++)
  {
    raw->SetTDCChannel(n,TDC_channel_import[n]);
    raw->SetTDCValue(n,TDC_value_import[n]);
    if(TDC_channel_import[n]>=6*128+48)counter++;
  }
  
  raw->SetTDCMultiplicity(ntdc);
  raw->SetSiliconTDCMultiplicity(counter);
  
  
  return raw;
}
