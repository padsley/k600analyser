#include "PR194CloverSort.h"
// #include "CloverData.h"

extern int ADCsize;

const double gain[4] = {0.0047450, 0.0044951, 0.0045365, 0.0037823};//These are gains and offsets for the clovers in this experiment. Should probably rename them and move them somewhere more helpful but I haven't got around to that yet.
const double offset[4] = {-0.664321, -0.6179, -0.6484, -0.5685};

CloverData *PR194CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
//PR194CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, CloverData *clov)
{
    multiTDC *mTDC = new multiTDC;
    mTDC->multiTDCSort(ntdc, TDC_channel_import, TDC_value_import);
    CloverData *clov = new CloverData();
    
    for(int n=0;n<mTDC->GetSize();n++)//Loop over TDC values
    {
      for(int i=0;i<ADCsize;i++)
      {
	if(i>=24 && i<=31 && ADCTDCChannelCheck(i,mTDC->GetChannel(n)))
	{
// 	  printf("ADCChannel: %d \t TDCChannel: %d\n",i,mTDC->GetChannel(n));
	  double GammaEnergy = GermaniumEnergyCalc(i,ADC_import[i]);
	  if(GammaEnergy>0.1)
	  {
	    clov->SetEnergy(GammaEnergy);
	    clov->SetTime(0);
	    if(i<=27)clov->SetDetectorHit(1);
	    else if(i>=28)clov->SetDetectorHit(2);
	    else clov->SetDetectorHit(-1);
	  }
	}
      }
    }
  
//   clov->TestEvent();
  mTDC->ClearEvent();
  delete mTDC;
  return clov;
}

double GermaniumEnergyCalc(int Channel, double ADCValue)
{
  double result = offset[Channel-24] + gain[Channel-24] * ADCValue;
    return result;
}

bool ADCTDCChannelCheck(int ADCChannel, int TDCChannel)
{
  bool result = false;
  if(TDCChannel - 6*128 - 120 == ADCChannel - 24)result = true;
  return result;
}