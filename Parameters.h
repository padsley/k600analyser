#ifndef PARAMETER_H
#define PARAMETER_H 1

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>

void ParameterInit();
void MMMNumberInit();
void MMMADCChannelsInit(int det, std::string side, int start, int stop);
void MMMTDCChannelsInit(int det, std::string side, int start, int stop);
void W1NumberInit();
void W1ADCChannelsInit(int det, std::string side, int start, int stop);
void W1TDCChannelsInit(int det, std::string side, int start, int stop);
void HagarInit();
void HagarADCChannelsInit(int start, int stop);
void HagarTDCChannelsInit(int start, int stop);
void CalibrationParametersInit();
void PulseLimitsInit();
void ADCInit();
void SetADCChannelCalibration(int channel, double offset, double gain);
void ADCClear();
void QDCInit();
void QDCClear();
void PrintParameters();

void ReadConfiguration();

#endif
