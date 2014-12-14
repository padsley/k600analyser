#include "Parameters.h"


int ADCModules;// = 5;

int ADCsize;// = 32*ADCModules;

float *ADC;

int TDCModules;// = 7;//This was set to 6 which, I think, is the number of FP TDCs - PA

int TDCsize;// = TDCModules*128;

int NumberOfMMM;

// int MMMADCChannelLimits[4][4] = {{0, 15, 80, 87},{0+16, 15+16, 80+8, 87+8},{0+32, 15+32, 80+16, 87+16},{0+48, 15+48, 80+24, 87+24}};
//  int MMMTDCChannelLimits[4][2] = {{6*128+48,128*6+48+15},{128*6+48+16,128*6+48+31},{128*6+48+32,128*6+48+47},{128*6+48+48,128*6+48+63}};

int **MMMADCChannelLimits;
int **MMMTDCChannelLimits;

int NumberOfW1;// = 0;

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

void ParameterInit()
{
  printf("\n ParameterInit\n");
  CalibrationParametersInit();
  ReadConfiguration();
  PulseLimitsInit();
  ADCInit();
  printf("Finished initialising parameters - to the sorting!\n");
}

void MMMNumberInit()//This is called after the number of MMM detectors is found from the config file
{
  printf("\n MMMNumberInit\n");
  
  MMMADCChannelLimits = new int*[NumberOfMMM];
  
  for(int i=0;i<NumberOfMMM;i++)
  {
    MMMADCChannelLimits[i] = new int[4];
  }
  
  for(int i=0;i<NumberOfMMM;i++)
  {
    for(int j=0;j<4;j++)
    {
      MMMADCChannelLimits[i][j] = -1;
    }
  }
  
  MMMTDCChannelLimits = new int*[NumberOfMMM];
  for(int i=0;i<NumberOfMMM;i++)
  {
    MMMTDCChannelLimits[i] = new int[4];
  }
  
  for(int i=0;i<NumberOfMMM;i++)
  {
    for(int j=0;j<4;j++)
    {
      MMMTDCChannelLimits[i][j] = -1;
    }
  }
}

void MMMADCChannelsInit(int det, std::string side, int start, int stop)//If there are segfaults in this section, it might be because the number of MMM detectors isn't correctly set
{
  if(side.compare(0,5,"pside")==0)
  {
    MMMADCChannelLimits[det-1][0] = start;
    MMMADCChannelLimits[det-1][1] = stop;
  }
  else if(side.compare(0,5,"nside")==0)
  {
    MMMADCChannelLimits[det-1][2] = start;
    MMMADCChannelLimits[det-1][3] = stop;
  }
}

void MMMTDCChannelsInit(int det, std::string side,int start, int stop)//If there are segfaults in this section, it might be because the number of MMM detectors isn't correctly set
{
  if(side.compare(0,5,"pside")==0)
  {
    MMMTDCChannelLimits[det-1][0] = start;
    MMMTDCChannelLimits[det-1][1] = stop;
  }
  else if(side.compare(0,5,"nside")==0)
  {
    MMMTDCChannelLimits[det-1][2] = start;
    MMMTDCChannelLimits[det-1][3] = stop;
  }
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

void ReadCalibrationParameters(std::string CalibFile)
{
  printf("ReadCalibrationParameters using file %s\n",CalibFile.c_str());
  
  bool CalibRead = true;
  
  std::ifstream CalibInput;
  if(CalibFile.compare(0,6,"ignore") == 0)
  {
    printf("\n ********** Ignoring calibrations: offsets are 0, gains are 1 **********\n");
  }
  else
  {
    CalibInput.open(CalibFile.c_str());
    
    if(CalibInput.is_open())
    {
      while(CalibRead)
      {
	std::string LineBuffer;
	int channel = -1;
	double offset = 0, gain = 1;
	CalibInput >> LineBuffer;
	if(LineBuffer.compare(0,3,"eof") == 0)
	{
	  CalibRead = false;
	}
	else
	{
	  channel = atoi(LineBuffer.c_str());
	  CalibInput >> LineBuffer;
	  offset = atoi(LineBuffer.c_str());
	  CalibInput >> LineBuffer;
	  gain = atoi(LineBuffer.c_str());
	  
	  if(channel!=-1)SetADCChannelCalibration(channel, offset, gain);
	}
      }
    }
  }
  CalibInput.close();
  printf("Finished Calibration Parameters\n");
}

void SetADCChannelCalibration(int channel, double offset, double gain)
{
  if(channel<ADCsize)
  {
    ADCOffsets[channel] = offset;
    ADCGains[channel]   = gain;
  }
}

void ADCInit()
{
  printf("ADCInit\n");
  ADC = new float[32*ADCModules];	     
  ADCClear();
}

void ADCClear()
{
  for(int i=0;i<ADCsize;i++)
  {
    ADC[i] = 0;
  }
}

void ReadConfiguration()
{
  bool ConfigRead = true;
  bool MMMADCChannelRead = false;
  bool MMMTDCChannelRead = false;
  
  std::ifstream input;
  input.open("config.cfg");
  
  if(input.is_open())
  {
    while(ConfigRead)
    {
      std::string LineBuffer;
      if(!MMMADCChannelRead && !MMMTDCChannelRead)
      {
	input >> LineBuffer;
	
	if(LineBuffer.compare(0,1,"%") == 0){input.ignore(std::numeric_limits<std::streamsize>::max(), '\n' );}
	else if(LineBuffer.compare(0,11,"NumberOfMMM") == 0)
	{
	  input >> LineBuffer;
	  NumberOfMMM = atoi(LineBuffer.c_str());
	  MMMNumberInit();
	}
	else if(LineBuffer.compare(0,10,"NumberOfW1") == 0)
	{
	  input >> LineBuffer;
	  NumberOfW1 = atoi(LineBuffer.c_str());
	  // 	W1NumberInit();
	}
	else if(LineBuffer.compare(0,10,"ADCModules") == 0)
	{
	  input >> LineBuffer;
	  ADCModules = atoi(LineBuffer.c_str());
	  ADCsize = 32*ADCModules;
	}
	else if(LineBuffer.compare(0,10,"TDCModules") == 0)
	{
	  input >> LineBuffer;
	  TDCModules = atoi(LineBuffer.c_str());
	  TDCsize = 128*TDCModules;
	}
	else if(LineBuffer.compare(0,14,"MMMADCChannels") == 0)
	{
	  if(MMMADCChannelRead==false)MMMADCChannelRead = true;
	  else if(MMMADCChannelRead==true)MMMADCChannelRead = false;
	}
	else if(LineBuffer.compare(0,14,"MMMTDCChannels") == 0)
	{
	  if(MMMTDCChannelRead==false)MMMTDCChannelRead = true;
	  else if(MMMTDCChannelRead==true)MMMTDCChannelRead = false;
	}
	else if(LineBuffer.compare(0,15,"CalibrationFile") == 0)
	{
	  input >> LineBuffer;
	  printf("Using calibration file: %s\n",LineBuffer.c_str());
	  ReadCalibrationParameters(LineBuffer);
	}
	else if(LineBuffer.compare(0,9,"ConfigEnd") == 0)
	{
	  printf("ConfigEnd\n");
	  ConfigRead = false;
	}
      }
      
      if(MMMADCChannelRead)
      {
	int num = 0, start = -1, stop = -1;
	std::string side = "";
	input >> LineBuffer;
	if(LineBuffer.compare(0,14,"MMMADCChannels") == 0)
	{
	  if(MMMADCChannelRead==true)MMMADCChannelRead = false;
	}
	else
	{
	  printf("\n Detector number %d\t",atoi(LineBuffer.c_str()));
	  num = atoi(LineBuffer.c_str());
	  input>> LineBuffer;
	  printf("Side: %s\t",LineBuffer.c_str());
	  side = LineBuffer;
	  input >> LineBuffer;
	  printf("Start: %d\t",atoi(LineBuffer.c_str()));
	  start = atoi(LineBuffer.c_str());
	  input >> LineBuffer;
	  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
	  stop = atoi(LineBuffer.c_str());
	  
	  MMMADCChannelsInit(num, side, start, stop);
	}
      }
      
      if(MMMTDCChannelRead)
      {
	int num = 0, start = -1, stop = -1;
	std::string side = "";
	input >> LineBuffer;
	if(LineBuffer.compare(0,14,"MMMTDCChannels") == 0)
	{
	  if(MMMTDCChannelRead==true)MMMTDCChannelRead = false;
	}
	else
	{
	  printf("\n Detector number %d\t",atoi(LineBuffer.c_str()));
	  num = atoi(LineBuffer.c_str());
	  input>> LineBuffer;
	  printf("Side: %s\t",LineBuffer.c_str());
	  side = LineBuffer;
	  input >> LineBuffer;
	  printf("Start: %d\t",atoi(LineBuffer.c_str()));
	  start = atoi(LineBuffer.c_str());
	  input >> LineBuffer;
	  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
	  stop = atoi(LineBuffer.c_str());
	  
	  MMMTDCChannelsInit(num, side, start, stop);
	}
      }
    }
  }
  printf("Finished ReadConfiguration\n");
}
