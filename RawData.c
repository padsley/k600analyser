#include "RawData.h"

extern int ADCsize;
extern int TDCsize;
extern int QDCsize;

extern double *ADCOffsets;
extern double *ADCGains;

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
    raw->SetQDCSize(raw, QDCsize);
    for(int i=0;i<ADCsize;i++)raw->SetADCValue(i,0);
    for(int i=0;i<ADCsize;i++)raw->SetADCCalibratedValue(i,0);
    
    for(int i=0;i<TDCsize;i++)
    {
      raw->SetTDCChannel(i,-1);
      raw->SetTDCValue(i,0);
    }
    
    for(int i=0;i<QDCsize;i++)
    {
      raw->SetQDCValue(i,0);
    }
}

void RawData::SetADCSize(RawData *raw, int asize)
{
  raw->ADCValues.resize(asize);
  raw->ADCCalibratedValues.resize(asize);
}

void RawData::SetTDCSize(RawData *raw, int tsize)
{
  raw->TDCValues.resize(tsize);
  raw->TDCChannels.resize(tsize);
}

void RawData::SetQDCSize(RawData *raw, int qsize)
{
  raw->QDCValues.resize(qsize);
}

RawData *RawDataDump(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, float *QDC_import)
{
  RawData *raw = new RawData();
  raw->Init(raw);
  for(int i=0;i<ADCsize;i++)raw->SetADCValue(i,ADC_import[i]);

  for(int i=0;i<ADCsize;i++)raw->SetADCCalibratedValue(i,ADCOffsets[i] + ADCGains[i] * ADC_import[i]);
  
  for(int n=0;n<ntdc;n++)
  {
    raw->SetTDCChannel(n,TDC_channel_import[n]);
    raw->SetTDCValue(n,TDC_value_import[n]);
  }
  
  for(int i=0;i<QDCsize;i++)raw->SetQDCValue(i,QDC_import[i]);
  return raw;
}
