#ifndef Hagar_h
#define Hagar_h 1

/*
 * 
 * This is a header file defining the data structure for HAGAR events for K600 analysis.
 * 
 * Author: Philip Adsley, June 2014
 * E-mail: padsley@gmail.com
 * 
 * Please contact me if you find errors or have useful suggestions
 * 
 */

#include "multiTDC.h"

class HagarData
{
 public :
  HagarData();
  virtual ~HagarData();

 private :
  int ADCHagar[7];
  int TDCHagar[7];  
  double HagarEnergy;

 public :
  void SetADCValues(float ADCs[7])
  {
    for(int i=0;i<7;i++)
    {
      ADCHagar[i] = ADCs[i];
    }
  }
  
  void SetTDCValues(float TDCs[7])
  {
    for(int i=0;i<7;i++)
    {
      TDCHagar[i] = TDCs[i];
    }
  }
  
  void SetEnergy(float ADCs[7]);  
  void PrintEvent();
};



HagarData* HagarDataSort(float *ADC_import, int ntdc, int *TDC_channel_import,  float *TDC_value_import);


#endif
