#ifndef _actarRawData_
#define _actarRawData_ 1

#include <TTree.h>
#include <vector>

class actarRawData //.h file is the definition file whereas .c file is where everything is processed and in turn called by a master class like f-plane.c
{
 public :
  actarRawData();
  virtual ~actarRawData();

 private :
  std::vector<int>actarTDCChannels;
  std::vector<int>actarTDCValues;

 public :
  //Set relevant values from actarRawData.c that is called from f-plane.c
  void setActarTDCChannel(int entry, int channel){actarTDCChannels.at(entry)=channel;}
  void setActarTDCValue(int entry, int value){actarTDCValues.at(entry)=value;}

  //Getting relevant data from f-plane.c
  int getActarTDCValue(int n){return actarTDCValues[n];}
  int getActarTDCChannel(int n){return actarTDCChannels[n];}

  void setActarTDCSize(actarRawData *actarRaw, int tsize);

  void init(actarRawData *actarRaw);
};

actarRawData *actarRawDataDump(int actar_ntdc, int *actar_TDC_channel_import, float *actar_TDC_value_import);
#endif
