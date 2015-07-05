#ifndef MMM_h
#define MMM_h 1
/* Header file for the 'MMM' silicon sort - you need to define the channel mappings for the different detectors in here as well as give positions, angle information, calibration parameters and Stuff Like That.
 *  Also in this file as a series of Boolean functions that test various things, such as whether a given ADC channel is a front or back of a silicon detector etc.

*/ 
 
#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TROOT.h>
#include <cmath>
#include "multiTDC.h"

SiliconData *MMMSiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import);

bool MMMSuppressChannel(int Channel);
double MMMEnergyCalc(int Channel, double ADCValue);
double MMMThetaCalc(int Channel);
double MMMPhiCalc(int Channel);
bool MMMFrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si);
bool MMMADCTDCChannelTestPSide(int ADCChannel, int TDCChannel);
bool MMMADCTDCChannelTestNSide(int ADCChannel, int TDCChannel);


void MMMInit();
void MMMLoadCuts(SiliconData *si);

int MMMDetHitNumber(int FrontChannel, int BackChannel);

int MMMStripFront(int FrontChannel);
int MMMStripBack(int BackChannel);

bool MMMTDCFrontTest(int TDCChannel);
bool MMMTDCBackTest(int TDCChannel);
bool MMMTDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel);

int MMMTDCIdentifyDetector(int TDCFrontChannel, int TDCBackChannel);

#endif
