#include "RawData.h"

extern int ADCsize;
extern int TDCsize;

RawData::RawData()
{
  
}

RawData::~RawData()
{
  
}

void RawData::Init(RawData *raw)
{
    raw->SetADCSize(raw, ADCsize);
    raw->SetTDCSize(raw, TDCsize);
    for(int i=0;i<ADCsize;i++)raw->SetADC(i,0);
    
    for(int i=0;i<TDCsize;i++)
    {
      raw->SetTDCChannel(i,-1);
      raw->SetTDCValue(i,0);
    }
}

void RawData::SetADCSize(RawData *raw, int asize)
{
  raw->ADCValues.resize(asize);
}

void RawData::SetTDCSize(RawData *raw, int tsize)
{
  raw->TDCValues.resize(tsize);
  raw->TDCChannels.resize(tsize);
}

RawData *RawDataDump(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  RawData *raw = new RawData();
  raw->Init(raw);
  for(int i=0;i<ADCsize;i++)raw->SetADC(i,ADC_import[i]);
  
  for(int n=0;n<ntdc;n++)
  {
    raw->SetTDCChannel(n,TDC_channel_import[n]);
    raw->SetTDCValue(n,TDC_value_import[n]);
  }
  
  return raw;
}