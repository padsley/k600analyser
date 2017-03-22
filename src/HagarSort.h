#ifndef HAGARSORT_h
#define HAGARSORT_h 1

/* Header file for the 'HAGAR' sort - channel mapping needs to be defined somewhere else... probably the Parameters.c file */

#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TROOT.h>
#include <cmath>
#include "multiTDC.h"

//GammaData *HagarSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);
void HagarSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, GammaData *gammy);

double HagarEnergyCalc(int Channel, double ADCValue);
//double HagarThetaCalc(int Channel);
//double HagarPhiCalc(int Channel);


//bool MMMSuppressChannel(int Channel);
//bool MMMFrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si);
//bool MMMADCTDCChannelTestPSide(int ADCChannel, int TDCChannel);
//bool MMMADCTDCChannelTestNSide(int ADCChannel, int TDCChannel);
//void MMMInit();
//void MMMLoadCuts(SiliconData *si);

int HagarDetHitNumber(int ADCChannel);
bool HagarADCTDCChannelCheck(int ADCChannel, int TDCChannel);

//int MMMStripFront(int FrontChannel);
//int MMMStripBack(int BackChannel);

bool HagarTDCTest(int TDCChannel);
//bool MMMTDCFrontTest(int TDCChannel);
//bool MMMTDCBackTest(int TDCChannel);
//bool MMMTDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel);

int HagarTDCIdentifyDetector(int TDCChannel);

#endif

