#ifndef __multiTDC__
#define __multiTDC__ 1

/* This header file defines the 'multiTDC' class. This is a class used to store the information about TDC values for ancillary detectors in K600 experiments, given that the V1190 TDCs can (and do!) run in multihit mode.
 * 
 * * Author: Philip Adsley, July 2014
 * E-mail: padsley@gmail.com
 * 
 * Please contact me if you find errors or have useful suggestions
 * 
 */

#include <vector>
#include <stdio.h>

extern int* PulseLimits;

class multiTDC
{
public :
  multiTDC();
  void multiTDCSort(int ntdc, int *TDC_channel_import, float *TDC_value_import);
  virtual ~multiTDC();
  
  private :
  //The actual members of the class
  unsigned int TDChits;
  
  std::vector<int> TDCchannels;
  std::vector<double> TDCvalues;
  std::vector<int> TDCmults;//multiplicities of each TDC event in that original channel -- added as a diagnostic thing
public :
  //Setters - set the physical values for each physical event
  void SetHits(int hits)		{TDChits = hits;}
  void SetChannel(int channel)		{TDCchannels.push_back(channel);}
  void SetValue(int value)		{TDCvalues.push_back(value);}
  void SetMult(int mult)		{TDCmults.push_back(mult);}
  //Getters
  int GetSize()				{return SizeOfEvent();}
  int GetChannel(int i)			{return TDCchannels.at(i);}
  double GetValue(int i)		{return TDCvalues.at(i);}
  int GetMult(int i)			{return TDCmults.at(i);}
  
  void PrintEvent();
  bool TestEvent();
  void ClearEvent();
  
  unsigned int SizeOfEvent();
};

#endif
