#ifndef SCINTILLATORSORT_h
#define SCINTILLATORSORT_h 1

/* Header file for the 'HAGAR' sort - channel mapping needs to be defined somewhere else... probably the Parameters.c file */

#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TROOT.h>
#include <cmath>
#include "multiTDC.h"

//GammaData *ScintillatorSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);
void ScintillatorSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, GammaData *gammy);

double ScintillatorEnergyCalc(int Channel, double ADCValue);
//double ScintillatorThetaCalc(int Channel);
//double ScintillatorPhiCalc(int Channel);


//bool MMMSuppressChannel(int Channel);
//bool MMMFrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si);
//bool MMMADCTDCChannelTestPSide(int ADCChannel, int TDCChannel);
//bool MMMADCTDCChannelTestNSide(int ADCChannel, int TDCChannel);
//void MMMInit();
//void MMMLoadCuts(SiliconData *si);

int ScintillatorDetHitNumber(int ADCChannel);
bool ScintillatorADCTDCChannelCheck(int ADCChannel, int TDCChannel);

//int MMMStripFront(int FrontChannel);
//int MMMStripBack(int BackChannel);

bool ScintillatorTDCTest(int TDCChannel);
//bool MMMTDCFrontTest(int TDCChannel);
//bool MMMTDCBackTest(int TDCChannel);
//bool MMMTDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel);

int ScintillatorTDCIdentifyDetector(int TDCChannel);
int ScintillatorTDCIdentifySegment(int TDCChannel);

#endif

