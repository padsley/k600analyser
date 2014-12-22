#ifndef _RAWDATA_
#define _RAWDATA_ 1

#include <TTree.h>

class RawData
{
public :
  RawData();
  virtual ~RawData();
  
private :
  int *ADCValues;//[ADCsize];//Need to set the sizes of these in a better fashion
//   int TDChits;
  int *TDCChannels;//[TDCsize];
  int *TDCValues;//[TDCsize];
  
public :
  //Setters
  void SetADC(int channel, int value)	{ADCValues[channel] = value;}
  void SetTDCChannel(int entry, int channel)	{TDCChannels[entry] = channel;}
  void SetTDCValue(int entry, int value)	{TDCValues[entry] = value;}
  
  //Getters
  int GetADC(int channel)	{return ADCValues[channel];}
  int GetTDCChannel(int n)	{return TDCChannels[n];}
  int GetTDCValue(int n)	{return TDCValues[n];}
//int GetTDC(int channel)	{return TDCValues[channel];}

  void Init(RawData *raw);

};

RawData *RawDataDump(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);
#endif