#include "Parameters.h"

int ADCModules = 5;

int ADCsize = 32*ADCModules;

float *ADC;

int TDCModules = 7;//This was set to 6 which, I think, is the number of FP TDCs - PA

int TDCsize = TDCModules*128;

int NumberOfMMM = 4;

// int MMMADCChannelLimits[4][4] = {{0, 15, 80, 87},{0+16, 15+16, 80+8, 87+8},{0+32, 15+32, 80+16, 87+16},{0+48, 15+48, 80+24, 87+24}};
//  int MMMTDCChannelLimits[4][2] = {{6*128+48,128*6+48+15},{128*6+48+16,128*6+48+31},{128*6+48+32,128*6+48+47},{128*6+48+48,128*6+48+63}};

int **MMMADCChannelLimits;
int **MMMTDCChannelLimits;

int NumberofW1 = 0;

int **W1ADCChannelLimits;
int **W1TDCChannelLimits;

// int W1ADCChannelLimits[4][4] = {{0,15,64,79},{0+16,15+16,64+16,79+16},{0+32,15+32,64+32,79+32},{0+48,15+48,64+48,79+48}};
// int W1TDCChannelLimits[4][4] = {{6*128+64,6*128+64+15},{6*128+64+16,6*128+64+15+16},{6*128+64+32,6*128+64+15+32},{6*128+64+48,6*128+64+15+48}};

int HagarADCChannels[7] = {128+9,129+9,130+9,131+9,132+9,133+9,134+9};
int HagarTDCChannels[7] = {0,0,0,0,0,0,0};

double HagarGain[7] = {1,1,1,1,1,1,1};
double HagarOffset[7] = {0,0,0,0,0,0,0};

int *PulseLimits;//[2] = {-1e6, 1e6};

double *ADCOffsets;
double *ADCGains;

// double SiliconOffset[128] = {-300.72,-446.965,-346.853,-355.579,-351.453,-365.891,-400.173,-365.167,-436.372,-425.072,-452.645,-453.476,-433.184,-310.608,-486.139,-395.248,186.871,130.997,73.8983,87.3985,81.2338,128.684,-29.6756,7.91461,-73.8193,-110.215,-80.1489,-172.145,-165.062,-172.743,-188.541,-130.623,-346.816,-328.048,-377.237,-301.929,-496.067,-395.531,-342.978,-327.192,-459.013,-328.803,-372.98,-398.922,-388.273,-1040.56,-371.758,-242.604,-214.844,-287.677,-193.148,-163.904,-89.6954,-104.69,-174.692,-99.0236,-76.9985,-20.8799,-25.906,29.4278,83.2436,73.206,113.178,189.161,158.926,52.9711,102.468,90.5307,145.594,138.956,46.8795,100.193,6.60394,157.506,111.16,149.302,-37.4439,124.469,27.2474,0,404.227,379.601,475.501,470.218,362.4,535.544,524.189,-7066.12,315.6,305.749,256.936,347.918,148.517,337.561,459.243,372.531,-64.6477,-41.3661,-124.066,-37.1786,-32.2445,60.1877,55.3287,3516.13,0,28.7981,-11.6035,-13.9398,-52.9702,-78.5182,-38.9458,-76.0316,234.922,263.639,92.3667,272.932,324.598,160.102,183.106,
// 367.979,72.8815,99.0438,-64.4422,39.9611,83.1194,107.974,113.089,3.72021};

// double SiliconGain[128] = {3.93867,3.98941,3.95779,3.9394,3.98771,3.89926,3.96829,4.02891,4.06826,3.97881,4.06826,3.84492,3.97619,4.22847,4.02622,3.93867,3.67011,3.73121,3.66265,3.68822,3.7461,3.79924,3.90334,3.84492,3.76257,3.7127,3.62693,3.7704,3.88067,3.88151,3.95779,3.72896,4.34216,4.36538,4.28399,4.36432,4.25247,4.25962,4.25247,4.23034,4.12917,4.20663,4.33898,4.34216,4.38787,4.64604,4.36432,4.34216,4.15962,4.31595,4.23539,4.1064,4.10925,4.19484,4.09702,4.17039,4.15874,4.26855,4.22812,4.04595,4.22534,4.16055,4.20154,4.15874,3.82827,3.79203,3.90087,3.78959,3.87308,3.8365,3.8365,3.84492,3.87465,3.86221,3.86221,3.73964,3.89323,3.80888,3.91969,0,2.96973,3.08213,2.96973,2.93806,3.27526,3.00357,2.97614,6.98694,3.09424,3.21183,3.17831,2.5789,3.19805,3.10669,3.15533,3.00107,3.44188,3.44982,3.38714,3.27461,3.33679,3.37315,3.58761,1.28868,0,3.37958,3.42153,3.40721,3.46451,3.30418,3.5099,3.41948,3.09254,3.22807,3.2362,3.11896,3.21535,3.11802,3.2625,3.0879,3.17261,3.24321,3.1861,3.05426,3.10122,3.24914,
// 3.12505,3.25441};

void ParameterInit()
{
  printf("\n ParameterInit\n");
  
  if(NumberOfMMM>0)MMMParameterInit();
  if(NumberofW1>0)W1ParameterInit();
  
  PulseLimitsInit();
  
  ADCInit();
}

void MMMParameterInit()
{
    printf("\n MMMParameterInit\n");
  
    MMMADCChannelLimits = new int*[NumberOfMMM];
    
    for(int i=0;i<NumberOfMMM;i++)
    {
      MMMADCChannelLimits[i] = new int[4];
    }
    
    for(int i=0;i<NumberOfMMM;i++)
    {
      for(int j=0;j<4;j++)
      {
	MMMADCChannelLimits[i][j] = 0;
      }
    }
    
    MMMADCChannelLimits[0][0] = 0;
    MMMADCChannelLimits[0][1] = 15;
    MMMADCChannelLimits[0][2] = 80;
    MMMADCChannelLimits[0][3] = 87;
    
    MMMADCChannelLimits[1][0] = 0+16;
    MMMADCChannelLimits[1][1] = 15+16;
    MMMADCChannelLimits[1][2] = 80+8;
    MMMADCChannelLimits[1][3] = 87+8;
  
    MMMADCChannelLimits[2][0] = 0+32;
    MMMADCChannelLimits[2][1] = 15+32;
    MMMADCChannelLimits[2][2] = 80+16;
    MMMADCChannelLimits[2][3] = 87+16;
    
    MMMADCChannelLimits[3][0] = 0+48;
    MMMADCChannelLimits[3][1] = 15+48;
    MMMADCChannelLimits[3][2] = 80+24;
    MMMADCChannelLimits[3][3] = 87+24;

    MMMTDCChannelLimits = new int*[NumberOfMMM];
    for(int i=0;i<NumberOfMMM;i++)
    {
      MMMTDCChannelLimits[i] = new int[4];
    }
    
    for(int i=0;i<NumberOfMMM;i++)
    {
      for(int j=0;j<4;j++)
      {
	MMMTDCChannelLimits[i][j] = 0;
      }
    }
    
    MMMTDCChannelLimits[0][0] = 6*128+48;
    MMMTDCChannelLimits[0][1] = 128*6+48+15;
    MMMTDCChannelLimits[0][2] = 0;
    MMMTDCChannelLimits[0][3] = 0;
    
    MMMTDCChannelLimits[1][0] = 6*128+48+16;
    MMMTDCChannelLimits[1][1] = 128*6+48+15+16;
    MMMTDCChannelLimits[1][2] = 0;
    MMMTDCChannelLimits[1][3] = 0;
    
    MMMTDCChannelLimits[2][0] = 6*128+48+32;
    MMMTDCChannelLimits[2][1] = 128*6+48+15+32;
    MMMTDCChannelLimits[2][2] = 0;
    MMMTDCChannelLimits[2][3] = 0;
    
    MMMTDCChannelLimits[3][0] = 6*128+48+48;
    MMMTDCChannelLimits[3][1] = 128*6+48+15+48;
    MMMTDCChannelLimits[3][2] = 0;
    MMMTDCChannelLimits[3][3] = 0;
//     {{6*128+48,128*6+48+15},{128*6+48+16,128*6+48+31},{128*6+48+32,128*6+48+47},{128*6+48+48,128*6+48+63}};
}

void W1ParameterInit()
{
  printf("\nW1ParameterInit\n");
}

void PulseLimitsInit()
{
  printf("\nPulseLimitsInit\n");
  
  PulseLimits = new int[2];
  PulseLimits[0] = -100000;
  PulseLimits[1] = 100000;
}

void CalibrationParametersInit()
{
 printf("\n CalibrationParametersInit\n"); 
  
  ADCOffsets = new double[32*ADCModules];
  ADCGains = new double[32*ADCModules];
  
  for(int i=0;i<32*ADCModules;i++)
  {
    ADCOffsets[i] = 0;
    ADCGains[i] = 1;
  }
}

void ADCInit()
{
  ADC = new float[32*ADCModules];
  
  if(32*ADCModules != ADCsize)printf("Parameters.c: 154 - bollocks");
  
  ADCClear();
}

void ADCClear()
{
  for(int i=0;i<ADCsize;i++)
  {
    ADC[i] = 0;
  }
}