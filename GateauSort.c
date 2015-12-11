#include "GateauData.h"
#include "GateauSort.h"


extern int ***GateauTDCChannelLimits;

int StartWire = 0;//C numbering

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
	double *FitResult = FitSectorData(mTDC,i,j,fatty->GetGroupWiresFiredForSector(i,j));
	fatty->SetSectorSlope(i,j,FitResult[0]);
	fatty->SetSectorIntercept(i,j,FitResult[1]);
	fatty->SetSectorFigureOfMerit(i,j,FitResult[2]);
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
  
  bool TestFired[16];
  for(int i=0;i<16;i++)TestFired[i] = false;
  
  for(int k=0;k<mTDC.GetSize();k++)
  {
    if(mTDC.GetChannel(k) > GateauTDCChannelLimits[plane][sector][0] && mTDC.GetChannel(k) < GateauTDCChannelLimits[plane][sector][1])
    {
      TestFired[mTDC.GetChannel(k) - GateauTDCChannelLimits[plane][sector][0]] = true;
    }
  }
  
  bool AllFired = true;
  
  int i = StartWire;
  
  while (i<16 && AllFired)
  {
    result++;
    if(!TestFired[i])AllFired = false;
    i++;
  }
  
  return result;//This should be a list of the number of wires fired starting from the middle wire. Hopefully.
}

double *FitSectorData(multiTDC mTDC, int plane, int sector, int WireCounter)
{
  double *result = new double[3];//0 is slope, 1 is intercept, 2 is Figure of Merit
  result[0] = 0; result[1] = 0; result[2] = 0;
  
  if(WireCounter>2)
  {
    int *wires = new int[WireCounter];
    int *values = new int[WireCounter];
    
    for(int k=0;k<mTDC.GetSize();k++)
    {
      if(mTDC.GetChannel(k) > GateauTDCChannelLimits[plane][sector][0] && mTDC.GetChannel(k) < GateauTDCChannelLimits[plane][sector][1])
      {
	wires[mTDC.GetChannel(k) - GateauTDCChannelLimits[plane][sector][0] - StartWire] = mTDC.GetChannel(k) - GateauTDCChannelLimits[plane][sector][0];//This should set the wire number to be the number of the wire fired in the sector:
	//Get the index from the start channel: TDCchannel - channel_limts = number of the wire in sector starting at zero
	//Then -StartWire gives the number of the index from the start wire (i.e. the first wire to fix with index zero should be the StartWire
	//Then the value is set to be the wire number in the sector
	values[mTDC.GetChannel(k) - GateauTDCChannelLimits[plane][sector][0]] = mTDC.GetValue(k);
      }
    }
    
    double sumx = 0., sumx2 = 0., sumxy = 0., sumy = 0., sumy2 = 0.;
    
    //Now we want to compute the fit parameters: using least-squares
    for(int i=0;i<WireCounter;i++)
    {
     sumx += wires[i];
     sumx2 += pow(wires[i],2.);
     sumxy += wires[i] * values[i];
     sumy += values[i];
     sumy2 += pow(values[i],2.);
    }
    
    double denom = WireCounter * sumx2 - pow(sumx,2.);
    
    if(denom==0)printf("Can't do the track fitting for GATEAU\n");
    
    result[0] = (WireCounter * sumxy - sumx * sumy)/denom;//Slope
    result[1] = (sumy*sumx2 - sumx*sumxy)/denom;//Intercept
    result[2] = (sumxy - sumx*sumy/WireCounter) / sqrt((sumx2 - pow(sumx,2.)/WireCounter) * (sumy2 - pow(sumy,2.)/WireCounter));
  }
}