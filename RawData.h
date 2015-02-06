#ifndef _RAWDATA_
#define _RAWDATA_ 1

#include <TTree.h>
#include <vector>

class RawData
{
public :
  RawData();
  virtual ~RawData();
  
private :
  std::vector<int> ADCValues;
  std::vector<int> TDCChannels;
  std::vector<int> TDCValues;
  
public :
  //Setters
  void SetADC(int channel, int value)	{ADCValues.at(channel) = value;}
  void SetTDCChannel(int entry, int channel)	{TDCChannels.at(entry) = channel;}
  void SetTDCValue(int entry, int value)	{TDCValues.at(entry) = value;}
  
  //Getters
  int GetADC(int channel)	{return ADCValues[channel];}
  int GetTDCChannel(int n)	{return TDCChannels[n];}
  int GetTDCValue(int n)	{return TDCValues[n];}
//int GetTDC(int channel)	{return TDCValues[channel];}

  void SetADCSize(RawData *raw, int asize);
  void SetTDCSize(RawData *raw, int tsize);

  void Init(RawData *raw);

};

RawData *RawDataDump(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);
#endif