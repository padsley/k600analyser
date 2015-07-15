/* Sort code for HAGAR, the NaI gamma detector
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for gamma sort codes for K600 experiments.
 * 
 * 
 */

#include "GammaData.h"
#include "HagarSort.h"
#include <math.h>
#include <stdlib.h>

GammaData *HagarSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  GammaData *gammy = new GammaData();
  multiTDC *mTDC;
mTDC->multiTDCSort(ntdc,TDC_channel_import,TDC_value_import);

  //Need to implement the sort algorithm here
  return gammy;
}

double HagarThetaCalc(int Channel)
{
  return 0;
}

double HagarPhiCalc(int Channel)
{
  return 0;
}
