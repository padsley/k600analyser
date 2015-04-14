#include "Parameters.h"


int ADCModules;

int ADCsize;

float *ADC;

int TDCModules;

int TDCsize;

int QDCsize = 32;
float *QDC;

int NumberOfMMM;

int **MMMADCChannelLimits;
int **MMMTDCChannelLimits;

int NumberOfW1;

int **W1ADCChannelLimits;
int **W1TDCChannelLimits;

int *HagarADCChannelLimits;
int *HagarTDCChannelLimits;

double HagarGain[7] = {1,1,1,1,1,1,1};
double HagarOffset[7] = {0,0,0,0,0,0,0};

int *PulseLimits;//[2] = {-1e6, 1e6};

double *ADCOffsets;
double *ADCGains;

int *ChannelCounter;
int *GoodChannelCounter;

bool VDC1_new, VDC2_new;

int rigThCTs;//Number of terms to correct the X1 position with ThSCAT
double *rigThetaCorr;//pointer array to store the terms from above

bool TestInelastic = true; //Test to see if this is an elastic reaction... default is true as they're the ones that we run the most
double *masses;
double T1;
double theta3 = 0;//Scattering angle for the light ion in the spectrometer - default to scattering angle of 0

void ParameterInit()
{
  printf("\n ParameterInit\n");
  masses = new double[4];
  ReadConfiguration();
  PulseLimitsInit();
  ADCInit();
  QDCInit();
  PrintParameters();
  printf("Finished initialising parameters - to the sorting!\n");
}

void CorrectionInit()
{
  rigThetaCorr = new double[rigThCTs];
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
  if(det<=NumberOfMMM)
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
  else
  {
    printf("ADC: Detector number is higher than the number of MMM detectors - skipped enabling this detector\n");
  }
}

void MMMTDCChannelsInit(int det, std::string side,int start, int stop)//If there are segfaults in this section, it might be because the number of MMM detectors isn't correctly set
{
  if(det<=NumberOfMMM)
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
  else
    {
      printf("ADC: Detector number is higher than the number of MMM detectors - skipped enabling this detector\n");
    }
}

void W1NumberInit()
{
  printf("\nW1ParameterInit\n");
  
  W1ADCChannelLimits = new int*[NumberOfW1];
  
  for(int i=0;i<NumberOfW1;i++)
  {
    W1ADCChannelLimits[i] = new int[4];
  }
  
  for(int i=0;i<NumberOfW1;i++)
  {
    for(int j=0;j<4;j++)
    {
      W1ADCChannelLimits[i][j] = -1;
    }
  }
  
  W1TDCChannelLimits = new int*[NumberOfW1];
  for(int i=0;i<NumberOfW1;i++)
  {
    W1TDCChannelLimits[i] = new int[4];
  }
  
  for(int i=0;i<NumberOfW1;i++)
  {
    for(int j=0;j<4;j++)
    {
      W1TDCChannelLimits[i][j] = -1;
    }
  }
  printf("\nW1ParameterInit - end\n");
}

void W1ADCChannelsInit(int det, std::string side, int start, int stop)//If there are segfaults in this section, it might be because the number of MMM detectors isn't correctly set
{
  if(det<=NumberOfW1)
  {
    if(side.compare(0,5,"pside")==0)
    {
      W1ADCChannelLimits[det-1][0] = start;
      W1ADCChannelLimits[det-1][1] = stop;
    }
    else if(side.compare(0,5,"nside")==0)
    {
      W1ADCChannelLimits[det-1][2] = start;
      W1ADCChannelLimits[det-1][3] = stop;
    }
  }
  else
  {
    printf("ADC: Detector number is higher than the number of W1 detectors - skipped enabling this detector\n");
  }
}

void W1TDCChannelsInit(int det, std::string side,int start, int stop)//If there are segfaults in this section, it might be because the number of MMM detectors isn't correctly set
{
  if(det<=NumberOfW1)
    {
      if(side.compare(0,5,"pside")==0)
	{
	  W1TDCChannelLimits[det-1][0] = start;
	  W1TDCChannelLimits[det-1][1] = stop;
	}
      else if(side.compare(0,5,"nside")==0)
	{
	  W1TDCChannelLimits[det-1][2] = start;
	  W1TDCChannelLimits[det-1][3] = stop;
	}
    }
  else
    {
      printf("ADC: Detector number is higher than the number of W1 detectors - skipped enabling this detector\n");
    }
}

void HagarInit()
{
  HagarADCChannelLimits = new int[2];
  HagarTDCChannelLimits = new int[2];
}

void HagarADCChannelsInit(int start, int stop)
{
  HagarADCChannelLimits[0] = start;
  HagarADCChannelLimits[1] = stop;
}

void HagarTDCChannelsInit(int start, int stop)
{
  HagarTDCChannelLimits[0] = start;
  HagarTDCChannelLimits[1] = stop;
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
    ADCOffsets[i] = 0;//printf("ADCOffsets[%d]: %f\n",i,ADCOffsets[i]);
    ADCGains[i] = 1;//printf("ADCGains[%d]: %f\n",i,ADCGains[i]);
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
    for(int i=0;i<32*ADCModules;i++)printf("ADCOffsets[%d]: %f\tADCGains[%d]: %f\n",i,ADCOffsets[i],i,ADCGains[i]);
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
	  offset = atof(LineBuffer.c_str());
	  CalibInput >> LineBuffer;
	  gain = atof(LineBuffer.c_str());
	  printf("Channel: %d\tOffset: %f\tGain: %f\n",channel,offset,gain);
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

void QDCInit()
{
  printf("QDCInit\n");
  QDC = new float[QDCsize];
  QDCClear();
}

void QDCClear()
{
  for(int i=0;i<QDCsize;i++)
  {
    QDC[i] = 0;
  }
}

void ReadConfiguration()
{
  bool ConfigRead = true;
  bool MMMADCChannelRead = false;
  bool MMMTDCChannelRead = false;
  bool W1ADCChannelRead = false;
  bool W1TDCChannelRead = false;
  bool HagarADCChannelRead = false;
  bool HagarTDCChannelRead = false;
  bool ThSCATCorrectionParametersRead = false;

  std::ifstream input;
  input.open("config.cfg");
  
  if(input.is_open())
    {
      while(ConfigRead)
	{
	  std::string LineBuffer;
	  if(!MMMADCChannelRead && !MMMTDCChannelRead && !W1ADCChannelRead && !W1TDCChannelRead && !HagarADCChannelRead && !HagarTDCChannelRead && !ThSCATCorrectionParametersRead)
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
		  W1NumberInit();
		}
	      else if(LineBuffer.compare(0,10,"ADCModules") == 0)
		{
		  input >> LineBuffer;
		  ADCModules = atoi(LineBuffer.c_str());
		  ADCsize = 32*ADCModules;
		  CalibrationParametersInit();
		}
	      else if(LineBuffer.compare(0,10,"TDCModules") == 0)
		{
		  input >> LineBuffer;
		  TDCModules = atoi(LineBuffer.c_str());
		  TDCsize = 128*TDCModules;
		  ChannelCounter = new int[128*TDCModules];
		  GoodChannelCounter = new int[128*TDCModules];
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
	      else if(LineBuffer.compare(0,13,"W1ADCChannels") == 0)
		{
		  if(W1ADCChannelRead==false)W1ADCChannelRead = true;
		  else if(W1ADCChannelRead==true)W1ADCChannelRead = false;
		}
	      else if(LineBuffer.compare(0,13,"W1TDCChannels") == 0)
		{
		  if(W1TDCChannelRead==false)W1TDCChannelRead = true;
		  else if(W1TDCChannelRead==true)W1TDCChannelRead = false;
		}
	      else if(LineBuffer.compare(0,9,"HagarUsed") == 0)
		{
		  HagarInit();
		}
	      else if(LineBuffer.compare(0,16,"HagarADCChannels") == 0)
		{
		  if(HagarADCChannelRead==false)HagarADCChannelRead = true;
		  else if(HagarADCChannelRead==true)HagarADCChannelRead = false;
		}
	      else if(LineBuffer.compare(0,16,"HagarTDCChannels") == 0)
		{
		  if(HagarTDCChannelRead==false)HagarTDCChannelRead = true;
		  else if(HagarTDCChannelRead==true)HagarTDCChannelRead = false;
		}
	      else if(LineBuffer.compare(0,4,"VDC1") == 0)
		{
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,3,"new") == 0)
		    {
		      printf("VDC1 is a new-type wire chamber\n");
		      VDC1_new = true;
		    }
		  else if(LineBuffer.compare(0,3,"old") == 0)
		    {
		      printf("VDC1 is an old-type wire chamber\n");
		      VDC1_new = false;
		    }
		}
	      else if(LineBuffer.compare(0,4,"VDC2") == 0)
		{
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,3,"new") == 0)
		    {
		      printf("VDC2 is a new-type wire chamber\n");
		      VDC2_new = true;
		    }
		  else if(LineBuffer.compare(0,3,"old") == 0)
		    {
		      printf("VDC2 is an old-type wire chamber\n");
		      VDC2_new = false;
		    }
		}
	      else if(LineBuffer.compare(0,15,"CalibrationFile") == 0)
		{
		  input >> LineBuffer;
		  printf("Using calibration file: %s\n",LineBuffer.c_str());
		  ReadCalibrationParameters(LineBuffer);
		}
	      else if(LineBuffer.compare(0,21,"ThSCATCorrectionTerms") == 0)
		{
		  input >> LineBuffer;
		  printf("Using %d terms for the ThSCAT position correction\n",atoi(LineBuffer.c_str()));
		  rigThCTs = atoi(LineBuffer.c_str());
		  rigThetaCorr = new double[rigThCTs];
		  ThSCATCorrectionParametersRead = true;
		}
	      else if(LineBuffer.compare(0,19,"InelasticScattering") ==0)
		{
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,4,"true") == 0)TestInelastic = true;
		  else if(LineBuffer.compare(0,5,"false") == 0)TestInelastic = false;
		  else TestInelastic = true;
		}
	      else if(LineBuffer.compare(0,5,"mass1") == 0)
		{
		  input >> LineBuffer;
		  printf("mass1: %f MeV/c**2\n",atof(LineBuffer.c_str()));
		  masses[0] = atof(LineBuffer.c_str());//Be careful... the index number is different to the particle number...
		}
	      else if(LineBuffer.compare(0,5,"mass2") == 0)
		{
		  input >> LineBuffer;
		  printf("mass2: %f MeV/c**2\n",atof(LineBuffer.c_str()));
		  masses[1] = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,5,"mass3") == 0)
		{
		  input >> LineBuffer;
		  printf("mass3: %f MeV/c**2\n",atof(LineBuffer.c_str()));
		  masses[2] = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,5,"mass4") == 0)
		{
		  input >> LineBuffer;
		  printf("mass4: %f MeV/c**2\n",atof(LineBuffer.c_str()));
		  masses[3] = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,10,"BeamEnergy")==0)
		{
		  input >> LineBuffer;
		  printf("Beam Energy: %f MeV\n",atof(LineBuffer.c_str()));
		  T1 = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,15,"ScatteringAngle")==0)
		{
		  input >> LineBuffer;
		  printf("Scattering Angle: %f degrees\n",atof(LineBuffer.c_str()));
		  theta3 = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,9,"ConfigEnd") == 0)
		{
		  printf("ConfigEnd\n");
		  ConfigRead = false;
		}
	      else
		{
		  printf("Line not recognised by ReadConfiguration\n");
		  printf("%s\n",LineBuffer.c_str());
		}
	    }
      
	  if(ThSCATCorrectionParametersRead)
	    {
	      int npar = -1, valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,24,"EndThSCATCorrectionTerms") == 0 && ThSCATCorrectionParametersRead)ThSCATCorrectionParametersRead = false;
	      else
		{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %f\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  rigThetaCorr[npar] = valpar;
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

	  if(W1ADCChannelRead)
	    {
	      int num = 0, start = -1, stop = -1;
	      std::string side = "";
	      input >> LineBuffer;
	      printf("%s\n",LineBuffer.c_str());
	      if(LineBuffer.compare(0,13,"W1ADCChannels") == 0)
		{
		  if(W1ADCChannelRead==true)W1ADCChannelRead = false;
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
	  
		  W1ADCChannelsInit(num, side, start, stop);
		}
	    }
	
	  if(W1TDCChannelRead)
	    {
	      int num = 0, start = -1, stop = -1;
	      std::string side = "";
	      input >> LineBuffer;
	      printf("%s\n",LineBuffer.c_str());
	      if(LineBuffer.compare(0,14,"W1TDCChannels") == 0)
		{
		  if(W1TDCChannelRead==true)W1TDCChannelRead = false;
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
	  
		  W1TDCChannelsInit(num, side, start, stop);
		}
	    }

	  if(HagarADCChannelRead)
	    {
	      int start = -1, stop = -1;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,14,"HagarADCChannels") == 0)
		{
		  if(HagarADCChannelRead==true)HagarADCChannelRead = false;
		}
	      else
		{
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
		  HagarADCChannelsInit(start, stop);
		}
	    }

	  if(HagarTDCChannelRead)
	    {
	      int start = -1, stop = -1;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,14,"HagarTDCChannels") == 0)
		{
		  if(HagarTDCChannelRead==true)HagarTDCChannelRead = false;
		}
	      else
		{
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
		  HagarTDCChannelsInit(start, stop);
		}
	    }
	}
    }
  else
    {
      if(ConfigRead)printf("Configuration file not found - you're going to crash\n");
    }
  printf("Finished ReadConfiguration\n");
}

void PrintParameters()
{
  printf("ADCModules: %d\n",ADCModules);
  printf("ADCsize: %d\n",ADCsize);
  printf("TDCModules: %d\n",TDCModules);
  printf("TDCsize: %d\n",TDCsize);
}
