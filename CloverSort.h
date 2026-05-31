#ifndef CLOVERSORT_h
#define CLOVERSORT_h 1

/* Header file for the 'HAGAR' sort - channel mapping needs to be defined somewhere else... probably the Parameters.c file */

#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TROOT.h>
#include <cmath>
#include "multiTDC.h"

//GammaData *CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);
void CloverSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, GammaData *gammy);

double CloverEnergyCalc(int Channel, double ADCValue);
//double CloverThetaCalc(int Channel);
//double CloverPhiCalc(int Channel);


//bool MMMSuppressChannel(int Channel);
//bool MMMFrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si);
//bool MMMADCTDCChannelTestPSide(int ADCChannel, int TDCChannel);
//bool MMMADCTDCChannelTestNSide(int ADCChannel, int TDCChannel);
//void MMMInit();
//void MMMLoadCuts(SiliconData *si);

int CloverDetHitNumber(int ADCChannel);
bool CloverADCTDCChannelCheck(int ADCChannel, int TDCChannel);

//int MMMStripFront(int FrontChannel);
//int MMMStripBack(int BackChannel);

bool CloverTDCTest(int TDCChannel);
//bool MMMTDCFrontTest(int TDCChannel);
//bool MMMTDCBackTest(int TDCChannel);
//bool MMMTDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel);

int CloverTDCIdentifyDetector(int TDCChannel);
int CloverTDCIdentifySegment(int TDCChannel);

#endif

