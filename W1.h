#ifndef W1_h
#define W1_h 1

/* Header file for the code sorting W1 data into something reasonable and sensible. 
 * Author: Phil Adsley
 * August 2014
 * padsley@gmail.com for queries/comments
 */

#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TROOT.h>
#include "multiTDC.h"

SiliconData *W1SiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);

bool W1SuppressChannel(int Channel);
double W1EnergyCalc(int Channel, double ADCValue);
double W1ThetaCalc(int FrontChannel, int BackChannel);
double W1PhiCalc(int FrontChannel, int BackChannel);
bool W1FrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si);
bool W1ADCTDCChannelTestPSide(int ADCChannel, int TDCChannel);
bool W1ADCTDCChannelTestNSide(int ADCChannel, int TDCChannel);

void W1Init();
void W1LoadCuts(SiliconData *si);

int W1DetHitNumber(int FrontChannel, int BackChannel);

int W1StripFront(int FrontChannel);
int W1StripBack(int BackChannel);

// New functions to optimise sorting
bool W1TDCFrontTest(int TDCChannel);
bool W1TDCBackTest(int TDCChannel);
bool W1TDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel);
int W1TDCIdentifyDetector(int TDCFrontChannel, int TDCBackChannel);

#endif
