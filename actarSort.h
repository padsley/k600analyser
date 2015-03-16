#ifndef _actarSort_
#define _actarSort_ 1         //why is there a 1?

#include <TTree.h>
#include <vector>
#include <stdio.h>
#include "actarRawData.h"

class actarSort
{
 public :
  actarSort();
  virtual ~actarSort();

 private : 
  static const int channels = 64;

 private :
  std::vector<int>actarTDCChannels;
  std::vector<int>actarTDCValues;
  std::vector<int>actarPulserTDCChannels;
  std::vector<int>actarPulserTDCValues;

  void setPulserTDCChannel(int entry, int channel)
  {

  }

  void setPulserTDCValue(int entry, int value)
  {

  }

 public :

  void setPulserData(int entry, int channel, int value)
  {
    setPulserTDCChannel(entry, channel);
    setPulserTDCValue(entry, value);
  }

  //void 

  void getPulserData(int n)
  {
    return actarPulser;
  }

  void setSourceData(int channel)
  {}











    };

#endif
