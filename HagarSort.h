#ifndef HAGARSORT_h
#define HAGARSORT_h 1

/* Header file for the 'HAGAR' sort - channel mapping needs to be defined somewhere else... probably the Parameters.c file */

#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TROOT.h>
#include "multiTDC.h"

GammaData *HagarSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);

double HagarEnerygCalc(int Channel, double ADCValue);
double HagarThetaCalc(int Channel);
double HagarPhiCalc(int Channel);

#endif

