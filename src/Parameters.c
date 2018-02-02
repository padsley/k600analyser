#include "Parameters.h"


int ADCModules;

int ADCsize;

float *ADC;
int *ADCchannel;

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

int ***GateauTDCChannelLimits;

int NumberOfScintillator;

int *ScintillatorADCChannels;
int *ScintillatorTDCChannels;

double HagarGain[7] = {1,1,1,1,1,1,1};
double HagarOffset[7] = {0,0,0,0,0,0,0};

double CloverGain[8] = {1,1,1,1,1,1,1,1};
double CloverOffset[8] = {0,0,0,0,0,0,0,0};

double ScintillatorGain[8] = {1,1,1,1,1,1,1,1};
double ScintillatorOffset[8] = {0,0,0,0,0,0,0,0};

int *PulseLimits;//[2] = {-1e6, 1e6};

double *ADCOffsets;
double *ADCGains;

double **ADCCalibrationParameters;

double *TDCOffsets;

int *ChannelCounter;
int *GoodChannelCounter;

bool VDC1_new, VDC2_new;
bool VDC1_new_UX, VDC2_new_UX;

int NXThetaCorr;//Number of terms to correct the X1 position with ThSCAT i.e. (ThSCAT)^n
double *XThetaCorr;//pointer array to store the terms from above
int NXThetaXCorr;//Number of terms to correct the X1 position with ThSCAT i.e. (ThSCAT)^n*(X)^n

double *XThetaXCorr;//pointer array to store the terms from above
int NXThetaXLoffCorr; //Number of terms to correct the X1 position (with offset) with ThSCAT with power progression for ThSCAT only i.e. ((ThSCAT)^n)*(X-X_LSOffset)
double *XThetaXLoffCorr; //pointer array to store the terms from above
double X_LSOffset; //Offset for lineshape correction of the form (ThSCAT^n)*(X-X_loff)
int NXY1Corr;
double *XY1Corr;
int NXTOFCorr; //Number of terms to correct X1pos (with offset) with TOF-TOF_LSOffset
double *XTOFCorr; //pointer array to store the terms from above
double TOF_LSOffset; //Offset for TOF to place it around 0

int NThFPSCATOffset;     //Number of terms to convert thetaFP to thetaSCAT
double *ThFPSCATOffset;  //pointer array to store the terms from above
int NThFPSCATSlope;    //Number of terms to convert thetaFP to thetaSCAT
double *ThFPSCATSlope; //pointer array to store the terms from above



//H. Fujita style parameters added by F. Diel
int NThetaXCorr_FD;  //Number of terms
double *ThetaXCorr_i1_FD; // position dependent parameters
double *ThetaXCorr_i2_FD; // position dependent parameters
double *ThetaXCorr_par_FD; // position dependent parameters

//H. Fujita style TOF correction added by F. Diel
int NTOFCorr_FD;  //Number of terms
double *TOFCorr_i1_FD; // position and angle dependent parameters
double *TOFCorr_i2_FD; // position and angle dependent parameters
double *TOFCorr_par_FD; // position and angle dependent parameters


int NXRigidityPars;
double *XRigidityPars;

int NrOfRunsForX1Offsets;
int *RunNrForX1Offsets;
double *X1Offsets;

int NrOfRunsForTOFOffsets;
int *RunNrForTOFOffsets;
double *TOFOffsets;

int NrOfRunsForPadOffsets;
int *RunNrForPadOffsets;
double *PadOffsets;


bool TestInelastic = true; //Test to see if this is an elastic reaction... default is true as they're the ones that we run the most
double *masses;
double T1;
double theta3 = 0;//Scattering angle for the light ion in the spectrometer - default to scattering angle of 0
double z_x1x2,x_x1x2;

int RunNumber = 0;
int TotalRunsNumber = 0;
double **ExCorrTerms;
double ExCorrection = 0.;


/*-------------------------------------------------*/
void ParameterInit()
{
  printf("\n ParameterInit\n");
  masses = new double[4];
  ReadConfiguration();
  PulseLimitsInit();
  ADCInit();
  QDCInit();
  TDCInit();
  PrintParameters();
//   printf("ADCCalibrationParameters.size(): %lu\n",ADCCalibrationParameters.size());
  printf("\nFinished initialising parameters - to the sorting!\n");
}

/*-------------------------------------------------*/
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

/*-------------------------------------------------*/
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

/*-------------------------------------------------*/
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

/*-------------------------------------------------*/
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

/*-------------------------------------------------*/
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

/*-------------------------------------------------*/
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

/*-------------------------------------------------*/
void HagarInit()
{
  HagarADCChannelLimits = new int[2];
  HagarTDCChannelLimits = new int[2];
}

/*-------------------------------------------------*/
void HagarADCChannelsInit(int start, int stop)
{
  HagarADCChannelLimits[0] = start;
  HagarADCChannelLimits[1] = stop;
}

/*-------------------------------------------------*/
void HagarTDCChannelsInit(int start, int stop)
{
  HagarTDCChannelLimits[0] = start;
  HagarTDCChannelLimits[1] = stop;
}

/*-------------------------------------------------*/
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
      printf("TDC: Detector number is higher than the number of Clover detectors - skipped enabling this detector\n");
    }
}

void GateauTDCChannelLimitsInits()
{
  GateauTDCChannelLimits = new int**[2];
  for(int i=0;i<2;i++)
  {
    GateauTDCChannelLimits[i] = new int*[5];
    for(int j=0;j<5;j++)
    {
      GateauTDCChannelLimits[i][j] = new int[2];
      for(int k=0;k<2;k++)
      {
	GateauTDCChannelLimits[i][j][k] = -1;
      }
    }
  }
}

void GateauSetChannelLimits(int plane, int sector, int start, int stop)
{
  GateauTDCChannelLimits[plane][sector][0] = start;
  GateauTDCChannelLimits[plane][sector][1] = stop;
}
void PulseLimitsInit()
{
  printf("\nPulseLimitsInit\n");
  
  PulseLimits = new int[2];
  PulseLimits[0] = 4000;
  PulseLimits[1] = 100000;
}

/*-------------------------------------------------*/
//void ScintillatorInit()
void ScintillatorNumberInit()
{

  printf("\nScintillatorParameterInit\n");
  
  ScintillatorADCChannels = new int[NumberOfScintillator];
  ScintillatorTDCChannels = new int[NumberOfScintillator];

    for(int j=0;j<NumberOfScintillator;j++)
    {
      ScintillatorTDCChannels[j] = -1;
      ScintillatorADCChannels[j] = -1;
    }  
  
  printf("\nScintillatorParameterInit - end\n");
}



/*-------------------------------------------------*/
void ScintillatorADCChannelsInit(int det, int start)
{
  if(det<=NumberOfScintillator)
{
  ScintillatorADCChannels[det-1] = start;
//  ScintillatorADCChannelLimits[1] = stop;
  }
  else
  {
    printf("ADC: Detector number is higher than the number of Scintillator detectors - skipped enabling this detector\n");
  }


}

/*-------------------------------------------------*/
void ScintillatorTDCChannelsInit(int det, int start)
{
  if(det<=NumberOfScintillator)
{
  ScintillatorTDCChannels[det-1] = start;
//  ScintillatorTDCChannelLimits[1] = stop;
  }
  else
  {
    printf("TDC: Detector number is higher than the number of Scintillator detectors - skipped enabling this detector\n");
  }

}

/* ----------------------------------------------- */
/*-------------------------------------------------*/
void CalibrationParametersInit()
{
  printf("\n CalibrationParametersInit\n"); 
  
  ADCOffsets = new double[32*ADCModules];
  ADCGains = new double[32*ADCModules];
  
  ADCCalibrationParameters = new double*[32*ADCModules];
  
  for(int i=0;i<32*ADCModules;i++)
  {
    ADCOffsets[i] = 0;//printf("ADCOffsets[%d]: %f\n",i,ADCOffsets[i]);
    ADCGains[i] = 1;//printf("ADCGains[%d]: %f\n",i,ADCGains[i]);
//     std::vector<double> temp;
//     temp.push_back(2);
//     temp.push_back(0);
//     temp.push_back(1);
//     ADCCalibrationParameters.push_back(temp);
    ADCCalibrationParameters[i] = new double[3];
    ADCCalibrationParameters[i][0] = 2;
    ADCCalibrationParameters[i][1] = 0;
    ADCCalibrationParameters[i][2] = 1;
  }
}

/*-------------------------------------------------*/
void ReadCalibrationParameters(std::string CalibFile)
{
  printf("ReadCalibrationParameters using file %s\n",CalibFile.c_str());
  
  bool CalibRead = true;
  
  std::ifstream CalibInput;
  if(CalibFile.compare(0,6,"ignore") == 0)
  {
    printf("\n ********** Ignoring calibrations: offsets are 0, gains are 1 **********\n");
    for(int i=0;i<32*ADCModules;i++)
    {
      printf("ADCOffsets[%d]: %f\tADCGains[%d]: %f\n",i,ADCOffsets[i],i,ADCGains[i]);
      printf("ADCCalibrationParameters[%d][1]: %f\tADCCalibrationParameters[%d][2]: %f\n",i,ADCCalibrationParameters[i][1],i,ADCCalibrationParameters[i][2]);
//       ADCCalibrationParameters.at(i).push_back(0);
//       ADCCalibrationParameters.at(i).push_back(1);
    }

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
// 	std::cout << LineBuffer << std::endl;
	if(LineBuffer.compare(0,3,"eof") == 0)
	{
	  CalibRead = false;
	}
	else
	{
	  int npars = -1;
	  channel = atoi(LineBuffer.c_str());
// 	  std::cout << "channel: " << LineBuffer << std::endl;
	  CalibInput >> LineBuffer;
// 	  std::cout << "npars: " << LineBuffer << std::endl;
	  npars = atoi(LineBuffer.c_str());
// 	  printf("npars: %d\n",npars);
	  
	  if(npars<0)printf("NOT FOUND NUMBER OF PARAMETERS!\n");
	  
	  if(npars>0)
	  {
	    delete ADCCalibrationParameters[channel];
	    ADCCalibrationParameters[channel] = new double[npars+1];
	  }
	  
	  ADCCalibrationParameters[channel][0] = (double)npars;
// 	  printf("ADCCalibrationParameters[channel][0]: %d\n",(int)ADCCalibrationParameters[channel][0]);
	  
	  for(int np=0;np<npars;np++)
	  {
	   CalibInput >> LineBuffer;
	   ADCCalibrationParameters[channel][np+1] = atof(LineBuffer.c_str());
	  }
	  
	  if(npars==2)
	  {
	    printf("Two calibration parameters - save in the usual linear gain structure\n");
	    offset = ADCCalibrationParameters[channel][1];
	    gain = ADCCalibrationParameters[channel][2];
	    
	    printf("Channel: %d\tnpars: %d\tOffset: %f\tGain: %f\n",channel,(int)ADCCalibrationParameters[channel][0],offset,gain);
	  }
	  if(channel!=-1)SetADCChannelCalibration(channel, offset, gain);
	  }
	}
      }
    }
  
  for(unsigned int i=0;i<32*ADCModules;i++)
  {
    printf("Channel: %d\t",i);
    printf("npars: %d\t",(int)ADCCalibrationParameters[i][0]);
    for(unsigned int j=0;j<(int)ADCCalibrationParameters[i][0];j++)
    {
     printf("parameter %d: %f\t",j,ADCCalibrationParameters[i][j+1]);
    }
  printf("\n");
  }
  CalibInput.close();
  printf("Finished Calibration Parameters\n");
}


/*-------------------------------------------------*/
void ReadX1Offsets(std::string X1offsetsFile)
{
  //printf("Read X1Offsets using file %s\n",X1offsetsFile.c_str());
  
  bool FileRead = true;
  int counter=0;  

  std::ifstream InputFile;
  if(X1offsetsFile.compare(0,6,"ignore") == 0)
  {
    printf("\n ********** Ignoring: X1 offsets for all runs are left at 0 **********\n");
    RunNrForX1Offsets[0]=0;   // for safety, array of 1 created  section "LineBuffer.compare(0,13,"NrOfX1Offsets")"
    X1Offsets[0]= 0;
  }
  else
  {
    InputFile.open(X1offsetsFile.c_str());
    
    if(InputFile.is_open())
    {
      while(FileRead)
      {
	std::string LineBuffer;
	int runnr = 0;
	double offset = 0;
	InputFile >> LineBuffer;
	if(LineBuffer.compare(0,3,"eof") == 0)
	{
	  FileRead = false;
	}
	else
	{
	  runnr = atoi(LineBuffer.c_str());
	  InputFile >> LineBuffer;
	  offset = atof(LineBuffer.c_str());
 	  printf("Runnr: %d\tOffset: %f\t \n",runnr,offset);          
          RunNrForX1Offsets[counter]=runnr;
          X1Offsets[counter]= offset;
          counter++;
	}
      }
    }
  }
  InputFile.close();

  printf("Finished reading %d X1offsets\n",counter);
}


/*-------------------------------------------------*/
void ReadTOFOffsets(std::string TOFoffsetsFile)
{
  //printf("Read TOFOffsets using file %s\n",TOFoffsetsFile.c_str());
  
  bool FileRead = true;
  int counter=0;  

  std::ifstream InputFile;
  if(TOFoffsetsFile.compare(0,6,"ignore") == 0)
  {
    printf("\n ********** Ignoring: TOF offsets for all runs are left at 0 **********\n");
    RunNrForTOFOffsets[0]=0;   // for safety, array of 1 created  section "LineBuffer.compare(0,13,"NrOfTOFOffsets")"
    TOFOffsets[0]= 0;
  }
  else
  {
    InputFile.open(TOFoffsetsFile.c_str());
    
    if(InputFile.is_open())
    {
      while(FileRead)
      {
	std::string LineBuffer;
	int runnr = 0;
	double offset = 0;
	InputFile >> LineBuffer;
	if(LineBuffer.compare(0,3,"eof") == 0)
	{
	  FileRead = false;
	}
	else
	{
	  runnr = atoi(LineBuffer.c_str());
	  InputFile >> LineBuffer;
	  offset = atof(LineBuffer.c_str());
 	  printf("Runnr: %d\tOffset: %f\t \n",runnr,offset);          
          RunNrForTOFOffsets[counter]=runnr;
          TOFOffsets[counter]= offset;
          counter++;
	}
      }
    }
  }
  InputFile.close();

  printf("Finished reading %d TOFoffsets\n",counter);
}






/*-------------------------------------------------*/
void ReadPadOffsets(std::string PadoffsetsFile)
{
  //printf("Read PadOffsets using file %s\n",PadoffsetsFile.c_str());
  
  bool FileRead = true;
  int counter=0;  

  std::ifstream InputFile;
  if(PadoffsetsFile.compare(0,6,"ignore") == 0)
  {
    printf("\n ********** Ignoring: Pad offsets for all runs are left at 0 **********\n");
    RunNrForPadOffsets[0]=0;   // for safety, array of 1 created  section "LineBuffer.compare(0,13,"NrOfPadOffsets")"
    PadOffsets[0]= 0;
  }
  else
  {
    InputFile.open(PadoffsetsFile.c_str());
    
    if(InputFile.is_open())
    {
      while(FileRead)
      {
	std::string LineBuffer;
	int runnr = 0;
	double offset = 0;
	InputFile >> LineBuffer;
	if(LineBuffer.compare(0,3,"eof") == 0)
	{
	  FileRead = false;
	}
	else
	{
	  runnr = atoi(LineBuffer.c_str());
	  InputFile >> LineBuffer;
	  //std::cout<< "TESTTESTTETS: " << LineBuffer.c_str() << std::endl;
	  offset = atof(LineBuffer.c_str());
 	  printf("Runnr: %d\tOffset: %f\t \n",runnr,offset);          
          RunNrForPadOffsets[counter]=runnr;
          PadOffsets[counter]= offset;
          counter++;
	}
      }
    }
  }
  InputFile.close();

  printf("Finished reading %d TPadoffsets\n",counter);
}


/*-------------------------------------------------*/
void ExRunCorrectionsInit(int runs)
{
  ExCorrTerms = new double*[runs];
  for(int i=0;i<runs;i++)
    {
      ExCorrTerms[i] = new double[2];
      ExCorrTerms[i][0] = 0;
      ExCorrTerms[i][1] = 0;
    }
}

/*-------------------------------------------------*/
void ReadExCorrectionTerms(std::string filename)
{
  bool Reading = true;

  if(filename.compare(0,6,"compare") == 0)
    {
      printf("Ignore the Ex correction terms\n");
    }
  else
    {
      std::ifstream input;
      input.open(filename.c_str());
      if(input.is_open())
	{
	  int runCounter = 0;
	  while(Reading)
	    {
	      std::string LineBuffer;
	      int runs = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,3,"eof") == 0)
		{
		  //End-of-file
		  Reading = false;
		}
	      else if(LineBuffer.compare(0,4,"runs") == 0)
		{
		  //Number of runs used
		  input >> LineBuffer;
		  runs = atoi(LineBuffer.c_str());
		  ExRunCorrectionsInit(runs);
		  TotalRunsNumber = runs;
		}
	      else
		{
		  int run = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  float ExCorr = atof(LineBuffer.c_str());
		  printf("Run: %d \t ExCorr: %f\n",run,ExCorr);
		  ExCorrTerms[runCounter][0] = run;
		  ExCorrTerms[runCounter][1] = ExCorr;
		  runCounter++;
		}
	    }
	}
    }
}

/*-------------------------------------------------*/
void LoadExCorrection(int run)
{
  bool foundRun = false;
  printf("Load Ex Correction term for run  %d\n",run);
  printf("TotalRunsNumber: %d\n",TotalRunsNumber);

  for(int i=0;i<TotalRunsNumber;i++)
    {
      if(ExCorrTerms[i][0] == run)
	{
	  ExCorrection = ExCorrTerms[i][1];
	  foundRun = true;
	}
    }
  if(!foundRun)printf("For Ex correction: run not found\n");
}

/*-------------------------------------------------*/
void SetADCChannelCalibration(int channel, double offset, double gain)
{
  if(channel<ADCsize)
  {
    ADCOffsets[channel] = offset;
    ADCGains[channel]   = gain;
  }
}

/*-------------------------------------------------*/
void ADCInit()
{
  printf("ADCInit\n");
  ADC = new float[32*ADCModules];	     
  ADCchannel = new int[32*ADCModules];	     
  ADCClear();
}

void ADCClear()
{
  for(int i=0;i<ADCsize;i++)
  {
    ADC[i] = 0;
    ADCchannel[i] = -1;
  }
}

/*-------------------------------------------------*/
void TDCInit()
{
  printf("TDCInit\n");
  TDCOffsets = new double[128*TDCModules];
  TDCOffsetsClear();
}

void TDCOffsetsClear()
{
  for(int i=0;i<TDCsize;i++)
    {
      TDCOffsets[i] = 0;
    }
}

void SetTDCChannelOffset(int channel, double offset)
{
  TDCOffsets[channel] = offset;
}

void ReadTDCOffsets(std::string OffsetsFile)
{
  bool Reading = true;

  if(OffsetsFile.compare(0,6,"ignore") == 0)
    {
      printf("Ignore the TDC offsets\n");
    }
  else
    {
      std::ifstream input;
      input.open(OffsetsFile.c_str());
      if(input.is_open())
	{
	  while(Reading)
	    {
	      std::string LineBuffer;
	      int channel = -1;
	      double offset = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,3,"eof") == 0)
		{
		  Reading = false;
		}
	      else
		{
		  channel = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  offset = atof(LineBuffer.c_str());
		  printf("TDC Channel: %d\tOffset: %f\t",channel,offset);
		  if(channel!=-1)SetTDCChannelOffset(channel, offset);
		}
	    }
	}
    }
}

/*-------------------------------------------------*/
void QDCInit()
{
  printf("QDCInit\n");
  QDC = new float[QDCsize];
  QDCClear();
}

/*-------------------------------------------------*/
void QDCClear()
{
  for(int i=0;i<QDCsize;i++)
  {
    QDC[i] = 0;
  }
}

/*-------------------------------------------------*/
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
  bool ScintillatorUsed = false;
  bool ScintillatorADCChannelRead = false;
  bool ScintillatorTDCChannelRead = false;
  bool ThSCATCorrectionParametersRead = false;

  bool LineshapeCorrectionParametersRead = false; 
  bool TOFCorrParametersRead = false; 
  bool ThSCATXCorrectionParametersRead = false; 
  bool ThSCATXLoffCorrectionParametersRead = false;
  bool TOFXCorrectionParametersRead = false;

  bool XRigidityParametersRead = false;
  bool Y1CorrectionParametersRead = false;
  bool GateauRead = false;
  bool ThFPSCATOffsetParametersRead = false;
  bool ThFPSCATSlopeParametersRead = false;
  bool X1OffsetParametersRead = false;
  bool TOFOffsetParametersRead = false;

  bool PadOffsetParametersRead = false;

  std::ifstream input;

  input.open("config.cfg");  //This is the line to change in order to change the configuration file

 if(input.is_open())
    {
      while(ConfigRead)
	{
	  std::string LineBuffer;


	  if(!MMMADCChannelRead && !MMMTDCChannelRead && !W1ADCChannelRead && !W1TDCChannelRead && !HagarADCChannelRead && !HagarTDCChannelRead && !CloverADCChannelRead && !CloverTDCChannelRead && !ScintillatorADCChannelRead && !ScintillatorTDCChannelRead && !ThSCATCorrectionParametersRead&& !ThSCATCorrectionParametersRead && !ThSCATXCorrectionParametersRead && !LineshapeCorrectionParametersRead && !XRigidityParametersRead && !Y1CorrectionParametersRead && !GateauRead && !ThFPSCATOffsetParametersRead &&!ThFPSCATSlopeParametersRead && !ThSCATXLoffCorrectionParametersRead && !X1OffsetParametersRead  && !TOFOffsetParametersRead  && !TOFXCorrectionParametersRead  && !TOFCorrParametersRead && !PadOffsetParametersRead)
	    {
	      input >> LineBuffer;
// 	      printf("Linebuffer: %s\n", LineBuffer.c_str());
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
		  else if(LineBuffer.compare(0,2,"no") == 0)
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
	      else if(LineBuffer.compare(0,16,"ScintillatorUsed") == 0)
		{
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,3,"yes") == 0)
		    {
		    //  ScintillatorInit();
		      ScintillatorUsed = true;
		    }
		  else if(LineBuffer.compare(0,2,"no") == 0)
		    {
		      ScintillatorUsed = false;
		    }
		  else
		    {
		      printf("Scintillator usage option not recognised\n");
		    }
		}
              else if(LineBuffer.compare(0,20,"NumberOfScintillator") == 0)
                {
                  input >> LineBuffer;
                  NumberOfScintillator = atoi(LineBuffer.c_str());
                  ScintillatorNumberInit();
                }
	      else if(LineBuffer.compare(0,23,"ScintillatorADCChannels") == 0)
		{
		  if(ScintillatorADCChannelRead==false)ScintillatorADCChannelRead = true;
		  else if(ScintillatorADCChannelRead==true)ScintillatorADCChannelRead = false;
		}
	      else if(LineBuffer.compare(0,23,"ScintillatorTDCChannels") == 0)
		{
		  if(ScintillatorTDCChannelRead==false)ScintillatorTDCChannelRead = true;
		  else if(ScintillatorTDCChannelRead==true)ScintillatorTDCChannelRead = false;
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
		  if(LineBuffer.compare(3,2,"ux") == 0)
		    {
		      printf("VDC1 is an new-type wire chamber, U wires first\n");
		      VDC1_new_UX = true;
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
		  if(LineBuffer.compare(3,2,"ux") == 0)
		    {
		      printf("VDC1 is an new-type wire chamber, U wires first\n");
		      VDC2_new_UX = true;
		    }
		}
	      else if(LineBuffer.compare(0,15,"CalibrationFile") == 0)
		{
		  input >> LineBuffer;
		  printf("Using calibration file: %s\n",LineBuffer.c_str());
		  ReadCalibrationParameters(LineBuffer);
		}

//===============================================================================================
	      else if(LineBuffer.compare(0,13,"NrOfX1Offsets") == 0)
                {
		  input >> LineBuffer;
		  printf("Reading %d X1offsets\n",atoi(LineBuffer.c_str()));
		  NrOfRunsForX1Offsets = atoi(LineBuffer.c_str());
                  if(NrOfRunsForX1Offsets<1) NrOfRunsForX1Offsets=1;    //if you put 0 in config I will create at least 1 entry for safety
                  RunNrForX1Offsets = new int[NrOfRunsForX1Offsets];
                  X1Offsets = new double[NrOfRunsForX1Offsets];
	        }
	      else if(LineBuffer.compare(0,12,"XOffsetsFile") == 0)
		{
		  input >> LineBuffer;
		  printf("Using X1offsets file: %s\n",LineBuffer.c_str());
		  ReadX1Offsets(LineBuffer);
		}
//===============================================================================================
	      else if(LineBuffer.compare(0,14,"NrOfTOFOffsets") == 0)
                {
		  input >> LineBuffer;
		  printf("Reading %d TOFoffsets\n",atoi(LineBuffer.c_str()));
		  NrOfRunsForTOFOffsets = atoi(LineBuffer.c_str());
                  if(NrOfRunsForTOFOffsets<1) NrOfRunsForTOFOffsets=1;    //if you put 0 in config I will create at least 1 entry for safety
                  RunNrForTOFOffsets = new int[NrOfRunsForTOFOffsets];
                  TOFOffsets = new double[NrOfRunsForTOFOffsets];
	        }
	      else if(LineBuffer.compare(0,14,"TOFOffsetsFile") == 0)
		{
		  input >> LineBuffer;
		  printf("Using TOFoffsets file: %s\n",LineBuffer.c_str());
		  ReadTOFOffsets(LineBuffer);
		}
	      else if(LineBuffer.compare(0,14,"NrOfPadOffsets") == 0)
                {
		  input >> LineBuffer;
		  printf("Reading %d Padoffsets\n",atoi(LineBuffer.c_str()));
		  NrOfRunsForPadOffsets = atoi(LineBuffer.c_str());
                  if(NrOfRunsForPadOffsets<1) NrOfRunsForPadOffsets=1;    //if you put 0 in config I will create at least 1 entry for safety
                  RunNrForPadOffsets = new int[NrOfRunsForPadOffsets];
                  PadOffsets = new double[NrOfRunsForPadOffsets];
	        }
	      else if(LineBuffer.compare(0,14,"PadOffsetsFile") == 0)
		{
		  input >> LineBuffer;
		  printf("Using Padoffsets file: %s\n",LineBuffer.c_str());
		  ReadPadOffsets(LineBuffer);
		}
	      else if(LineBuffer.compare(0,14,"TDCOffsetsFile") == 0)
		{
		  input >> LineBuffer;
		  printf("Using TDC Offsets file: %s\n",LineBuffer.c_str());
		  ReadTDCOffsets(LineBuffer);
		}
	      else if(LineBuffer.compare(0,17,"ExCorrectionTerms") == 0)
		{
		  input >> LineBuffer;
		  printf("Using Ex correction file: %s\n",LineBuffer.c_str());
		  ReadExCorrectionTerms(LineBuffer);
		  
		  //added by F. Diel
		} else if(LineBuffer.compare(0,24,"LineshapeCorrectionTerms") == 0){
		  input >> LineBuffer;
		  printf("Using %d terms for the lineshape correction\n",atoi(LineBuffer.c_str()));
		  NThetaXCorr_FD = atoi(LineBuffer.c_str());
		  ThetaXCorr_i1_FD = new double[NThetaXCorr_FD];
		  ThetaXCorr_i2_FD = new double[NThetaXCorr_FD];
		  ThetaXCorr_par_FD = new double[NThetaXCorr_FD];
		  
		  for(int c=0;c<NThetaXCorr_FD;c++){
			  
			  ThetaXCorr_i1_FD[c] = 0;
			  ThetaXCorr_i2_FD[c] = 0;
			  ThetaXCorr_par_FD[c] = 0;
		  }
		  LineshapeCorrectionParametersRead = true;
		} else if(LineBuffer.compare(0,18,"TOFCorrectionTerms") == 0){
			TOFCorrParametersRead = true;
			input >> LineBuffer;
			printf("Using %d terms for the TOF correction\n",atoi(LineBuffer.c_str()));
			NTOFCorr_FD = atoi(LineBuffer.c_str());
			TOFCorr_i1_FD = new double[NThetaXCorr_FD];
			TOFCorr_i2_FD = new double[NThetaXCorr_FD];
			TOFCorr_par_FD = new double[NThetaXCorr_FD];
		  
			for(int c=0;c<NTOFCorr_FD;c++){
			  
				TOFCorr_i1_FD[c] = 0;
				TOFCorr_i2_FD[c] = 0;
				TOFCorr_par_FD[c] = 0;
			}
			
			
			
		} else if(LineBuffer.compare(0,21,"ThSCATCorrectionTerms") == 0)
		{
		  input >> LineBuffer;
		  printf("Using %d terms for the ThSCAT position correction\n",atoi(LineBuffer.c_str()));
		  NXThetaCorr = atoi(LineBuffer.c_str());
		  XThetaCorr = new double[NXThetaCorr];
		  for(int c=0;c<NXThetaCorr;c++)XThetaCorr[c] = 0;
		  ThSCATCorrectionParametersRead = true;
		}
	      else if(LineBuffer.compare(0,22,"ThSCATXCorrectionTerms") == 0)
		{
		  input >> LineBuffer;
		  printf("Using %d terms for the ThSCAT*X position correction\n",atoi(LineBuffer.c_str()));
		  NXThetaXCorr = atoi(LineBuffer.c_str());
		  XThetaXCorr = new double[NXThetaXCorr];
		  for(int c=0;c<NXThetaXCorr;c++)XThetaXCorr[c] = 0;
		  ThSCATXCorrectionParametersRead = true;
		}

	      else if(LineBuffer.compare(0,26,"ThSCATXLoffCorrectionTerms") == 0)
		{		  				
		  input >> LineBuffer;
		  printf("Using %d terms for the ThSCAT^n*(X position-offset) correction\n",atoi(LineBuffer.c_str()));
		  NXThetaXLoffCorr = atoi(LineBuffer.c_str());
		  XThetaXLoffCorr = new double[NXThetaXLoffCorr];
		  for(int c=0;c<NXThetaXLoffCorr;c++)XThetaXLoffCorr[c] = 0;
		  ThSCATXLoffCorrectionParametersRead = true;			
		}

	      else if(LineBuffer.compare(0,18,"TOFCorrectionTerms") == 0)
		{		  				
		  input >> LineBuffer;
		  printf("Using %d terms for the (TOF-offset) correction\n",atoi(LineBuffer.c_str()));
		  NXTOFCorr = atoi(LineBuffer.c_str());
		  XTOFCorr = new double[NXTOFCorr];
		  for(int c=0;c<NXTOFCorr;c++)XTOFCorr[c] = 0;
		  TOFXCorrectionParametersRead = true;			
		}

	      else if(LineBuffer.compare(0,19,"InelasticScattering") ==0)
		{
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,4,"true") == 0)TestInelastic = true;
		  else if(LineBuffer.compare(0,5,"false") == 0)TestInelastic = false;
		  else TestInelastic = true;
		  if(TestInelastic)printf("Going to do excitation energy calculation assuming inelastic scattering\n");
		}
	      else if(LineBuffer.compare(0,19,"ThFPSCATOffsetTerms") == 0)
		{
		  input >> LineBuffer;
		  printf("Using %d terms for thetaFP to thetaSCAT conversion: offset \n",atoi(LineBuffer.c_str()));
		  NThFPSCATOffset = atoi(LineBuffer.c_str());
		  ThFPSCATOffset = new double[NThFPSCATOffset];
		  for(int c=0;c<NThFPSCATOffset;c++) ThFPSCATOffset[c] = 0;
		  ThFPSCATOffsetParametersRead = true;
		}
	      else if(LineBuffer.compare(0,18,"ThFPSCATSlopeTerms") == 0)
		{
		  input >> LineBuffer;
		  printf("Using %d terms for thetaFP to thetaSCAT conversion: slope \n",atoi(LineBuffer.c_str()));
		  NThFPSCATSlope = atoi(LineBuffer.c_str());
		  ThFPSCATSlope = new double[NThFPSCATSlope];
		  for(int c=0;c<NThFPSCATSlope;c++) ThFPSCATSlope[c] = 0;
		  ThFPSCATSlopeParametersRead = true;
		}

	      else if(LineBuffer.compare(0,22,"VDCSeparationDistanceZ") == 0)
		{
		  input >> LineBuffer;
		  printf("VDCSeparationDistanceZ: %f \n",atof(LineBuffer.c_str()));
		  z_x1x2 = atof(LineBuffer.c_str());
		}
	      else if(LineBuffer.compare(0,22,"VDCSeparationDistanceX") == 0)
		{
		  input >> LineBuffer;
		  printf("VDCSeparationDistanceX: %f \n",atof(LineBuffer.c_str()));
		  x_x1x2 = atof(LineBuffer.c_str());
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
		else if(LineBuffer.compare(0,6,"GATEAU") == 0)
		{
		  if(GateauRead==false)GateauRead = true;
		  else if(GateauRead==true)GateauRead = false;
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
	    
	  if(LineshapeCorrectionParametersRead){
		  int npar = -1;
		  int nparX = -1;
		  int nparTh = -1;
		  double valpar = 0;
		  
		  
	  
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,27,"EndLineshapeCorrectionTerms") == 0 && LineshapeCorrectionParametersRead){
			   LineshapeCorrectionParametersRead = false;
		  } else {
			//printf("|%d %s|\n",NThetaXCorr_FD,LineBuffer.c_str());
			
			printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
			npar = atoi(LineBuffer.c_str());
						
			input >> LineBuffer;
			printf("%d\t",atoi(LineBuffer.c_str()));
			nparX = atoi(LineBuffer.c_str());
			
			input >> LineBuffer;
			printf("%d\t",atoi(LineBuffer.c_str()));
			nparTh = atoi(LineBuffer.c_str());
			
			input >> LineBuffer;
			printf("%e\n",atof(LineBuffer.c_str()));
			valpar = atof(LineBuffer.c_str());

		    ThetaXCorr_i1_FD[npar] = nparX;
		    ThetaXCorr_i2_FD[npar] = nparTh;
			ThetaXCorr_par_FD[npar] = valpar;
			
	     }
	  }
	  
	  if(TOFCorrParametersRead){
		  int npar = -1;
		  int nparX = -1;
		  int nparTh = -1;
		  double valpar = 0;
		  
		  
	  
		  input >> LineBuffer;
		  if(LineBuffer.compare(0,21,"EndTOFCorrectionTerms") == 0 && TOFCorrParametersRead){
			   TOFCorrParametersRead = false;
		  } else {
			//printf("|%d %s|\n",NThetaXCorr_FD,LineBuffer.c_str());
			
			printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
			npar = atoi(LineBuffer.c_str());
						
			input >> LineBuffer;
			printf("%d\t",atoi(LineBuffer.c_str()));
			nparX = atoi(LineBuffer.c_str());
			
			input >> LineBuffer;
			printf("%d\t",atoi(LineBuffer.c_str()));
			nparTh = atoi(LineBuffer.c_str());
			
			input >> LineBuffer;
			printf("%e\n",atof(LineBuffer.c_str()));
			valpar = atof(LineBuffer.c_str());

		    TOFCorr_i1_FD[npar] = nparX;
		    TOFCorr_i2_FD[npar] = nparTh;
			TOFCorr_par_FD[npar] = valpar;
			
	     }
	  }
      
	  if(ThSCATCorrectionParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,24,"EndThSCATCorrectionTerms") == 0 && ThSCATCorrectionParametersRead) {
			  ThSCATCorrectionParametersRead = false;
			  }
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

	  if(ThSCATXCorrectionParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      
	      if(LineBuffer.compare(0,25,"EndThSCATXCorrectionTerms") == 0 && ThSCATXCorrectionParametersRead) {
			   ThSCATXCorrectionParametersRead = false;
	      } else {
		  
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  XThetaXCorr[npar] = valpar;
		}
	    }


  	  if(ThSCATXLoffCorrectionParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,29,"EndThSCATXLoffCorrectionTerms") == 0 && ThSCATXLoffCorrectionParametersRead) ThSCATXLoffCorrectionParametersRead = false;
	      else
		{ 
			if(LineBuffer.compare(0,16,"XLineshapeOffset") == 0)
				{
		 			 input >> LineBuffer;
		 			 printf("XLineshapeOffset: %f \n",atof(LineBuffer.c_str()));
		  			 X_LSOffset = atof(LineBuffer.c_str());
					 //printf("X_LSOffset: %f \n",X_LSOffset);
			       	}

		 else
			{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  XThetaXLoffCorr[npar] = valpar;
			}
		}
	     }



  	  if(TOFXCorrectionParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,21,"EndTOFCorrectionTerms") == 0 && TOFXCorrectionParametersRead) TOFXCorrectionParametersRead = false;
	      else
		{ 
			if(LineBuffer.compare(0,18,"TOFLineshapeOffset") == 0)
				{
		 			 input >> LineBuffer;
		 			 printf("TOFLineshapeOffset: %f \n",atof(LineBuffer.c_str()));
		  			 TOF_LSOffset = atof(LineBuffer.c_str());
					 //printf("X_LSOffset: %f \n",X_LSOffset);
			       	}

		 else
			{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  XTOFCorr[npar] = valpar;
			}
		}
	     }

	  if(Y1CorrectionParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,20,"EndY1CorrectionTerms") == 0 && Y1CorrectionParametersRead) Y1CorrectionParametersRead = false;
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

      
	  if(ThFPSCATOffsetParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,22,"EndThFPSCATOffsetTerms") == 0 && ThFPSCATOffsetParametersRead) ThFPSCATOffsetParametersRead = false;
	      else
		{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  ThFPSCATOffset[npar] = valpar;
		}
	    }

	  if(ThFPSCATSlopeParametersRead)
	    {
	      int npar = -1;
	      double valpar = 0;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,21,"EndThFPSCATSlopeTerms") == 0 && ThFPSCATSlopeParametersRead) ThFPSCATSlopeParametersRead = false;
	      else
		{
		  printf("Parameter number: %d\t",atoi(LineBuffer.c_str()));
		  npar = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Parameter value: %e\n",atof(LineBuffer.c_str()));
		  valpar = atof(LineBuffer.c_str());
		  ThFPSCATSlope[npar] = valpar;
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

	  if(GateauRead)
	  {
	    int plane = 0, sector = 0, start = -1, stop = -1;
	    
	    input >> LineBuffer;
	    if(LineBuffer.compare(0,6,"GATEAU") == 0)
	    {
	      if(GateauRead==true)GateauRead = false;
	    }
	    else
	    {
	      printf("\n GATEAU wireplane: %d\t",atoi(LineBuffer.c_str()));
	      plane = atoi(LineBuffer.c_str());
	      input >> LineBuffer;
	      printf("Gateau Sector: %d\t",LineBuffer.c_str());
	      sector = atoi(LineBuffer.c_str());
	      input >> LineBuffer;
	      printf("Start: %d\t",LineBuffer.c_str());
	      start = atoi(LineBuffer.c_str());
	      input >> LineBuffer;
	      printf("Stop: %d\t",LineBuffer.c_str());
	      stop = atoi(LineBuffer.c_str());
	      
	      GateauSetChannelLimits(plane,sector,start,stop);
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
	  if(ScintillatorADCChannelRead)
	    {
	      int start = -1, stop = -1;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,23,"ScintillatorADCChannels") == 0)
		{
		  if(ScintillatorADCChannelRead==true)ScintillatorADCChannelRead = false;
		}
	      else
		{
		  printf("ScintillatorADCChannelRead: \t");
		  printf("Start: %d\t",atoi(LineBuffer.c_str()));
		  start = atoi(LineBuffer.c_str());
		  input >> LineBuffer;
		  printf("Stop: %d\n",atoi(LineBuffer.c_str()));
		  stop = atoi(LineBuffer.c_str());
		  ScintillatorADCChannelsInit(start, stop);
		}
	    }

          if(ScintillatorADCChannelRead)
            {
              int num = 0, value = -1;
              std::string side = "";
              input >> LineBuffer;
              printf("%s\n",LineBuffer.c_str());
              if(LineBuffer.compare(0,23,"ScintillatorADCChannels") == 0)
                {
                  if(ScintillatorADCChannelRead==true)ScintillatorADCChannelRead = false;
                }
              else
                {
                  printf("\n Detector number %d\t",atoi(LineBuffer.c_str()));
                  num = atoi(LineBuffer.c_str());
                  input>> LineBuffer;
                  printf("Value: %d\t",atoi(LineBuffer.c_str()));
                  value = atoi(LineBuffer.c_str());
                  ScintillatorADCChannelsInit(num, value);
                }
            }

	  if(ScintillatorTDCChannelRead)
	    {
	      int num = -1, value = -1;
	      input >> LineBuffer;
	      if(LineBuffer.compare(0,23,"ScintillatorTDCChannels") == 0)
		{
		  if(ScintillatorTDCChannelRead==true)ScintillatorTDCChannelRead = false;
		}
	      else
		{
                  printf("\n Detector number %d\t",atoi(LineBuffer.c_str()));
                  num = atoi(LineBuffer.c_str());
                  input>> LineBuffer;
                  printf("Value: %d\t",atoi(LineBuffer.c_str()));
                  value = atoi(LineBuffer.c_str());
                  ScintillatorTDCChannelsInit(num, value);
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

/*-------------------------------------------------*/
void PrintParameters()
{
  printf("ADCModules: %d\n",ADCModules);
  printf("ADCsize: %d\n",ADCsize);
  printf("TDCModules: %d\n",TDCModules);
  printf("TDCsize: %d\n",TDCsize);
}

