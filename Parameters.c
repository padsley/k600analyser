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

int NXRigidityPars;
double *XRigidityPars;

int NrOfRunsForX1Offsets;
int *RunNrForX1Offsets;
double *X1Offsets;

std::vector<std::vector<int>> thetaSCATMappingPars_RunRanges_cache;
std::vector<std::vector<std::vector<double>>> thetaSCATMappingPars_cache;
std::vector<std::vector<double>> thetaSCATMappingPars;

bool X1MappingDefined;
std::vector<std::tuple<int, std::vector<double>>> X1MappingParameters_cache;
std::vector<double> X1MappingParameters;

std::vector<std::tuple<int, double>> Y1offsets;
double Y1offset;

std::vector<std::vector<int>> TLCRunRanges_cache;
std::vector<std::vector<int>> TLCCorrectionTypes_cache;
std::vector<std::vector<std::vector<std::vector<double>>>> TLCParameters_cache;
std::vector<int> TLCRunRanges;
std::vector<int> TLCCorrectionTypes;
std::vector<std::vector<std::vector<double>>> TLCParameters;

bool momentumCalibrationRead;
std::vector<double> momentumCalPars;
std::vector<double> m;
std::vector<double> T;
std::vector<double> E;
std::vector<double> p;
std::vector<double> polarScatteringAngles;

double z_x1x2,x_x1x2;

int NrOfRunsForTOFOffsets;
int *RunNrForTOFOffsets;
int *TOFOffsets;

int NrOfRunsForPadOffsets;
int *RunNrForPadOffsets;
int *PadOffsets;

int RunNumber = 0;
int TotalRunsNumber = 0;
double **ExCorrTerms;
double ExCorrection = 0.;


void GetODBRunInfo_Parameters()
// Get globals from the ODB; not needed if we use Parameters and Module Declaration path
{
    HNDLE hDB, hKey;
    RUNINFO runinfo2;
    
    // open ODB structures
    cm_get_experiment_database(&hDB, NULL);
    
    RUNINFO_STR(runinfo_str);            // RUNINFO_STR in experim.h      rn:  not true !?!?!
    db_create_record(hDB, 0, (char *)"/Runinfo", strcomb(runinfo_str));
    db_find_key(hDB, 0, (char *)"/Runinfo", &hKey);
    if (db_open_record(hDB, hKey, &runinfo2, sizeof(runinfo2), MODE_READ, NULL, NULL) !=
        DB_SUCCESS) {
        cm_msg(MERROR, (char *)"analyzer_init",  (char *)"Cannot open \"/Runinfo\" tree in ODB");
        exit(1);
    }
    printf("\n==================================================================== %i  \n\n",runinfo2.run_number);
    extern int RunNumber;
    RunNumber = runinfo2.run_number;
    printf("RunNumber: \t%d\n",RunNumber);
    //LoadExCorrection(RunNumber);               // not used at present
    db_close_record(hDB,hKey);
}

/*-------------------------------------------------*/
void ParameterInit()
{
    printf("\n ParameterInit\n");
    
    m = std::vector<double>(4, 0);
    T = std::vector<double>(4, 0);
    E = std::vector<double>(4, 0);
    p = std::vector<double>(4, 0);
    polarScatteringAngles = std::vector<double>(4, 0);
    
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
void ReadThetaSCATMappingPars_cache(std::string thetaSCATMappingPars_cacheFile)
{
    //----------------------------------------------------
    std::ifstream InputFile;
    InputFile.open(thetaSCATMappingPars_cacheFile.c_str());
    
    if(InputFile.is_open())
    {
        std::string LineBuffer;
        
        InputFile >> LineBuffer;
        
        while(LineBuffer.compare(0,3,"eof") != 0)
        {
            std::vector<std::vector<double>> thetaSCATMappingPars_cache_perRunRange;
            std::vector<int> runRange;
            
            //------------------------
            //      Run range
            int minRunRange, maxRunRange;
            
            minRunRange = atoi(LineBuffer.c_str());
            InputFile >> LineBuffer;
            maxRunRange = atoi(LineBuffer.c_str());
            
            int runNumbers = minRunRange;
            if(minRunRange<=maxRunRange)
            {
                while(runNumbers<=maxRunRange)
                {
                    //----------------------------------------------------------------------------------------------------
                    //  Deleting any previously defined corrections for each run
                    //  This ensures that a run cannot erronouesly undergo two different sets of corrections
                    //  The last TLC correction for a run, given in the assocaited TLCParameters_cacheFile, is correction used.
                    for(auto i = thetaSCATMappingPars_RunRanges_cache.begin(); i != thetaSCATMappingPars_RunRanges_cache.end(); ++i)
                    {
                        int it_i = std::distance(thetaSCATMappingPars_RunRanges_cache.begin(), i);
                        
                        for(auto j = (*i).begin(); j != (*i).end(); ++j)
                        {
                            int it_j = std::distance((*i).begin(), j);
                            
                            if((*j)==runNumbers)
                            {
                                thetaSCATMappingPars_RunRanges_cache[it_i].erase(thetaSCATMappingPars_RunRanges_cache[it_i].begin() + it_j);
                            }
                        }
                        
                    }
                    
                    runRange.push_back(runNumbers);
                    runNumbers++;
                }
            }
            else
            {
                printf("ERROR in thetaSCATMappingPars_cache: invalid run range\n");
            }
            
            
            //----------------------------
            //      Offset parameters
            int nOffsetPars;
            std::vector<double> offsetPars;

            InputFile >> LineBuffer;
            nOffsetPars = atoi(LineBuffer.c_str());
            
            for(int i=0; i<nOffsetPars; i++)
            {
                InputFile >> LineBuffer;
                offsetPars.push_back(atof(LineBuffer.c_str()));
            }
            
            thetaSCATMappingPars_cache_perRunRange.push_back(offsetPars);

            //----------------------------
            //      Gain parameters
            int nGainPars;
            std::vector<double> gainPars;

            InputFile >> LineBuffer;
            nGainPars = atoi(LineBuffer.c_str());
            
            for(int i=0; i<nGainPars; i++)
            {
                InputFile >> LineBuffer;
                gainPars.push_back(atof(LineBuffer.c_str()));
            }
            
            thetaSCATMappingPars_cache_perRunRange.push_back(gainPars);
            
            //------------------------------------------------------------
            
            thetaSCATMappingPars_RunRanges_cache.push_back(runRange);
            thetaSCATMappingPars_cache.push_back(thetaSCATMappingPars_cache_perRunRange);
            
            InputFile >> LineBuffer;
        }
    }
    
    InputFile.close();
    
    std::cout << std::endl;
    
    //----------------------------------------------------
    
    printf("Finished reading ThetaSCAT Mapping Parameters\n");
}

/*-------------------------------------------------*/
void ReadY1Offsets(std::string Y1offsetsFile)
{
    std::ifstream InputFile;
    
    InputFile.open(Y1offsetsFile.c_str());
    
    if(InputFile.is_open())
    {
        int runNr = 0;
        double offset = 0;
        std::string LineBuffer;
        
        InputFile >> LineBuffer;

        while(LineBuffer.compare(0,3,"eof") != 0)
        {
            runNr = atoi(LineBuffer.c_str());
            InputFile >> LineBuffer;

            offset = atof(LineBuffer.c_str());
            Y1offsets.push_back(std::make_tuple(runNr, offset));
            InputFile >> LineBuffer;
        }
    }
    
    InputFile.close();
    
    printf("Finished reading Y1 offsets\n");
}

/*-------------------------------------------------*/
void ReadX1Mapping(std::string X1mappingFile)
{
    std::ifstream InputFile;
    
    InputFile.open(X1mappingFile.c_str());
    
    if(InputFile.is_open())
    {
        int runNr = 0;
        std::string LineBuffer;
        
        InputFile >> LineBuffer;
        while(LineBuffer.compare(0,3,"eof") != 0)
        {
            runNr = atoi(LineBuffer.c_str());
            
            InputFile >> LineBuffer;
            int nOrderMapping = atoi(LineBuffer.c_str());
            
            std::vector<double> X1MappingParameters_perRun;

            for(int i=0; i<nOrderMapping; i++)
            {
                InputFile >> LineBuffer;
                X1MappingParameters_perRun.push_back(atof(LineBuffer.c_str()));
            }
            
            X1MappingParameters_cache.push_back(std::make_tuple(runNr,X1MappingParameters_perRun));
            
            InputFile >> LineBuffer;
        }
        
        /*
        InputFile >> LineBuffer;
        if(LineBuffer.compare(0,3,"eof") != 0)
        {
            runNr = atoi(LineBuffer.c_str());
        }
        
        std::vector<double> X1MappingParameters_perRun;
        InputFile >> LineBuffer;
        
        while(LineBuffer.compare(0,3,"eof") != 0)
        {
            double mappingParameter = atof(LineBuffer.c_str());
            X1MappingParameters_perRun.push_back(mappingParameter);
            
            InputFile >> LineBuffer;
        }
        
        X1MappingParameters_cache.push_back(std::make_tuple(runNr,X1MappingParameters_perRun));
        */
        
    }

    InputFile.close();
    printf("Finished reading X1 Mapping Parameters\n");
}

/*-------------------------------------------------*/
void ReadTLCParameters_cache(std::string TLCParameters_cacheFile)
{
    //----------------------------------------------------
    std::ifstream InputFile;
    InputFile.open(TLCParameters_cacheFile.c_str());
    
    if(InputFile.is_open())
    {
        std::string LineBuffer;

        InputFile >> LineBuffer;

        while(LineBuffer.compare(0,3,"eof") != 0)
        {
            std::vector<int> correctionTypes_perRunRange;
            std::vector<std::vector<std::vector<double>>> TLCParameters_cache_perRunRange;
            std::vector<int> runRange;

            //------------------------
            //      Run range
            int minRunRange, maxRunRange;
            
            minRunRange = atoi(LineBuffer.c_str());
            InputFile >> LineBuffer;
            maxRunRange = atoi(LineBuffer.c_str());
            
            int runNumbers = minRunRange;
            if(minRunRange<=maxRunRange)
            {
                while(runNumbers<=maxRunRange)
                {
                    //----------------------------------------------------------------------------------------------------
                    //  Deleting any previously defined corrections for each run
                    //  This ensures that a run cannot erronouesly undergo two different sets of corrections
                    //  The last TLC correction for a run, given in the assocaited TLCParameters_cacheFile, is correction used.
                    for(auto i = TLCRunRanges_cache.begin(); i != TLCRunRanges_cache.end(); ++i)
                    {
                        int it_i = std::distance(TLCRunRanges_cache.begin(), i);
                        
                        for(auto j = (*i).begin(); j != (*i).end(); ++j)
                        {
                            int it_j = std::distance((*i).begin(), j);

                            if((*j)==runNumbers)
                            {
                                TLCRunRanges_cache[it_i].erase(TLCRunRanges_cache[it_i].begin() + it_j);
                            }
                        }
                        
                    }
                    
                    runRange.push_back(runNumbers);
                    runNumbers++;
                }
            }
            else
            {
                printf("ERROR in TLCParameters_cache: invalid run range\n");
            }
            
            //----------------------------
            //      Correction Types
            int nCorrections;
            
            InputFile >> LineBuffer;
            nCorrections = atoi(LineBuffer.c_str());
            
            for(int i=0; i<nCorrections; i++)
            {
                InputFile >> LineBuffer;
                correctionTypes_perRunRange.push_back(atoi(LineBuffer.c_str()));
            }
            
            for(int i=0; i<(int) correctionTypes_perRunRange.size(); i++)
            {
                std::cout << "correctionTypes_perRunRange: " << correctionTypes_perRunRange[i] << std::endl;
            }
            
            for(int i=0; i<nCorrections; i++)
            {
                InputFile >> LineBuffer;
                int polOrder = atoi(LineBuffer.c_str());
                
                std::vector<std::vector<double>> correctionParameters;
                
                for(int j=0; j<polOrder; j++)
                {
                    InputFile >> LineBuffer;
                    int nParameters = atoi(LineBuffer.c_str()); // nParameters: the order of the polynomial used to model the polynomial coefficient (order=polOrder)
                    
                    std::vector<double> polModelParameters;
                    for(int k=0; k<nParameters; k++)
                    {
                        InputFile >> LineBuffer;
                        polModelParameters.push_back(atof(LineBuffer.c_str()));
                    }
                    
                    correctionParameters.push_back(polModelParameters);
                }
                
                TLCParameters_cache_perRunRange.push_back(correctionParameters);
            }
            
            //----------------------------

            TLCCorrectionTypes_cache.push_back(correctionTypes_perRunRange);
            TLCRunRanges_cache.push_back(runRange);
            TLCParameters_cache.push_back(TLCParameters_cache_perRunRange);
            
            InputFile >> LineBuffer;
        }
      
    }

    InputFile.close();
    
    
    //----------------------------
    for(int h=0; h<(int) TLCParameters_cache.size(); h++)
    {
        std::cout << "//----------------------------------------------------" << std::endl;

        std::cout << "TLCCorrectionTypes_cache: ";
        for(int j=0; j<(int) TLCCorrectionTypes_cache[h].size(); j++)
        {
            if(j!=0) {std::cout << ", ";}
            std::cout << TLCCorrectionTypes_cache[h][j];
        }
        std::cout << std::endl;
        
        //----------------------------------------
        std::cout << "TLCRunRanges_cache: ";
        for(int j=0; j<(int) TLCRunRanges_cache[h].size(); j++)
        {
            if(j!=0) {std::cout << ", ";}
            std::cout << TLCRunRanges_cache[h][j];
        }
        std::cout << std::endl;
        
        //----------------------------------------
        for(int i=0; i<(int) TLCParameters_cache[h].size(); i++)
        {
            std::cout << "correctionType: " << TLCCorrectionTypes_cache[h][i] << std::endl;
            
            for(int j=0; j<(int) TLCParameters_cache[h][i].size(); j++)
            {
                std::cout << "TLCParameters_cache[h]: ";
                
                for(int k=0; k<(int) TLCParameters_cache[h][i][j].size(); k++)
                {
                    if(k!=0) {std::cout << ", ";}
                    std::cout << TLCParameters_cache[h][i][j][k];
                }
                
                std::cout << std::endl;
            }
        }
    }
    
    
    std::cout << std::endl;
    
    //----------------------------------------------------

    printf("Finished reading TLC Parameters\n");
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
                int runNr = 0;
                double offset = 0;
                InputFile >> LineBuffer;
                if(LineBuffer.compare(0,3,"eof") == 0)
                {
                    FileRead = false;
                }
                else
                {
                    runNr = atoi(LineBuffer.c_str());
                    InputFile >> LineBuffer;
                    offset = atof(LineBuffer.c_str());
                    printf("runNr: %d\tOffset: %f\t \n",runNr,offset);
                    RunNrForTOFOffsets[counter]=runNr;
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
                int runNr = 0;
                double offset = 0;
                InputFile >> LineBuffer;
                if(LineBuffer.compare(0,3,"eof") == 0)
                {
                    FileRead = false;
                }
                else
                {
                    runNr = atoi(LineBuffer.c_str());
                    InputFile >> LineBuffer;
                    offset = atof(LineBuffer.c_str());
                    printf("runNr: %d\tOffset: %f\t \n",runNr,offset);
                    RunNrForPadOffsets[counter]=runNr;
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
    
    bool ThSCATXCorrectionParametersRead = false;
    bool ThSCATXLoffCorrectionParametersRead = false;
    bool TOFXCorrectionParametersRead = false;
    
    bool Y1CorrectionParametersRead = false;
    bool GateauRead = false;
    bool ThFPSCATOffsetParametersRead = false;
    bool ThFPSCATSlopeParametersRead = false;
    bool X1OffsetParametersRead = false;
    bool TOFOffsetParametersRead = false;
    
    bool PadOffsetParametersRead = false;
    
    std::ifstream input;
    
    GetODBRunInfo_Parameters();
    
    input.open("config.cfg");  //This is the line to change in order to change the configuration file
    
    std::cout << "CHICKEN PARAMETERS.C, RunNumber: " << RunNumber << std::endl;
    
    if(input.is_open())
    {
        while(ConfigRead)
        {
            std::string LineBuffer;
            
            
            if(!MMMADCChannelRead && !MMMTDCChannelRead && !W1ADCChannelRead && !W1TDCChannelRead && !HagarADCChannelRead && !HagarTDCChannelRead && !CloverADCChannelRead && !CloverTDCChannelRead && !ScintillatorADCChannelRead && !ScintillatorTDCChannelRead && !ThSCATCorrectionParametersRead && !ThSCATXCorrectionParametersRead && !Y1CorrectionParametersRead && !GateauRead && !ThFPSCATOffsetParametersRead &&!ThFPSCATSlopeParametersRead && !ThSCATXLoffCorrectionParametersRead && !X1OffsetParametersRead  && !TOFOffsetParametersRead  && !TOFXCorrectionParametersRead  && !PadOffsetParametersRead)
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
                else if(LineBuffer.compare(0,30,"ThetaSCATMappingParametersFile") == 0)
                {
                    input >> LineBuffer;
                    printf("Using thetaSCAT to thetaFP Mappings file: %s\n",LineBuffer.c_str());
                    
                    ReadThetaSCATMappingPars_cache(LineBuffer);
                }
                //===============================================================================================
                else if(LineBuffer.compare(0,23,"X1MappingParametersFile") == 0)
                {
                    input >> LineBuffer;
                    printf("Using X1 mapping parameters file: %s\n",LineBuffer.c_str());
                    ReadX1Mapping(LineBuffer);
                }
                //===============================================================================================
                else if(LineBuffer.compare(0,13,"Y1OffsetsFile") == 0)
                {
                    input >> LineBuffer;
                    printf("Using Y1 offsets file: %s\n",LineBuffer.c_str());
                    ReadY1Offsets(LineBuffer);
                }
                //===============================================================================================
                else if(LineBuffer.compare(0,17,"TLCParametersFile") == 0)
                {
                    input >> LineBuffer;
                    printf("Using TLC parameters file: %s\n",LineBuffer.c_str());
                    ReadTLCParameters_cache(LineBuffer);
                }
                //===============================================================================================
                else if(LineBuffer.compare(0,14,"NrOfTOFOffsets") == 0)
                {
                    input >> LineBuffer;
                    printf("Reading %d TOFoffsets\n",atoi(LineBuffer.c_str()));
                    NrOfRunsForTOFOffsets = atoi(LineBuffer.c_str());
                    if(NrOfRunsForTOFOffsets<1) NrOfRunsForTOFOffsets=1;    //if you put 0 in config I will create at least 1 entry for safety
                    RunNrForTOFOffsets = new int[NrOfRunsForTOFOffsets];
                    TOFOffsets = new int[NrOfRunsForTOFOffsets];
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
                    PadOffsets = new int[NrOfRunsForPadOffsets];
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
                else if(LineBuffer.compare(0,14,"ReactionMasses") == 0)
                {
                    for(int i=0; i<4; i++)
                    {
                        input >> LineBuffer;
                        m[i] = atof(LineBuffer.c_str());
                        std::cout << "m[i]: " << m[i] << std::endl;
                    }
                }
                else if(LineBuffer.compare(0,10,"BeamEnergy") == 0)
                {
                    input >> LineBuffer;
                    printf("Beam Energy: %f MeV\n",atof(LineBuffer.c_str()));
                    T[0] = atof(LineBuffer.c_str());
                }
                else if(LineBuffer.compare(0,28,"PolarScatteringAngleEjectile") == 0)
                {
                    input >> LineBuffer;
                    printf("Polar Scattering Angle of ejectile: %f degrees\n",atof(LineBuffer.c_str()));
                    polarScatteringAngles[2] = atof(LineBuffer.c_str());
                }
                else if(LineBuffer.compare(0,19,"MomentumCalibration") == 0)
                {
                    momentumCalibrationRead = true;
                    input >> LineBuffer;
                    printf("Using %d parameters for Momentum Calibration (with respect to X1posC)\n", atoi(LineBuffer.c_str()));
                    
                    int nMomentumCalPars = atoi(LineBuffer.c_str());
                    for(int i=0; i<nMomentumCalPars; i++)
                    {
                        input >> LineBuffer;
                        momentumCalPars.push_back(atof(LineBuffer.c_str()));
                    }
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
            
            if(ThSCATCorrectionParametersRead)
            {
                int npar = -1;
                double valpar = 0;
                input >> LineBuffer;
                if(LineBuffer.compare(0,24,"EndThSCATCorrectionTerms") == 0 && ThSCATCorrectionParametersRead) ThSCATCorrectionParametersRead = false;
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
                if(LineBuffer.compare(0,25,"EndThSCATXCorrectionTerms") == 0 && ThSCATXCorrectionParametersRead) ThSCATXCorrectionParametersRead = false;
                else
                {
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

