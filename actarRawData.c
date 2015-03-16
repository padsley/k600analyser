#include "actarRawData.h"


int TDCSize = 64;//should this be an extern int? this will depend on whether its a test or full on experiment
//constructor to be called
//could cause overflow error
actarRawData::actarRawData()
{

}
//destructor to be called when deleting *actarRaw which should be added at the end of f-plane.c
actarRawData::~actarRawData()
{

}

void actarRawData::init(actarRawData *actarRaw)//declare void this way since it is part of the constructor that calls the actarRawData class in the .h file
{
  actarRaw->setActarTDCSize(actarRaw, TDCSize);
  for(int i=0; i<TDCSize;i++)
    {
      actarRaw->setActarTDCChannel(i,-1);
      actarRaw->setActarTDCValue(i,0);
    }
}

void actarRawData::setActarTDCSize(actarRawData *actarRaw, int tsize)
{
  actarRaw->actarTDCValues.resize(tsize);
  actarRaw->actarTDCChannels.resize(tsize);
}

actarRawData *actarRawDataDump(int actar_ntdc, int *actar_TDC_channel_import, float *actar_TDC_value_import)
{
  actarRawData *actarRaw = new actarRawData();
  actarRaw->init(actarRaw);
  for(int i=0;i<actar_ntdc;i++)
    {
      actarRaw->setActarTDCChannel(i,actar_TDC_channel_import[i]);
      actarRaw->setActarTDCValue(i,actar_TDC_value_import[i]);
    }
}
