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

int NumberOfClover;

int **CloverADCChannelLimits;
int **CloverTDCChannelLimits;

double HagarGain[7] = {1,1,1,1,1,1,1};
double HagarOffset[7] = {0,0,0,0,0,0,0};

double CloverGain[8] = {1,1,1,1,1,1,1,1};
double CloverOffset[8] = {0,0,0,0,0,0,0,0};


int *PulseLimits;//[2] = {-1e6, 1e6};

double *ADCOffsets;
double *ADCGains;

int *ChannelCounter;
int *GoodChannelCounter;

bool VDC1_new, VDC2_new;

int NXThetaCorr;//Number of terms to correct the X1 position with ThSCAT
double *XThetaCorr;//pointer array to store the terms from above
int NXY1Corr;
double *XY1Corr;

int NXRigidityPars;
double *XRigidityPars;

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
//printf("\nHagarParameterInit - end\n");
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



void CloverNumberInit()
{
   printf("\nCloverParameterInit\n");
  
  CloverADCChannelLimits = new int*[NumberOfClover];
  
  for(int i=0;i<NumberOfClover;i++)
  {
    CloverADCChannelLimits[i] = new int[2];
  }
  
  for(int i=0;i<NumberOfClover;i++)
  {
    for(int j=0;j<2;j++)
    {
      CloverADCChannelLimits[i][j] = -1;
    }
  }
  
  CloverTDCChannelLimits = new int*[NumberOfClover];
  for(int i=0;i<NumberOfClover;i++)
  {
    CloverTDCChannelLimits[i] = new int[2];
  }
  
  for(int i=0;i<NumberOfClover;i++)
  {
    for(int j=0;j<2;j++)
    {
      CloverTDCChannelLimits[i][j] = -1;
    }
  }
  printf("\nCloverParameterInit - end\n");
}

void CloverADCChannelsInit(int det, int start, int stop)
{
  if(det<=NumberOfClover)
  {
   
      CloverADCChannelLimits[det-1][0] = start;
      CloverADCChannelLimits[det-1][1] = stop;
  
  }
  else
  {
    printf("ADC: Detector number is higher than the number of Clover detectors - skipped enabling this detector\n");
  }
}

void CloverTDCChannelsInit(int det, int start, int stop)
{
  if(det<=NumberOfClover)
    {
    
	  CloverTDCChannelLimits[det-1][0] = start;
	  CloverTDCChannelLimits[det-1][1] = stop;

    }
  else
    {
      printf("ADC: Detector number is higher than the number of W1 detectors - skipped enabling this detector\n");
    }
}





void PulseLimitsInit()
{
  printf("\nPulseLimitsInit\n");
  
  PulseLimits = new int[2];
  PulseLimits[0] = 0;
  PulseLimits[1] = 0;
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
  bool HagarUsed = false;
  bool HagarADCChannelRead = false;
  bool HagarTDCChannelRead = false;
  bool CloverUsed = false;
  bool CloverADCChannelRead = false;
  bool CloverTDCChannelRead = false;
  bool ThSCATCorrectionParametersRead = false;
  bool XRigidityParametersRead = false;
  bool Y1CorrectionParametersRead = false;

  std::ifstream input;

  input.open("config.cfg");//This is the line to change in order to change the configuration file
  //input.open("config.cfg");//This is the line to change in order to change the configuration file
  //input.open("/afs/tlabs.ac.za/user/p/padsley/data/PR236/Si28/configSi28PR236WE3.cfg");
  //input.open("/afs/tlabs.ac.za/user/p/padsley/data/PR236/Mg26/configMg26PR236WE2.cfg");
  //input.open("/afs/tlabs.ac.za/user/p/padsley/data/PR226/configPR226.cfg");
  //input.open("/afs/tlabs.ac.za/user/p/padsley/data/PR244/Si28/configSi28PR244WE1.cfg");
  //input.open("/afs/tlabs.ac.za/user/p/padsley/data/PR244/Mg24/configMg24PR244WE1.cfg");

  if(input.is_open())
    {
      while(ConfigRead)
	{
	  std::string LineBuffer;
	  if(!MMMADCChannelRead && !MMMTDCChannelRead && !W1ADCChannelRead && !W1TDCChannelRead && !HagarADCChannelRead && !HagarTDCChannelRead && !CloverADCChannelRead && !CloverTDCChannelRead && !ThSCATCorrectionParametersRead && !XRigidityParametersRead && !Y1CorrectionParametersRead)
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
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,3,"yes") == 0)
		    {
		      HagarInit();
		      HagarUsed = true;
		    }
		  else if(LineBuffer.compare(0,3,"no") == 0)
		    {
		      HagarUsed = false;
		    }
		  else
		    {
		      printf("Hagar usage option not recognised\n");
		    }
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
              else if(LineBuffer.compare(0,10,"CloverUsed") == 0)
		{
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,3,"yes") == 0)
		    {
		   //   CloverNumberInit();
		      CloverUsed = true;
		    }
		  else if(LineBuffer.compare(0,3,"no") == 0)
		    {
		      CloverUsed = false;
		    }
		  else
		    {
		      printf("Clover usage option not recognised\n");
		    }
		}
 	      else if(LineBuffer.compare(0,14,"NumberOfClover") == 0)
		{
		  input >> LineBuffer;
		  NumberOfClover = atoi(LineBuffer.c_str());
		  CloverNumberInit();
		}
	      else if(LineBuffer.compare(0,17,"CloverADCChannels") == 0)
		{
		  if(CloverADCChannelRead==false)CloverADCChannelRead = true;
		  else if(CloverADCChannelRead==true)CloverADCChannelRead = false;
		}
	      else if(LineBuffer.compare(0,17,"CloverTDCChannels") == 0)
		{
		  if(CloverTDCChannelRead==false)CloverTDCChannelRead = true;
		  else if(CloverTDCChannelRead==true)CloverTDCChannelRead = false;
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
		  NXThetaCorr = atoi(LineBuffer.c_str());
		  XThetaCorr = new double[NXThetaCorr];
		  for(int c=0;c<NXThetaCorr;c++)XThetaCorr[c] = 0;
		  ThSCATCorrectionParametersRead = true;
		}
	      else if(LineBuffer.compare(0,19,"InelasticScattering") ==0)
		{
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,4,"true") == 0)TestInelastic = true;
		  else if(LineBuffer.compare(0,5,"false") == 0)TestInelastic = false;
		  else TestInelastic = true;
		  if(TestInelastic)printf("Going to do excitation energy calculation assuming inelastic scattering\n");
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
	      else if(LineBuffer.compare(0,10,"BeamEnergy") == 0)
		{
		  input >> LineBuffer;
		  printf("Beam Energy: %f MeV\n",atof(LineBuffer.c_str()));
		  T1 = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,15,"ScatteringAngle") == 0)
		{
		  input >> LineBuffer;
		  printf("Scattering Angle: %f degrees\n",atof(LineBuffer.c_str()));
		  theta3 = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,19,"RigidityCalibration") == 0)
		{
		  input >> LineBuffer;
		  printf("Using %d parameters for the correction X position -> Rigidity calibration\n",atoi(LineBuffer.c_str()));
		  NXRigidityPars = atoi(LineBuffer.c_str());
		  XRigidityPars = new double[NXRigidityPars];
		  for(int c=0;c<NXRigidityPars;c++)XRigidityPars[c] = 0;
		  XRigidityParametersRead = true;
		}
	      else if(LineBuffer.compare(0,17,"Y1CorrectionTerms") == 0)
		{
		  input >> LineBuffer;
		  printf("Using %d terms for the Y1 position correction\n",atoi(LineBuffer.c_str()));
		  NXY1Corr = atoi(LineBuffer.c_str());
		  XY1Corr = new double[NXY1Corr];
		  for(int c=0;c<NXY1Corr;c++)XY1Corr[c] = 0;
		  Y1CorrectionParametersRead = true;
		}
	      else if(LineBuffer.compare(0,12,"PulseLimits") == 0)
		{
		  input >> LineBuffer;
		  PulseLimits[0] = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  PulseLimits[1] = atoi(LineBuffer.c_str());
		  printf("Good pulse limits: %d - %d\n", PulseLimits[0], PulseLimits[1]);
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
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,24,"EndThSCATCorrectionTerms") == 0 && ThSCATCorrectionParametersRead)ThSCATCorrectionParametersRead = false;
	      else
		{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  XThetaCorr[npar] = valpar;
		}
	    }
		
	  if(Y1CorrectionParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,20,"EndY1CorrectionTerms") == 0 && Y1CorrectionParametersRead)Y1CorrectionParametersRead = false;
	      else
		{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  XY1Corr[npar] = valpar;
		}
	    }
	
	  if(XRigidityParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,22,"EndRigidityCalibration") == 0 && XRigidityParametersRead)XRigidityParametersRead = false;
	      else
		{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());//Line is actually pointless... I should tidy this up
		  XRigidityPars[npar] = atof(LineBuffer.c_str());
		  //printf("Check XRigidityPars[%d]: %e\n",npar,XRigidityPars[npar]);
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
	      if(LineBuffer.compare(0,16,"HagarADCChannels") == 0)
		{
		  if(HagarADCChannelRead==true)HagarADCChannelRead = false;
		}
	      else
		{
		  printf("HagarADCChannelRead: \t");
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
	      if(LineBuffer.compare(0,16,"HagarTDCChannels") == 0)
		{
		  if(HagarTDCChannelRead==true)HagarTDCChannelRead = false;
		}
	      else
		{
		  printf("HagarTDCChannelRead: \t");
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
		  HagarTDCChannelsInit(start, stop);
		}
	    }

      /*    if(CloverADCChannelRead)
	    {
	      int start = -1, stop = -1;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,16,"CloverADCChannels") == 0)
		{
		  if(CloverADCChannelRead==true)CloverADCChannelRead = false;
		}
	      else
		{
		  printf("CloverADCChannelRead: \t");
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
		  CloverADCChannelsInit(start, stop);
		}
	    }

	  if(CloverTDCChannelRead)
	    {
	      int start = -1, stop = -1;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,16,"CloverTDCChannels") == 0)
		{
		  if(CloverTDCChannelRead==true)CloverTDCChannelRead = false;
		}
	      else
		{
		  printf("CloverTDCChannelRead: \t");
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
		  CloverTDCChannelsInit(start, stop);
		}
	    }*/

	  if(CloverADCChannelRead)
	    {
	      int num = 0, start = -1, stop = -1;
	      std::string side = "";
	      input >> LineBuffer;
	      printf("%s\n",LineBuffer.c_str());
	      if(LineBuffer.compare(0,17,"CloverADCChannels") == 0)
		{
		  if(CloverADCChannelRead==true)CloverADCChannelRead = false;
		}
	      else
		{
		  printf("\n Detector number %d\t",atoi(LineBuffer.c_str()));
		  num = atoi(LineBuffer.c_str());
		  input>> LineBuffer;
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
	  
		  CloverADCChannelsInit(num, start, stop);
		}
	    }
	
	  if(CloverTDCChannelRead)
	    {
	      int num = 0, start = -1, stop = -1;
	      std::string side = "";
	      input >> LineBuffer;
	      printf("%s\n",LineBuffer.c_str());
	      if(LineBuffer.compare(0,17,"CloverTDCChannels") == 0)
		{
		  if(CloverTDCChannelRead==true)CloverTDCChannelRead = false;
		}
	      else
		{
		  printf("\n Detector number %d\t",atoi(LineBuffer.c_str()));
		  num = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
	  
		  CloverTDCChannelsInit(num, start, stop);
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

