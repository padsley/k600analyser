#include "GateauData.h"
#include "GateauSort.h"


extern int ***GateauTDCChannelLimits;

int StartWire = 1;

void GateauSort(int TDCHits, int *TDC_channel_import, float *TDC_value_import, GateauData *fatty)
{
  //multiTDC to get TDC info
  multiTDC mTDC;// = new multiTDC();
  mTDC.multiTDCSort(TDCHits, TDC_channel_import, TDC_value_import);
  
  
  fatty->SetWiresFired(GetTotalWiresFired(mTDC));//using the multihit TDC stuff
  
  for(int i=0;i<2;i++)
  {
    for(int j=0;j<5;j++)
    {
      fatty->SetWiresFiredForSector(i, j, GetWiresFiredPerSector(mTDC,i,j));//using multiTDC
    }
  }
  
  
  
  
}


int GetTotalWiresFired(multiTDC mTDC)
{
  int result = 0;
  
  //Loop over TDC entries and count
  for(int k=0;k<mTDC.GetSize();k++)//Loop over all of the TDC values - there should only be a small number of these relative to the ADC values
      {
	for(int i=0;i<2;i++)
	{
	  for(int j=0;j<5;j++)
	  {
	    if(mTDC.GetChannel(k) > GateauTDCChannelLimits[i][j][0] && mTDC.GetChannel(k) < GateauTDCChannelLimits[i][j][1])
	    {
	      result++;
	    }
	  }
	}
      }
      return result;
}

int GetWiresFiredPerSector(multiTDC mTDC, int plane, int sector)
{
  int result = 0;
  
  for(int k=0;k<mTDC.GetSize();k++)
  {
    if(mTDC.GetChannel(k) > GateauTDCChannelLimits[plane][sector][0] && GateauTDCChannelLimits[plane][sector][1])
    {
      result++;
    }
  }
  return result;
}