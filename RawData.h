#ifndef _RAWDATA_
#define _RAWDATA_ 1

#include <TTree.h>

class RawData
{
public :
  RawData();
  virtual ~RawData();
  
private :
  int ADCValues[128];
  int TDCChannels[128*6];
  int TDCValues[128*6];
  int TDCMultiplicity;
  int SiliconTDCMultiplicity;
  
public :
  //Setters
  void SetADC(int channel, int value)	{ADCValues[channel] = value;}
  void SetTDCChannel(int entry, int channel)	{TDCChannels[entry] = channel;}
  void SetTDCValue(int entry, int value)	{TDCValues[entry] = value;}
  void SetTDCMultiplicity(int mult)		{TDCMultiplicity = mult;}
  void SetSiliconTDCMultiplicity(int mult)	{SiliconTDCMultiplicity = mult;}
  
  //Getters
  int GetADC(int channel)	{return ADCValues[channel];}
  int GetTDCChannel(int n)	{return TDCChannels[n];}
  int GetTDCValue(int n)	{return TDCValues[n];}
//int GetTDC(int channel)	{return TDCValues[channel];}

  void Init(RawData *raw);

};

RawData *RawDataDump(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);
#endif