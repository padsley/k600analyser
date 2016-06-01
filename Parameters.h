#ifndef PARAMETER_H
#define PARAMETER_H 1

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>

// Originally declared as constants, sinc I tried to make a distinction
// between things one can change and things one should not change at all ... RN

const double X_WIRESPACE = 4.0;     // distance (mm) between signal wires
const double DRIFTLENGTH = 8.0;     // max distance (mm) the electrons can drift
const double U_WIRE_ANGLE=50.0;     // angle of wires in U-plane wrt to horizontal
const int X_WIRES=208;
const int U_WIRES=145;
const int TDC_MAX_TIME=14999;
const int TDC_MIN_TIME=0;
const int TDC_N_BINS=14999;
const int TOF_TDC_CHAN=1;             // ch1 in 1st TDC; i.e. the 2nd channel     
const int MAX_WIRES_PER_EVENT = 400;  // If more wires than this fire, the event data was bad  -- RN random choice  -- 
				      // to be replaced by something else?!?! really only used for array definition
				      // MUST FIND A BETTER WAY ... THIS VARIABLE NOT GOOD?
const int LUT_CHANNELS=9000;
const int TDC_CHAN_PULSER=2;      // A pulser PR, in absence of pattern register unit
const int TDC_CHAN_POLU=4;        // May 2016: this was last used in PR153 Oct 2010
const int TDC_CHAN_POLD=5;        // May 2016: this was last used in PR153 Oct 2010




//-------------------------------------------------------------------------------------
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

void CloverNumberInit();
void CloverADCChannelsInit(int det, int start, int stop);
void CloverTDCChannelsInit(int det, int start, int stop);

void CalibrationParametersInit();
void PulseLimitsInit();

void ADCInit();
void SetADCChannelCalibration(int channel, double offset, double gain);
void ADCClear();

void ReadCalibrationParameters(std::string CalibFile);

void TDCInit();
void TDCOffsetsClear();
void ReadTDCOffsets(std::string OffsetsFile);

void QDCInit();
void QDCClear();

void PrintParameters();
void ReadConfiguration();
void LoadExCorrection(int run);

#endif
