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
void W1ParameterInit();
void CalibrationParametersInit();
void PulseLimitsInit();
void ADCInit();
void SetADCChannelCalibration(int channel, double offset, double gain);
void ADCClear();
void PrintParameters();

void ReadConfiguration();

#endif