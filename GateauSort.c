#include "GateauData.h"
#include "GateauSort.h"


extern int ***GateauTDCChannelLimits;

int StartWire = 1;

void GateauSort(int TDCHits, int *TDC_channel_import, float *TDC_value_import, GateauData *fatty)
{
  //multiTDC to get TDC info
  multiTDC mTDC;// = new multiTDC();
  mTDC.multiTDCSort(TDCHits, TDC_channel_import, TDC_value_import);
  
  
  fatty->SetWiresFired(CalcTotalWiresFired(mTDC));//using the multihit TDC stuff
  
  for(int i=0;i<2;i++)
  {
    for(int j=0;j<5;j++)
    {
      fatty->SetWiresFiredForSector(i, j, CalcWiresFiredPerSector(mTDC,i,j));//using multiTDC
      
//       fatty->SetListWiresFired(i,j,mTDC);
//       fatty->SetValueWiresFired(i,j,mTDC);
      
      fatty->SetGroupWiresFiredForSector(i, j, CalcGroupWiresFiredPerSector(mTDC,i,j));
      if(fatty->GetGroupWiresFiredForSector(i,j)>2)
      {
	fatty->SetSectorSlope(i,j,CalcSectorSlope(mTDC,i,j));
	fatty->SetSectorIntercept(i,j,CalcSectorSlope(mTDC,i,j));
	fatty->SetSectorFigureOfMerit(i,j,CalcFigureOfMerit(mTDC,i,j));
      }
      else
      {
	fatty->SetSectorSlope(i,j,-1000);
	fatty->SetSectorIntercept(i,i,-1000);
	fatty->SetSectorFigureOfMerit(i,j,-1);
      }
    }
  }
  
  //Want to now list wires fired in each sector and list the values of those fired wires
  
  
}


int CalcTotalWiresFired(multiTDC mTDC)
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

int CalcWiresFiredPerSector(multiTDC mTDC, int plane, int sector)
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

int CalcGroupWiresFiredPerSector(multiTDC mTDC, int plane, int sector)
{
 int result = 0;
 
 return result;
}

double CalcSectorSlope(multiTDC mTDC, int plane, int sector)
{
  double result = 0.;
  
  return result;
}

double CalcSectorIntercept(multiTDC mTDC, int plane, int sector)
{
  double result = 0.;
  
  return result;
}

double CalcFigureOfMerit(multiTDC mTDC, int plane, int sector)
{
  double result = 0.;
  
  return result;
}