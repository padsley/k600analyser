#ifndef __PR194CloverSort__
#define __PR194CloverSort__ 1

/* Header file for a CloverSort algorithm for the PR194 experiment. Intented to be a prototype for future CloverSort algorithms (so the user (YOU!) will likely need to change ADC channel addresses etc.
 * 
 * Author: Padsley, July 2014
 * E-mail: padsley@gmail.com
 * 
 * Let me know if you find mistakes/can suggest corrections
 */

#include <vector>
#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include "multiTDC.h"
#include "CloverData.h"

CloverData *PR194CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);

double GermaniumEnergyCalc(int Channel, double ADCValue);

bool ADCTDCChannelCheck(int ADCChannel, int TDCChannel);

#endif