/********************************************************************\
  Name:         f-plane.c
  Created by:   Retief Neveling
  Contents:     Focal-plane detector package module. 

  Remember: X wireplane has 198 wires, and 13 preamps. 
            Thus 208 preamp channels from 0-207
            Wire 1 start at preamp ch 10, Wire 198 stops at pream ch 208
            (wire 1 on low momentum side)
  Remember: U-wire plane has 143 wires, and 9 preamps
            Thus 144 preamp channels from 0-143
            Wire 1 start at preamp ch 1, Wire 143 stops at pream ch 143
            (wire 1 on low momentum side)
\********************************************************************/


/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <vector>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"

/* Struck ADC Header and shizzle */
#include "TSIS3302.hxx"
#include "TGenericData.hxx"

/*SDJ includes */
#include <math.h>

/* root includes */
#include <TH1F.h>
#include <TH2F.h>     
#include <TTree.h>
#include <TFile.h>
#include <TRandom3.h>
#include <TMath.h>
#include "Parameters.h"

/* home-made includes */
#include "Parameters.h"
#include "SiliconData.h"
#include "MMM.h"
#include "W1.h"

#include "GammaData.h"
#include "HagarSort.h"
#include "CloverSort.h"

#include "RawData.h"

//#include "GateauData.h"
//#include "GateauSort.h"

/*------------definitions to change analysis------------------------*/
//#define _POLARIZATION
//#define _MOVIE
//#define _JJAUTOTRIM
//#define _PRINTTOSCREEN
//#define _VDCRESCALCS
//#define _FULLANALYSIS
//#define _MISALIGNTIME
#define _ADC

#define _RAWDATA
#define _SILICONDATA 
#define _MMM
//#define _W1
//#define _GAMMADATA
//#define _HAGAR
//#define _GATEAU
#define _TDC
//#define _STRUCKADC

/*-------------------- defined somewhere else ----------------------*/
extern float *ADC;
extern int ADCModules;
extern float *QDC;






/*-- For ODB: from /Analyzer/Parameters and /Equipment/-------------*/
FOCALPLANE_PARAM gates;     // these are to be found in experim.h
GLOBAL_PARAM globals;
WIRECHAMBER_SETTINGS WireChamberSettings;
RUNINFO runinfo2;
  // At present we use 2 methods to get to ODB data:
  // the globals we get through GetODBGlobals() subroutine
  // the gates we get directly from the ODB, see focalplane_module declaration
  // below. The gates will thus change upon changing an ODB entry 'on-the-fly' 
  // while the globals will only change for each initialzation as it is
  // performed in the init routine.

/*-- Module declaration --------------------------------------------*/
INT focal_event(EVENT_HEADER *, void *);
INT focal_bor(INT run_number);
INT focal_eor(INT run_number);
INT focal_init(void);

FOCALPLANE_PARAM_STR(focalplane_param_str);

ANA_MODULE focalplane_module = {
   "focalplane",                /* module name           */
   "Retief Neveling",           /* author                */
   focal_event,                 /* event routine         */
   focal_bor,                   /* BOR routine           */
   focal_eor,                   /* EOR routine           */
   focal_init,                  /* init routine          */
   NULL,                        /* exit routine          */
   &gates,                      /* parameter structure   */
   sizeof(gates),               /* structure size        */
   focalplane_param_str,        /* initial parameters IN ODB????   */
};


/*------------ Global Constants and variables----------------------------------------------*/      
// regarding the constants: these are things I do not want people to change, thus not in ODB.
const Double_t X_WIRESPACE = 4.0;  // distance (mm) between signal wires
const Double_t DRIFTLENGTH = 8.0;  // max distance (mm) the electrons can drift
const Double_t U_WIRE_ANGLE=50.0;    // angle of wires in U-plane wrt to horizontal
const int X_WIRES=208;
const int U_WIRES=145;
const int TDC_CHANNELS=1024;
const int TDC_MAX_TIME=14999;
const int TDC_MIN_TIME=0;
const int TDC_N_BINS=14999;
const int TOF_TDC_CHAN=1;     // ch1 in 1st TDC; i.e. the 2nd channel     
const int MAX_WIRES_PER_EVENT = 400;  // If more wires than this fire, the event data was bad  -- RN random choice  -- 
				      // to be replaced by something else?!?! really only used for array definition
				      // MUST FIND A BETTER WAY ... THIS VARIABLE NOT GOOD?
const int NR_OF_TDCS=8;
const int LUT_CHANNELS=9000;
const int TDC_CHAN_PULSER=2;

#ifdef _POLARIZATION              // for PR153, Sept/Oct 2010
const int TDC_CHAN_POLU=2;
const int TDC_CHAN_POLD=3;
#endif

float lutx1[LUT_CHANNELS];
float lutx2[LUT_CHANNELS];
float lutu1[LUT_CHANNELS];
float lutu2[LUT_CHANNELS];
float cableOffset[896];
char string_var[200];

#ifdef _MISALIGNTIME
int misaligntime;
#endif

unsigned int Channel2Wire[896]; // Variables below used for wire-mapping
  
typedef struct VDC {               // Struct to hold (for each wireplane) wire nr, dt, dd
  Int_t wire[MAX_WIRES_PER_EVENT];
  Int_t time[MAX_WIRES_PER_EVENT];
  Double_t dist[MAX_WIRES_PER_EVENT];
} VDC;
VDC X1;
VDC X2;
VDC U1;
VDC U2;

// TTree variables, all with prefix "t_"
// general TDC variables for TTree
Double_t t_pad1,t_pad2;        
Double_t t_pad1hiP = 0, t_pad1lowP = 0, t_pad2hiP = 0, t_pad2lowP = 0;
Double_t t_pad1hiPT = 0, t_pad1lowPT = 0, t_pad2hiPT = 0, t_pad2lowPT = 0;
Int_t    t_tof,t_toftdc2,t_toftdc3,t_toftdc4,t_toftdc5,t_toftdc6, t_toftdc7;

//
Int_t    t_k600;
Int_t    t_runtime=0;
Int_t    t_evtcounter=0;
Int_t    t_tdcsperevent=0;

Int_t    t_triggerU=0;
Int_t    t_triggerI=0;
Int_t    t_CIU=0;
Int_t    t_CII=0;


Double_t    x1offset=0.0;

// focal plane variables for TTree
Int_t    t_X1hits = 0,  t_X2hits = 0,   t_U1hits = 0,   t_U2hits = 0;
Double_t t_X1pos=-100.0,t_X2pos=-100.0, t_U1pos=-100.0, t_U2pos=-100.0;
Double_t t_X1th=-100.0, t_X2th=-100.0,  t_U1th=-100.0,  t_U2th=-100.0;
Double_t t_X1chisq=15.0,t_X2chisq=15.0, t_U1chisq=15.0, t_U2chisq=15.0;
Int_t    t_X1flag=-100, t_X2flag=-100,  t_U1flag=-100,  t_U2flag=-100;
//Double_t t_X1effID=0,   t_X2effID=0,    t_U1effID=0,    t_U2effID=0;    // these are at present (31may10) not useful in TREE
Double_t t_X1posC=-100.0;
double t_Ex = -0.;

Int_t t_X1chisqminimization=0,t_X2chisqminimization=0, t_U1chisqminimization=0, t_U2chisqminimization=0;

double t_T3 = -0.;
double t_rigidity3 = -0.;
double t_theta = -90;
double t_phi = -180;

// resolution parameters from raytrace subroutine: not all are always needed
Double_t t_X1res0,      t_X2res0,       t_U1res0,       t_U2res0;
Double_t t_X1res1,      t_X2res1,       t_U1res1,       t_U2res1;
#ifdef _VDCRESCALCS
Double_t t_X1res2,      t_X2res2,       t_U1res2,       t_U2res2;
Double_t t_X1res3,      t_X2res3,       t_U1res3,       t_U2res3;
Double_t t_X1res4,      t_X2res4,       t_U1res4,       t_U2res4;
Double_t t_X1res5,      t_X2res5,       t_U1res5,       t_U2res5;
Double_t t_X1res6,      t_X2res6,       t_U1res6,       t_U2res6;
Double_t t_X1res7,      t_X2res7,       t_U1res7,       t_U2res7;
Double_t t_X1res8,      t_X2res8,       t_U1res8,       t_U2res8;
#endif
Double_t t_ThFP=-100; Double_t t_ThFPx=-100;
Double_t t_ThSCAT=-100;
Double_t t_PhiSCAT=-100;
Double_t t_PhiFP=-100;
Double_t t_Y1=-100.0,t_Y2=-100.0;

Double_t t_X1effall, t_X1effdt, t_X1effgroup, t_X1effgood;
Double_t t_U1effall, t_U1effdt, t_U1effgroup, t_U1effgood;
Double_t t_X2effall, t_X2effdt, t_X2effgroup, t_X2effgood;
Double_t t_U2effall, t_U2effdt, t_U2effgroup, t_U2effgood;

#ifdef _FULLANALYSIS
Double_t t_X1TimeDiff=-1000;
Int_t t_nX1wiresUsed, t_nU1wiresUsed, t_nX2wiresUsed, t_nU2wiresUsed;
Int_t t_X1doublewires, t_U1doublewires, t_X2doublewires, t_U2doublewires;
Int_t t_X1multiplemin, t_U1multiplemin, t_X2multiplemin, t_U2multiplemin;
Int_t t_X1wireUsed[MAX_WIRES_PER_EVENT];
Int_t t_U1wireUsed[MAX_WIRES_PER_EVENT];
Int_t t_X2wireUsed[MAX_WIRES_PER_EVENT];
Int_t t_U2wireUsed[MAX_WIRES_PER_EVENT];
Double_t t_X1distUsed[MAX_WIRES_PER_EVENT];
#endif

#ifdef _ADC
Double_t t_NaI[7];
Double_t t_NaIE[7];
Double_t t_NaIEtot;
//Double_t t_Plastic[8];
Double_t t_SiPside1[16];
Double_t t_SiPside2[16];
Double_t t_SiPside3[16];
Double_t t_SiPside4[16];
Double_t t_SiNside1[16];
Double_t t_SiNside2[16];
Double_t t_SiNside3[16];
Double_t t_SiNside4[16];
Double_t t_NaITDC[7];
Double_t t_SiPside1TDC[16];
Double_t t_SiPside2TDC[16];
Double_t t_SiPside3TDC[16];
Double_t t_SiPside4TDC[16];
#endif

#ifdef _STRUCKADC
Double_t t_StruckADC1[8];
#endif

#ifdef _SILICONDATA
SiliconData *si;
#endif

#ifdef _RAWDATA
RawData *raw;
#endif

#ifdef _GAMMADATA
GammaData *gammy;
#endif

#ifdef _GATEAU
GateauData *fatty;
#endif

Int_t t_pulser=0;    // a pattern register equivalent

#ifdef _POLARIZATION  
Int_t t_polu=0, t_pold=0;   // a pattern register equivalent
#endif

#ifdef _MOVIE  // if you want to use the 'movie' script
Double_t t_X1dt[MAX_WIRES_PER_EVENT];                                    
Double_t t_X1dtnohot[MAX_WIRES_PER_EVENT];  
Double_t t_U1dt[MAX_WIRES_PER_EVENT];
Double_t t_X2dt[MAX_WIRES_PER_EVENT];
Double_t t_U2dt[MAX_WIRES_PER_EVENT];
Int_t    t_nX1wires,t_nX2wires,t_nU1wires,t_nU2wires;     //for dynamic size of t_X1dt etc in tree    
Int_t t_X1wire[MAX_WIRES_PER_EVENT];
Int_t t_X1wirenohot[MAX_WIRES_PER_EVENT];
Int_t t_X2wire[MAX_WIRES_PER_EVENT];
Int_t t_U1wire[MAX_WIRES_PER_EVENT];
Int_t t_U2wire[MAX_WIRES_PER_EVENT];
#endif

Double_t X1wirefit[10], X1distfit[10];      
Double_t U1wirefit[10], U1distfit[10];      
Double_t X2wirefit[10], X2distfit[10];      
Double_t U2wirefit[10], U2distfit[10];  


/*--------Histogramming Data Structures ---------------------------------------------*/
TFile *f1=new TFile("output.root","RECREATE");
TTree *t1=new TTree("DATA","K600 data");

static TH1F *hX1_lut, *hX2_lut, *hU1_lut, *hU2_lut;
static TH1F *hCableOff, *hCableOfftmp;

static TH1F *hEventID, *hEventID2;

static TH2F *hPad1VsTofG, *hPad1Pad2, *hPad1Pad2G;

static TH1F *hTDCRawSpectra[NR_OF_TDCS];
static TH2F *hTDC2DModule[NR_OF_TDCS];
static TH1F *hTDCPerEventRaw;
//static TH1F *hTDCPerEvent;

static TH2F *hChanVsTimeRef, *hChanVsTimeOffset, *hChanVsTimeOffsetPID;
static TH2F *hWireVsTimeOffset, *hWireVsTimeOffsetPID;

static TH1F *hHitPatternRawTDC, *hHitPatternAll, *hHitPatternPID;

static TH1F *hX1_EffID, *hU1_EffID, *hX2_EffID, *hU2_EffID;
static TH1F *hX1_DriftLength, *hX2_DriftLength, *hU1_DriftLength, *hU2_DriftLength;
static TH1F *hX1_DriftTimeGood, *hU1_DriftTimeGood, *hX2_DriftTimeGood, *hU2_DriftTimeGood;

#ifdef _FULLANALYSIS
static TH1F *hDriftTimeRawAll, *hDriftTimeOffsetAll;

static TH1F *hX1_DriftTimeOffset;
static TH1F *hX2_DriftTimeOffset;
static TH1F *hU1_DriftTimeOffset;
static TH1F *hU2_DriftTimeOffset;

static TH1F *hX1_DriftTime140;

static TH1F *hX1_DriftTimeRef;
static TH1F *hX2_DriftTimeRef;
static TH1F *hU1_DriftTimeRef;
static TH1F *hU2_DriftTimeRef;

static TH1F *hX1_Hits, *hX2_Hits , *hU1_Hits, *hU2_Hits;
static TH1F *hX1_HitsG, *hX2_HitsG, *hU1_HitsG, *hU2_HitsG;

static TH1F *hX1_Chisq, *hX1_Eff, *hX1_Res;
static TH2F *hX1_Res2dRCNP, *hX1_Res2diTL;
static TH2F *hX1_PosEff;

static TH1F *hU1_Chisq, *hU1_Eff, *hU1_Res;
static TH2F *hU1_Res2dRCNP, *hU1_Res2diTL;

static TH1F *hX2_Chisq, *hX2_Eff, *hX2_Res;
static TH2F *hX2_Res2dRCNP, *hX2_Res2diTL;

static TH1F *hU2_Chisq, *hU2_Eff, *hU2_Res;
static TH2F *hU2_Res2dRCNP, *hU2_Res2diTL;

static TH1F *h_Y1, *h_Y2;
#endif


#ifdef _JJAUTOTRIM
static TH1F *hTDC_REF[TDC_CHANNELS];   // uncomment if you use JJ_autotrim.C
#endif


//----------------------------------------------------------------------------------------//
void GetODBGlobals()
// Get globals from the ODB; not needed if we use Parameters and Module Declaration path
{
    HNDLE hDB, hKey;

   // open ODB structures 
   cm_get_experiment_database(&hDB, NULL);
   db_find_key(hDB, 0, "/Analyzer/Parameters/GLOBAL", &hKey);
   if(db_open_record(hDB, hKey, &globals, sizeof(GLOBAL_PARAM), MODE_READ, NULL, NULL) != DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init", "Cannot open \"/Analyzer/Parameters/GLOBAL\" tree in ODB");
      exit(1);//return 0;
   }
   //printf("ODB test : %d  \n",globals.misswires);
   
   db_close_record(hDB,hKey);
}

//----------------------------------------------------------------------------------------//
void GetODBRunInfo()
// Get globals from the ODB; not needed if we use Parameters and Module Declaration path
{
    HNDLE hDB, hKey;

   // open ODB structures 
   cm_get_experiment_database(&hDB, NULL);
   
   RUNINFO_STR(runinfo_str);            // RUNINFO_STR in experim.h      rn:  not true !?!?!
   db_create_record(hDB, 0, "/Runinfo", strcomb(runinfo_str));
   db_find_key(hDB, 0, "/Runinfo", &hKey);
   if (db_open_record(hDB, hKey, &runinfo2, sizeof(runinfo2), MODE_READ, NULL, NULL) !=
       DB_SUCCESS) {
      cm_msg(MERROR, "analyzer_init", "Cannot open \"/Runinfo\" tree in ODB");
      exit(1);
   }
   printf("\n==================================================================== %i  \n\n",runinfo2.run_number);
   
   db_close_record(hDB,hKey);
}









//----------------------------------------------------------------------------------------//
void PrintODBstuff()
// test mode to see if I read the right stuff
{
  printf("globals.x1_1st_wire_chan: %d  \n",globals.x1_1st_wire_chan);
  printf("globals.x2_1st_wire_chan: %d  \n",globals.x2_1st_wire_chan);  
  printf("globals.u1_1st_wire_chan: %d  \n",globals.u1_1st_wire_chan);  
  printf("globals.u2_1st_wire_chan: %d  \n",globals.u2_1st_wire_chan);

  printf("gates.lowpad1: %d  \n",gates.lowpad1);
  printf("gates.hipad1: %d  \n",gates.hipad1);
  printf("gates.lowpad2: %d  \n",gates.lowpad2);
  printf("gates.hipad2: %d  \n",gates.hipad2);
}

//------------------------------------------------------------------------------
void printevent(Int_t hits, Int_t wire[], Int_t time[])
{
   for(int i = 0; i < hits ; i++) { 
     printf("Sorted: wire %d  time %d",wire[i],time[i]);
     //printf("  time %d",time[i]);
     printf("\n");
   }
}

//------------------------------------------------------------------------------
void printevent2(Int_t hits, Int_t wire[], Double_t dist[])
{
   for(int i = 0; i < hits ; i++) { 
     printf("wire dist: %d %f",wire[i],dist[i]);
     //printf("  dist %f",dist[i]);
     printf("\n");
   }
}

/* ---------------------------------------------------------------------------------------*/
void printv1190data(DWORD *data, int tdcmodulecounter, int i)
// keep for debugging purposes
{
Int_t  code;
Int_t  edge,time,channel;//,wire;
char sbuffer[128]; //string for possible storage of info for output
code = 0x1F&((*data)>>27);

switch (code) {
	case 0: // valid measurent word 
		edge = 0x1&((*data)>>26);
		channel = 0x7F&((*data)>>19);
		time = 0x7FFFF&(*data);
		printf("module : %d tdc %3d: 0x%08x, code 0x%02x, edge %d, chan %2d, time %6d\n", 
			tdcmodulecounter, i, (*data), code, edge, channel, time);
		break; // end of case 0
	case 0x01: // TDC Header
		printf("TDC Header: 0x%08x, tdc: %d eventid: %d bunch id: 0%d\n",
				(*data), (*data)&0xf,((*data)>>12)&0xfff, ((*data)&0xfff));
		break;
	case 0x03: // TDC Trailer
		printf("TDC Trailer: 0x%08x, tdc: %d, event id: %d word count: %d\n",
				(*data), ((*data)>>24)&0x3,((*data)>>12)&0x3ffff,(*data));
		break;
	case 0x04: // TDC Error
		printf("TDC module (%d) Error: 0x%08x, tdc: %d error flag 0x%02x, \n", 
				tdcmodulecounter, (*data), ((*data)>>24)&0x3, ((*data)&0x7fff));
		break;
	case 0x08: // global header
		printf("Global Header: 0x%08x, GEO 0x%02x, event count %d\n",
				(*data), (*data)&0x1f,((*data)>>5)&0x3ffff);
		break;
	case 0x10: // Buffer Trailer
		if(((*data)>>24)&0x1) sprintf(sbuffer,"trigger lost");
		if(((*data)>>24)&0x2) sprintf(sbuffer,"buffer overflow");
		if(((*data)>>24)&0x4) sprintf(sbuffer,"TDC Error");
		printf("Buffer Trailer: 0x%08x, status: %s, word count: %x, geo: %d\n",
			(*data),sbuffer, ((*data)>>5)&0xffff,(*data)&0x1f);
		break;
	case 0x11: // extended time trigger
		printf("TDC Extended time trigger: 0x%08x, time tag: %l\n", (*data),(*data)&0x7ffffff);
		break;
	case 0x18: // filler word
		printf("TDC Filler: 0x%08x\n",(*data));
		break;
	default:
	   	printf("TDC Unsupported code %3d: 0x%08x, code 0x%02x\n",i,(*data),code);
		break;
	}// switch statement
}

/*---------------------------------------------------------------*/
void read_misalignment(int *_matime, char *fileName) 
{
   FILE *file = fopen(fileName, "r");
   char string_var[200] = "";
   int  int_var = 0;

   fscanf(file,"%s", string_var);
   int_var = atoi(string_var);
   *_matime = int_var;
   //printf("----------------Read routine: misalignment time cutoff (sec): %i  -----------\n",*_matime );                
   fclose(file);   
}

/*---------------------------------------------------------------*/
void read_lut(float lutx[], int offset, char *fileName) 
{
   FILE *file = fopen(fileName, "r");
   //assert(file != NULL);
   short nrows = LUT_CHANNELS;   
   char string_var[200] = "";
   float float_var = 0;

   for(int i = 0; i < nrows; i ++) {
      if (feof(file)){
         break;           //just in case
      }
      fscanf(file,"%s", string_var);
      float_var = strtof(string_var, NULL);
      //printf("float_var : %f  \n",float_var);
      if((i-offset)>0) lutx[i-offset] = float_var;        
   }
   fclose(file);   
}

/*---------------------------------------------------------------*/
void read_cable(float cable[], char* fileName) 
{
   FILE *file = fopen(fileName, "r");
   //assert(file != NULL);
   short nrows = 1000; //akin to number of tdc channels, but not physical chan. 
   char string_var[200] = "";
   float float_var = 0;

   for(int i = 0; i < nrows; i ++) {
      if (feof(file)){
        break; //just in case
      }	
      fscanf(file,"%s", string_var);
      float_var = strtof(string_var, NULL);
      //printf("float_Var : %f  \n",float_var);
      cable[i] = float_var;
      //printf("float # %f \n",cable[i]);
   }
   fclose(file);   
}

/* ---------------------------------------------------------------------------------------*/
void setupchannel2wireXoldXold()
// Mapping of wires to channels when using 2 old VDCs
// See camac-vme-cabling.xls
// This is fairly complicated due to design mistakes on the PCB's
// In this mapping 
// chan 0-207   = X wires VDC1
// chan 500-707 = X wires VDC2
//
{
   printf("setupchannel2wireXoldXold()\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  int tdcchan;

  for(input=0;input<896;input++) Channel2Wire[input]=-1;

  for(tdcmodulecounter=0;tdcmodulecounter<8;tdcmodulecounter++){
      for(input=1;input<8;input++){
	   channelstart=0;
	   preampnum=(tdcmodulecounter*7)+input;
	   //printf("tdc %d   input %d   preamp %d  channel %d\n",tdcmodulecounter,input,preampnum,channel);

	   if(preampcount<13) { // wireplane X1  =================================================
	      	basecount=8;
	        preampbase=1;
		channelstart=basecount + (preampcount-preampbase)*16;
		if(preampcount==0){
		  Channel2Wire[24]=0;
		  Channel2Wire[25]=1;
		  Channel2Wire[26]=2;
		  Channel2Wire[27]=3;
		  Channel2Wire[28]=4;
		  Channel2Wire[29]=5;
		  Channel2Wire[30]=6;
		  Channel2Wire[31]=7;			
		  for(int i=24;i<32;i++){
		    tdcchan=i;
                    //printf("chan2wire[%d]  = %d  \n",tdcchan,Channel2Wire[tdcchan]);
                    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, Channel2Wire[tdcchan]);
                  }
		}
		else{
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
		    Channel2Wire[tdcchan]=i;
                    //printf("chan2wire[%d] = %d   \n",tdcchan, Channel2Wire[tdcchan]);
		  }
		}
	   }
	   else if(preampcount>21 && preampcount <35){ // wireplane X2  =================================================
	       	basecount=508;
	       	preampbase=23;
		channelstart=basecount + (preampcount-preampbase)*16;
		if(preampcount==22){
		  Channel2Wire[424]=500;
		  Channel2Wire[425]=501;
		  Channel2Wire[426]=502;
		  Channel2Wire[427]=503;
		  Channel2Wire[428]=504;
		  Channel2Wire[429]=505;
		  Channel2Wire[430]=506;
		  Channel2Wire[431]=507;			
		  for(int i=424;i<432;i++){
		    tdcchan=i;
//                     printf("chan2wire[%d]  = %d  \n",tdcchan,Channel2Wire[tdcchan]);
                    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, Channel2Wire[tdcchan]);
                  }
		}
		else{
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
		    Channel2Wire[tdcchan]=i;
//                     printf("chan2wire[%d] = %d   \n",tdcchan, Channel2Wire[tdcchan]);
		  }
		}
	   }
	   preampcount++;	
      }
  }
}

/* ---------------------------------------------------------------------------------------*/
void setupchannel2wireXoldXU()
// Mapping of wires to channels when using 1 old VDC and 1 new VDC (placed with first wireplane being the X-wires)
// See camac-vme-cabling.xls
{
  printf("setupchannel2wireXoldXU()\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  int tdcchan;

  for(input=0;input<896;input++) Channel2Wire[input]=-1;

  for(tdcmodulecounter=0;tdcmodulecounter<8;tdcmodulecounter++){
      for(input=1;input<8;input++){   //input is TDC input, there are 8, 0-7, and input 0 is used for trigger and other things
	   channelstart=0;
	   preampnum=(tdcmodulecounter*7)+input;
	   //printf("tdc %d   input %d   preamp %d  channel %d\n",tdcmodulecounter,input,preampnum,channel);

	   if(preampcount<13) { // wireplane X1  =================================================
	      	basecount=8;
	        preampbase=1;
		channelstart=basecount + (preampcount-preampbase)*16;
		if(preampcount==0){
		  Channel2Wire[24]=0;
		  Channel2Wire[25]=1;
		  Channel2Wire[26]=2;
		  Channel2Wire[27]=3;
		  Channel2Wire[28]=4;
		  Channel2Wire[29]=5;
		  Channel2Wire[30]=6;
		  Channel2Wire[31]=7;			
		  for(int i=24;i<32;i++){
		    tdcchan=i;
                    //printf("chan2wire[%d]  = %d  \n",tdcchan,Channel2Wire[tdcchan]);
                    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, Channel2Wire[tdcchan]);
                  }
		}
		else{
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
		    //tdcchan=(tdcmodulecounter*128) +  ((preampcount+1)-7*tdcmodulecounter)*16 +(i-channelstart);
		    Channel2Wire[tdcchan]=i;
                    //printf("channelstart %d;  tdcmodule %d ;  preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, tdcmodulecounter, preampcount, tdcchan, Channel2Wire[tdcchan]);
		  }
		}
	   }
	   else if(preampcount>12 && preampcount <22){ // wireplane U1  =================================================
	       	basecount=300;
	       	preampbase=13;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  Channel2Wire[tdcchan]=i;
		  //printf("chan2wire[%d] = %d  \n",tdcchan, Channel2Wire[tdcchan]);
		}
	   }
	   else if(preampcount>21 && preampcount <35){ // wireplane X2  =================================================
	       	basecount=500;
	       	preampbase=22;
		channelstart=basecount+(preampcount-preampbase)*16;

		if(preampcount==(35-1)){		    
		  Channel2Wire[624]=697;
		  Channel2Wire[625]=696;
		  Channel2Wire[626]=695;
		  Channel2Wire[627]=694;
		  Channel2Wire[628]=693;
		  Channel2Wire[629]=692;
		  //for(int i=624;i<640;i++){
		  //    printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[i],i);
		  //}
		}
		else {
		  int counter=1;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    Channel2Wire[tdcchan]=(channelstart+16) - counter;
		    counter++;
  		    //printf("chan2wire[%d] = %d  \n",tdcchan, Channel2Wire[tdcchan]);
		  }
		} 
	   }
	   else if(preampcount>34 && preampcount <44){// wireplane U2  =================================================
	       	basecount=800;
	       	preampbase=35;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  Channel2Wire[tdcchan]=i;
		  //printf("chan2wire[%d] = %d  \n",tdcchan, Channel2Wire[tdcchan]);
		}
	   }
	   preampcount++;	
      }
  }

  //for(int i=0;i<300;i++){
  //     printf("--------------------- chan2wire[%d] = %d   \n",i, Channel2Wire[i]);
  //}



}

/* ---------------------------------------------------------------------------------------*/
void setupchannel2wireXUXU()
// hack the mapping of wires to channels when new VDC is placed with first wireplane being the X-wires
// See camac-vme-cabling.xls
// This is due to design mistakes on the PCB's
//
{
   printf("setupchannel2wireXUXU()\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  //int channel=0;
  int tdcchan;

  for(input=0;input<896;input++) Channel2Wire[input]=-1;

  for(tdcmodulecounter=0;tdcmodulecounter<8;tdcmodulecounter++){
      for(input=1;input<8;input++){
	   channelstart=0;
	   preampnum=(tdcmodulecounter*7)+input;
	   //printf("tdc %d   input %d   preamp %d \n",tdcmodulecounter,input,preampnum);

	   if(preampcount<13) { // wireplane X1  =================================================
	      	basecount=0;
	       	preampbase=0;
		channelstart=basecount+(preampcount-preampbase)*16;

		if(preampcount==(13-1)){		    
		  Channel2Wire[224]=197;
		  Channel2Wire[225]=196;
		  Channel2Wire[226]=195;
		  Channel2Wire[227]=194;
		  Channel2Wire[228]=193;
		  Channel2Wire[229]=192;
		  for(int i=230;i<240;i++){
		    Channel2Wire[i]=0;
		  }
		  //for(int i=224;i<240;i++){
		  //    printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[i],i);
		  //}
		}
		else {
		  int counter=1;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    Channel2Wire[tdcchan]=(channelstart+16) - counter;
		    counter++;
		    //printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[tdcchan],tdcchan);
		  }
		} 
	   }
	   else if(preampcount<22){ // wireplane U1  =================================================
	       	basecount=300;
	       	preampbase=13;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  Channel2Wire[tdcchan]=i;
		  //printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[tdcchan],tdcchan);
		}
	   }
	   else if(preampcount<35){ // wireplane X2  =================================================
	       	basecount=500;
	       	preampbase=22;
		channelstart=basecount+(preampcount-preampbase)*16;

		if(preampcount==(35-1)){		    
		  Channel2Wire[624]=697;
		  Channel2Wire[625]=696;
		  Channel2Wire[626]=695;
		  Channel2Wire[627]=694;
		  Channel2Wire[628]=693;
		  Channel2Wire[629]=692;
		  for(int i=230;i<240;i++){
		    Channel2Wire[i]=0;
		  }
		  //for(int i=624;i<640;i++){
		  //    printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[i],i);
		  //}
		}
		else {
		  int counter=1;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    Channel2Wire[tdcchan]=(channelstart+16) - counter;
		    counter++;
		    //printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[tdcchan],tdcchan);
		  }
		} 
	   }
	   else if(preampcount<44){// wireplane U2  =================================================
	       	basecount=800;
	       	preampbase=35;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  Channel2Wire[tdcchan]=i;
		  //printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[tdcchan],tdcchan);
		}
	   }
	   preampcount++;	
      }
  }
}

void setupchannel2wireXUXold()
// hack the mapping of wires to channels with XU and then old X chamber
//
{
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  //int channel=0;
  int tdcchan;

  for(input=0;input<896;input++) Channel2Wire[input]=-1;

  for(tdcmodulecounter=0;tdcmodulecounter<8;tdcmodulecounter++){
    for(input=1;input<8;input++){
      channelstart=0;
      preampnum=(tdcmodulecounter*7)+input;
      //printf("tdc %d input %d preamp %d \n",tdcmodulecounter,input,preampnum);

      if(preampcount<13) { // wireplane X1  =================================================
        basecount=0;
        preampbase=0;
	channelstart=basecount+(preampcount-preampbase)*16;

	if(preampcount==(13-1)){            
	  Channel2Wire[224]=197;
	  Channel2Wire[225]=196;
	  Channel2Wire[226]=195;
	  Channel2Wire[227]=194;
	  Channel2Wire[228]=193;
	  Channel2Wire[229]=192;
	  for(int i=230;i<240;i++){
	    Channel2Wire[i]=0;
	  }
	  //for(int i=224;i<240;i++){
	  //    printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[i],i);
	  //}
	}
	else {
	  int counter=1;
	  for(int i=channelstart;i<channelstart+16;i++){
	    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
	    Channel2Wire[tdcchan]=(channelstart+16) - counter;
	    counter++;
	    //printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[tdcchan],tdcchan);
	  }
	} 
      }
      else if(preampcount<22){ // wireplane U1  =================================================
        basecount=300;
        preampbase=13;
	channelstart=basecount+(preampcount-preampbase)*16;
	for(int i=channelstart;i<channelstart+16;i++){
	  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
	  Channel2Wire[tdcchan]=i;
	  //printf("chan2wire %d   tdcchan= %d  \n",Channel2Wire[tdcchan],tdcchan);
	}
      }
      else if(preampcount>21 && preampcount <35){ // wireplane X2  =================================================
        basecount=508;
        preampbase=23;
	channelstart=basecount + (preampcount-preampbase)*16;
	if(preampcount==22){
	  Channel2Wire[424]=500;
	  Channel2Wire[425]=501;
	  Channel2Wire[426]=502;
	  Channel2Wire[427]=503;
	  Channel2Wire[428]=504;
	  Channel2Wire[429]=505;
	  Channel2Wire[430]=506;
	  Channel2Wire[431]=507;            
	  for(int i=424;i<432;i++){
	    tdcchan=i;
	    //printf("chan2wire[%d]  = %d  \n",tdcchan,Channel2Wire[tdcchan]);
	    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, Channel2Wire[tdcchan]);
	  }
	}
	else{
	  for(int i=channelstart;i<channelstart+16;i++){
	    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
	    Channel2Wire[tdcchan]=i;
	    //printf("chan2wire[%d] = %d   \n",tdcchan, Channel2Wire[tdcchan]);
	  }
	}
      }
      preampcount++;   
    }
  }
}


/* ---------------------------------------------------------------------------------------*/
void setupchannel2wire()
// hack the mapping of wires to channels for UXUX setup
// See camac-vme-cabling.xls
// This is fairly complicated due to design mistakes on the PCB's
//
// chan 0-207   = X wires VDC1
// chan 300-443 = U wires VDC1
// chan 500-707 = X wires VDC2
// chan 800-943 = U wires VDC2

{
   printf("setupchannel2wire() - UXUX\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  //int channel=0;
  int tdcchan;

  for(input=0;input<896;input++) Channel2Wire[input]=-1;

  for(tdcmodulecounter=0;tdcmodulecounter<8;tdcmodulecounter++){
      for(input=1;input<8;input++){
	   channelstart=0;
	   preampnum=(tdcmodulecounter*7)+input;
	   //printf("tdc %d   input %d   preamp %d  channel %d\n",tdcmodulecounter,input,preampnum,channel);
	   if(preampcount<13) { // wireplane X1  =================================================
	      	basecount=0;
	       	preampbase=0;
		channelstart=basecount+(preampcount-preampbase)*16;
		if(preampcount==0){		    
		  Channel2Wire[16]=10;
		  Channel2Wire[17]=11;
		  Channel2Wire[18]=12;
		  Channel2Wire[19]=13;
		  Channel2Wire[20]=14;
		  Channel2Wire[21]=15;
		  for(int i=22;i<31;i++){
		    Channel2Wire[i]=0;
		  }
		  channelstart=basecount+(preampcount-preampbase)*16;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  }
		}
		else {
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    Channel2Wire[tdcchan]=i;
 //printf("tdcchan %d   i %d  \n",Channel2Wire[tdcchan],i);
		  }
		} 
	   }
	   else if(preampcount<22){ // wireplane U1  =================================================
	       	basecount=300;
	       	preampbase=13;
		channelstart=basecount+(preampcount-preampbase)*16;
		int counter=1;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  Channel2Wire[tdcchan]=(channelstart+16) - counter;
		  counter++;
		}
	   }
	   else if(preampcount<35){ // wireplane X2  =================================================
	       	basecount=500;
	       	preampbase=22;
		channelstart=basecount+(preampcount-preampbase)*16;
		if(preampcount==22){		    
		  Channel2Wire[416]=510;
		  Channel2Wire[417]=511;
		  Channel2Wire[418]=512;
		  Channel2Wire[419]=513;
		  Channel2Wire[420]=514;
		  Channel2Wire[421]=515;
		  for(int i=422;i<431;i++){
		    Channel2Wire[i]=0;
		  }
		  channelstart=basecount+(preampcount-preampbase)*16;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    //printf("tdc %d input %d channel %d wire %d\n",tdcmodulecounter,input,tdcchan,Channel2Wire[tdcchan]);
		  }
		}
		else {
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    Channel2Wire[tdcchan]=i;
		    //printf("tdc %d input %d channel %d wire %d\n",tdcmodulecounter,input,tdcchan,Channel2Wire[tdcchan]);
		  }
		}	   }
	   else if(preampcount<44){// wireplane U2  =================================================
	       	basecount=800;
	       	preampbase=35;
		channelstart=basecount+(preampcount-preampbase)*16;
		int counter=1;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  Channel2Wire[tdcchan]=(channelstart+16) - counter;
		  counter++;
		}
	   }
	   preampcount++;	
      }
  }
}

//---------------------------------------------------------------------------------------------//
void getRefTimes(int time[], int ntdc, DWORD ptdc[])
//void getRefTimes(int time[], int ntdc, DWORD ptdc[],int doubletdc[],int doublecnt[])
// 1st chan into each TDC is a copy of the trigger. Find this for each module for each event.
// Without this you cannot get accurate time determination.
{
  extern int TDCModules;
   memset(time,0,TDCModules*sizeof(int));   
   int module=0,channel=9999;   
   for(int i=0;i<ntdc;i++){               // loop through all the TDC datawords
      if((((ptdc[i])>>27)&0x1f)==0x1f){     // first determine TDC module nr. This precedes data for each module.
	module=ptdc[i]&0xf;
	continue;              // go back to beginning of for loop as data is not true.
      }
      if((0x1F&((ptdc[i])>>27))==0){     // if a valid data word with bits 27-31 = 0
	channel = 0x7F&((ptdc[i])>>19);
        if(channel==0){                  // looking only at 1st channel of each TDC
	  if(time[module]==0){	         // do this to ensure there is only 1 ref time per event
		time[module]=(ptdc[i]& 0x7FFFF);      //any TDC, ch 0
	  } 
	  else{	    
	    printf("                                                        ERROR: double ref times: module= %d, ntdc= %d, channel= %d\n",module,ntdc,channel);//// error condition
	  }
	}
      }
   }
}

//------------------------------------------------------------------------------
void ZeroFPWireTimeDist(void)
{
   for(int i =0; i < MAX_WIRES_PER_EVENT; i++) {	
      X1.wire[i] = 0;
      X1.time[i] = 0;
      X1.dist[i] = 0.0;
      X2.wire[i] = 0;
      X2.time[i] = 0;
      X2.dist[i] = 0.0;
      U1.wire[i] = 0;
      U1.time[i] = 0;
      U1.dist[i] = 0.0;
      U2.wire[i] = 0;
      U2.time[i] = 0;
      U2.dist[i] = 0.0;
   }   
}

//------------------------------------------------------------------------------
void ZeroTTreeVariables(void)     // Really more an initialization as a zero-ing
{
   t_tdcsperevent=0;
   t_pad1hiP=-1;  t_pad1lowP=-1;  t_pad2hiP=-1;  t_pad2lowP=-1;
   t_pad1hiPT=-1; t_pad1lowPT=-1; t_pad2hiPT=-1; t_pad2lowPT=-1;
   t_tof=0; t_toftdc2=0; t_toftdc3=0; t_toftdc4=0; t_toftdc5=0; t_toftdc6=0; t_toftdc7=0;
   t_k600=0; t_runtime=-1;
   t_X1hits = -100; t_X2hits = -100; t_U1hits = -100; t_U2hits = -100;
   t_X1pos=-100.; t_X2pos=-100.; t_U1pos=-100.; t_U2pos=-100.;
   t_X1th=-100.;  t_X2th=-100.;  t_U1th=-100.;  t_U2th=-100.;
   t_X1posC=-100.;
   t_Ex=-1.;
   t_X1chisq=-100.; t_X2chisq=-100.; t_U1chisq=-100.; t_U2chisq=-100.;
   t_X1flag=-100;   t_X2flag=-100;   t_U1flag=-100;   t_U2flag=-100;
   //t_X1effID=-100.; t_X2effID=-100.; t_U1effID=-100.; t_U2effID=-100.;   
   t_X1res0=-100.0; t_X2res0=-100.0; t_U1res0=-100.0; t_U2res0=-100.0;
   t_X1res1=-100.0; t_X2res1=-100.0; t_U1res1=-100.0; t_U2res1=-100.0;   
   t_X1chisqminimization=0;t_X2chisqminimization=0; t_U1chisqminimization=0; t_U2chisqminimization=0;
  #ifdef _VDCRESCALCS
   t_X1res2=-100.0; t_X2res2=-100.0; t_U1res2=-100.0; t_U2res2=-100.0;
   t_X1res3=-100.0; t_X2res3=-100.0; t_U1res3=-100.0; t_U2res3=-100.0;
   t_X1res4=-100.0; t_X2res4=-100.0; t_U1res4=-100.0; t_U2res4=-100.0;
   t_X1res5=-100.0; t_X2res5=-100.0; t_U1res5=-100.0; t_U2res5=-100.0;
   t_X1res6=-100.0; t_X2res6=-100.0; t_U1res6=-100.0; t_U2res6=-100.0;
   t_X1res7=-100.0; t_X2res7=-100.0; t_U1res7=-100.0; t_U2res7=-100.0;
   t_X1res8=-100.0; t_X2res8=-100.0; t_U1res8=-100.0; t_U2res8=-100.0;
   #endif
   t_ThFP=-100; t_ThFPx=-100; 
   t_ThSCAT=-100; t_PhiSCAT=-100;t_PhiFP=-100;
   t_Y1=-100.0; t_Y2=-100.0;

   t_X1effall=-1; t_X1effdt=-1; t_X1effgroup=-1; t_X1effgood=-1;
   t_U1effall=-1; t_U1effdt=-1; t_U1effgroup=-1; t_U1effgood=-1;
   t_X2effall=-1; t_X2effdt=-1; t_X2effgroup=-1; t_X2effgood=-1;
   t_U2effall=-1; t_U2effdt=-1; t_U2effgroup=-1; t_U2effgood=-1;

   #ifdef _FULLANALYSIS
   t_X1TimeDiff=-100;
   t_nX1wiresUsed=0;  t_nU1wiresUsed=0;  t_nX2wiresUsed=0; t_nU2wiresUsed=0;
   t_X1doublewires=-1;  t_U1doublewires=-1;   t_X2doublewires=-1;  t_U2doublewires=-1;
   t_X1multiplemin=-1;  t_U1multiplemin=-1; t_X2multiplemin=-1;  t_U2multiplemin=-1;
   for(int i =0; i < MAX_WIRES_PER_EVENT; i++) {	   
	t_X1wireUsed[i]=-1; t_U1wireUsed[i]=-1; t_X2wireUsed[i]=-1;t_U2wireUsed[i]=-1;
	t_X1distUsed[i]=-1.; 
   }
   #endif

   t_pulser=0;

   #ifdef _ADC
   for(int i = 0; i < 7 ; i++) { 
      t_NaI[i]=0.; t_NaIE[i]=0.; //t_Plastic[i]=0.;
   }
   for(int i = 0; i < 16 ; i++) { 
      t_SiPside1[i]=-1.; t_SiPside2[i]=-1.; t_SiPside3[i]=-1.; t_SiPside4[i]=-1.;
   }
   for(int i = 0; i < 16 ; i++) { 
      t_SiNside1[i]=-1.; t_SiNside2[i]=-1.; t_SiNside3[i]=-1.; t_SiNside4[i]=-1.; 
   }
   t_NaIEtot=0.;
   #endif

   #ifdef _MOVIE 
   for(int i =0; i < MAX_WIRES_PER_EVENT; i++) {	
	   t_X1dt[i]=-1.0; t_U1dt[i]=-1.0; t_X2dt[i]=-1.0; t_U2dt[i]=-1.0; //without this the X1dt will have junk in above and beyond the good data
	   t_X1wire[i]=-1; t_U1wire[i]=-1;  t_X2wire[i]=-1;  t_U2wire[i]=-1;  
   }
   #endif

   #ifdef _POLARIZATION
   t_polu=0; t_pold=0;
   #endif

}


//------------------------------------------------------------------------------
void sortTDCs(Int_t hits, Int_t wire[], Int_t time[])      //Bubble sort so that order of e.g. X1.wire[] is correct
{
   for(int i = 0; i < hits ; i++) { 
      for(int j = hits-1; j > i ; j--) { 
	 int tempwire;
	 int temptime;
	 if(wire[j-1] > wire[j]){
	    tempwire=wire[j-1];
  	    wire[j-1]=wire[j];
	    wire[j]=tempwire;
	    //
	    temptime=time[j-1];
  	    time[j-1]=time[j];
	    time[j]=temptime;
	 }	      
      }
   }
}

//------------------------------------------------------------------------------
void getRidOfHotWire(Int_t *_hits,Int_t *_hits2, Int_t wire[], Int_t time[])
{

   int hotwire[20];
   int hottime[20];
   int hotwirecandidate[20];
   int badindex[20];
   for(int i = 0; i < 20 ; i++) { 
	hotwire[i]=0;
	hottime[i]=0;
	hotwirecandidate[i]=0;
	badindex[i]=0;
   }
   int hotcount=0;
   int hotcount2=0;
   int timelimit = 5000;
   int previoushot=-1;
   int wirenum=0;
   int timetmp[*_hits];
   int wiretmp[*_hits];
   int hotindex=0;
   int flagbad=0;

   for(int i = 0; i < *_hits ; i++) { 
	timetmp[i]=time[i];
	wiretmp[i]=wire[i];
   }

   // first determine nr of hot wires. They are defined as wires that have
   // multiple hits and at least one of these are well outside of good
   // drifttime region i.e. below 5000
   // The fact that the data is already bubblesorted helps a lot.
   for(int i = 1; i < *_hits ; i++) { 
      if(wire[i-1]==wire[i] && (time[i]<timelimit || time[i-1]<timelimit ) ){
	 hotwirecandidate[hotcount]=wire[i];
         //printf("hotwirecandidate %i \n",hotwirecandidate[hotcount]);
	 hotcount++;
      }
   }
   //printf("--------------hotcount %i \n\n",hotcount);

   // now you have a list of wires that appear at least twice.
   // Your list may have a signel wire showing a couple of times.
   // I just want a list of bad wires, i.e. one value per bad wire
   for(int i = 1; i < hotcount ; i++) { 
      if(hotwirecandidate[i-1] != hotwirecandidate[i]){
         hotwire[hotcount2]=hotwirecandidate[i-1];
         hotcount2++;
      }
   }
   if(hotwirecandidate[hotcount-1] != hotwire[hotcount2-1]){
      hotwire[hotcount2]=hotwirecandidate[hotcount-1];
      hotcount2++;
   } 
   //printf("-----------------------------------hotcount2 %i \n",hotcount2);
   //for(int i = 0; i < hotcount2 ; i++) { 
   //	printf("hotwire %i \n",hotwire[i]); 
   //}

   // now I know which are the hot wires. Fill dist and time arrays without them
   for(int i = 0; i < *_hits ; i++) { 
	   for(int h = 0; h < hotcount2 ; h++) { 
		if(wire[i]==hotwire[h]){
		    flagbad=1; 
		    //printf("wire %i \n",wire[i]); 
                }
           }
           if(flagbad==0) {
	        //printf("good wire:  %i \n",wiretmp[i]);
		wire[wirenum]=wiretmp[i];			
	 	time[wirenum]=timetmp[i];
                wirenum++;
	   }
	   flagbad=0;
   }
   //for(int i = 0; i < wirenum ; i++) { 
   //	printf("final: wire %i  time %i \n",wire[i],time[i]); 
   //}

   *_hits2=wirenum;
}


//------------------------------------------------------------------------------
void fixDoubleHit(Double_t dd[],Int_t wire[], Int_t badwire, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_last){
// USED IN RAYTRACE
   // Originally I, right away, got rid of the wires where a wire gives 2 drifttimes: throw them out...
   // In Nov 2011 I modified to look at which of the 2 wires that shows a multihit
   // has the biggest driftdistance (usually close to the 8mm) and simply takes that one out.
   // Note: if we have 2 sets of double wires, we deal with the second one first as badwire will be
   // pointing to the second incidence of the doublehit.
   // NOTE: this procedure cannot handle the scenario where 3 or more wirenumbers are the same. 
   // BUT These events can hopefully be dealt with the routine getRidOfHotWire

   Double_t tmpdd[20];
   Int_t tmpwire[20];
   Int_t i;
   Int_t wireIDmin=0;
   Double_t baddist = 7.9;

   //printf("BEFORE=======\n");
   //printevent2(*_wire_num, wire,dd);

   //=========for when 2nd tdc time is such that dd is almost 8mm: problem usually in 2nd instance of wire, except at start and end of matrix

   if ( (dd[badwire+1]>baddist) && ((badwire+2)==*_wire_num) ){   // double hit at end of matrix. 2nd of double is big dist
      for(i=badwire+1;i<*_wire_num;i++){                           // Assume first hit is small distance; assume big distance is best
          tmpdd[i]=dd[i];
          tmpwire[i]=wire[i];
          dd[i-1]=tmpdd[i];
          wire[i-1]=tmpwire[i];
      }
      *_wire_num=*_wire_num-1;                                    // (badwire+2)==wire_num since we count from 0, and to go to badwire+1
      //wireID_last  = *_wire_num-1;       
   }	     
   else if ( (dd[badwire+1]>baddist) && ((badwire)==0) ){   // double hit at start of matrix. 2nd of double is big dist
      for(i=badwire+1;i<*_wire_num;i++){                     // Assume first hit is small distance; assume big distance is best
          tmpdd[i]=dd[i];
          tmpwire[i]=wire[i];
          dd[i-1]=tmpdd[i];
          wire[i-1]=tmpwire[i];
      }
      *_wire_num=*_wire_num-1;   	
      //wireID_last  = *_wire_num-1;       
   }	     
   else if ((dd[badwire+1]>baddist) && ((badwire)!=0)  && ((badwire+2)!=*_wire_num))   {  // problem in middle of matrix.
      for(i=badwire+2;i<*_wire_num;i++){              				     // assume the big distance is wrong
          tmpdd[i]=dd[i];								     // big distance is 2nd hit
          tmpwire[i]=wire[i];
          dd[i-1]=tmpdd[i];
          wire[i-1]=tmpwire[i];
      }
      *_wire_num=*_wire_num-1;
      //wireID_last  = *_wire_num-1;       
   }	     

   //==================following "else if" statements are for scenario when bigger of 2 tdc times is less than baddist

   else if ( (dd[badwire+1]<baddist) && ((badwire+2)==*_wire_num) ){  // double hit at end of matrix. 2nd of double is big dist
      dd[badwire]=dd[badwire+1];				       // since i assume it will be the smaller of the 2 numbers
      wire[badwire]=wire[badwire+1];
      *_wire_num=*_wire_num-1;
      //wireID_last  = *_wire_num-1;       
   }	    
   else if ( (dd[badwire+1]<baddist) && ((badwire)==0) ){   // double hit at start of matrix. 2nd of double is big dist
      for(i=badwire+1;i<*_wire_num;i++){                       // Assume first hit is small distance; assume big distance is best
          tmpdd[i]=dd[i];
          tmpwire[i]=wire[i];
          dd[i-1]=tmpdd[i];
          wire[i-1]=tmpwire[i];
      }
      *_wire_num=*_wire_num-1;   	
      //wireID_last  = *_wire_num-1;       
   }	     
   else if ( (dd[badwire+1]<baddist) && ((badwire)!=0) && ((badwire)==*_wireID_min)  ) {   // here we are around min in drift distance
      for(i=badwire+2;i<*_wire_num;i++){              					    // so the long dist must be wrong
          tmpdd[i]=dd[i];
          tmpwire[i]=wire[i];
          dd[i-1]=tmpdd[i];
          wire[i-1]=tmpwire[i];
      }
      *_wire_num=*_wire_num-1;
      //wireID_last  = *_wire_num-1;       
   }	     
   else if ((dd[badwire+1]<baddist) && ((badwire)!=0) && ((badwire+2)!=*_wire_num))   {  // problem in middle of matrix.
      //printf("========Double events per wire: badwire=%i : baddist(i+1)=%f : *_wire_num=%i \n",badwire,dd[badwire+1],*_wire_num);
      //printevent2(*_wire_num, wire,dd);
      for(i=badwire+2;i<*_wire_num;i++){              				    // assume the big distance is wrong
          tmpdd[i]=dd[i];								    // big distance is 2nd hit
          tmpwire[i]=wire[i];
          dd[i-1]=tmpdd[i];
          wire[i-1]=tmpwire[i];
      }
      *_wire_num=*_wire_num-1;
      //wireID_last  = *_wire_num-1;       

      //printf("AFTER=======\n");
      //printevent2(*_wire_num, wire,dd);
   }

   *_wireID_last  = *_wire_num-1;       
   for(i=1;i<*_wire_num;i++){	                   // Label wire associated with minimum drift distance
     if(dd[wireIDmin] > dd[i]) wireIDmin = i;    // Note: wireID_min an array index nr, NOT a real wire number
   }
   *_wireID_min=wireIDmin;


}






//------------------------------------------------------------------------------
void fixZ(Double_t dd[],Int_t wire[], Int_t *_flag1, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_last){
// USED IN RAYTRACE
// Assume instead of a V we have something approaching a Z
// The leg of the Z on the opposite side of the distance min should be cut off
//
//  Shown below is for different wire index the drift distance (high value on right, low on left
//  A proper event is a V with min at low distance. If the event resembles a Z there is only
//  one low distance minimum, around which you assume the good event is centered.
//
//   6        x
//   5     x              In this example wires 0 and 1 are bad
//   4  x
//   3     x 
//   2       x
//   1     x		<-
//   0   x		<-
//
//
//   7   x  		<-     In this example the wires 6 and 7 are bad
//   6     x		<-
//   5        x 
//   4      x
//   3    x
//   2  x
//   1     x
//   0        x
//
//
   Double_t tmpdd[20];
   Int_t tmpwire[20];
   Int_t i;
   Int_t badwire2=1000;
   Int_t maxddwire=1000;
   Int_t propermin=1000;
   Int_t wireIDmin=0;
   
   for(i=1;i<20;i++){
	tmpdd[i]=0.0;
	tmpwire[i]=0;
   }

   *_flag1=4; //denote event as a Z

   for(i=1;i<(*_wire_num-1);i++){
      if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
         maxddwire=i;
         //printf("maxddwire = %i \n",maxddwire);
      }
      if( (dd[i]<dd[i-1]) && (dd[i]<dd[i+1]) ) {     
         propermin=i;
         //printf("proper min dist at = %i \n",propermin);
      }
   }

   if( (maxddwire!=1000) && (propermin>maxddwire)  ){
      for(i=maxddwire;i<*_wire_num;i++){                         
         tmpdd[i-maxddwire]=dd[i];
         tmpwire[i-maxddwire]=wire[i];
      }
      *_wire_num=*_wire_num-maxddwire;
      for(i=0;i<*_wire_num;i++){     
         dd[i]=tmpdd[i];
         wire[i]=tmpwire[i];
      }
      *_flag1=0;
   }
   if( (maxddwire!=1000) && (propermin<maxddwire)  ){
      *_wire_num=maxddwire+1;
      *_flag1=0;
   }

   *_wireID_last  = *_wire_num-1;       
   for(i=1;i<*_wire_num;i++){	                   // Label wire associated with minimum drift distance
     if(dd[wireIDmin] > dd[i]) wireIDmin = i;    // Note: wireID_min an array index nr, NOT a real wire number
   }
   *_wireID_min=wireIDmin;

}

//------------------------------------------------------------------------------
void fixW(Double_t dd[],Int_t wire[], Int_t *_flag1, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_last){
// USED IN RAYTRACE
// Assume instead of a V we have something approaching a W
// One leg of the W should be cut off.
// Note we are here only concerned with a simple geometry
//
//  Shown below is for different wirenr the drift distance (high value on right, low on left
//  A proper event is a V with min at low distance. If the event resembles a W then you have a problem.
//  If the one leg of the W is removed by one or more wires, then you can assume that is the bad side.
//
//   16        x
//   15     x              In this example wires 9 and 10 are assumed to be bad
//   14  x
//   13    x 
//   12      x
//
//   10   x
//   9           x          It can also happen that the dd diff between the bad W leg is big
//
//
//   20       x  	In this example the wires 19 and 20 are assumed to be bad
//   19     x
//
//   15        x 
//   14      x
//   13    x
//   12  x
//   11     x
//   10        x
//
   Double_t tmpdd[20];
   Int_t tmpwire[20];
   Int_t i;
   Int_t badwire2=1000;
   Int_t maxddwire=1000;
   Int_t minddwire=1000;
   Int_t flagmin=1000;
   Int_t wireIDmin=0;
   
   for(i=1;i<20;i++){
	tmpdd[i]=0.0;
	tmpwire[i]=0;
   }

   *_flag1=5; //denote event as a W

   //First assumption is that the extra leg, to make a V a W, has at least one empty wire inbetween

   for(i=1;i<(*_wire_num-1);i++){
       if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
          if( (wire[i]-1)!=wire[i-1]){
  	     maxddwire=i;
             flagmin=1;				
             //printf("problem below: maxddwire = %i \n",maxddwire);	
          }
          if( (wire[i]+1)!=wire[i+1]){
  	     maxddwire=i;
             flagmin=2;				
             //printf("problem above: maxddwire = %i \n",maxddwire);		
          }
       } 
       if( (dd[i]<dd[i-1]) && (dd[i]<dd[i+1]) ) {     
          if( abs(dd[i]-dd[i-1])>5.) {
	     minddwire=i;
             flagmin=3;				
             //printf("big dd diff: minddwire = %i \n",minddwire);		
          }
       }
   }
   if( (maxddwire!=1000) && flagmin==1 ){
      for(i=maxddwire;i<*_wire_num;i++){                         
         tmpdd[i-maxddwire]=dd[i];
         tmpwire[i-maxddwire]=wire[i];
      }
      *_wire_num=*_wire_num-maxddwire;
      for(i=0;i<*_wire_num;i++){                         
         dd[i]=tmpdd[i];
         wire[i]=tmpwire[i];
      }
      *_flag1=0;
   }
   if( (maxddwire!=1000) && (flagmin==2)  ){
      //for(i=0;i<(maxddwire+1);i++){                         
      //   tmpdd[i]=dd[i];
      //   tmpwire[i]=wire[i];
      //   dd[i]=tmpdd[i];
      //   wire[i]=tmpwire[i];
      //}
      *_wire_num=maxddwire+1;
      *_flag1=0;
   }
   if( (minddwire!=1000) && flagmin==3 ){
      for(i=minddwire+1;i<*_wire_num;i++){                         
         tmpdd[i-(minddwire+1)]=dd[i];
         tmpwire[i-(minddwire+1)]=wire[i];
      }
      *_wire_num=*_wire_num-(minddwire+1);
      for(i=0;i<*_wire_num;i++){                         
         dd[i]=tmpdd[i];
         wire[i]=tmpwire[i];
      }
      *_flag1=0;
   }

   *_wireID_last  = *_wire_num-1;       
   for(i=1;i<*_wire_num;i++){	                   // Label wire associated with minimum drift distance
     if(dd[wireIDmin] > dd[i]) wireIDmin = i;    // Note: wireID_min an array index nr, NOT a real wire number
   }
   *_wireID_min=wireIDmin;


}


//------------------------------------------------------------------------------
void TryToImproveFit(Double_t dd[],Int_t wire[], Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_first, Int_t *_wireID_last, Double_t *_Xttif, Double_t *_Thttif, Double_t *_chisqttif, Double_t *_attif, Double_t *_bttif){

   Int_t i;
   Double_t x_tttv, sum_0=0.0, sum_x=0.0, sum_z=0.0, sum_xz=0.0, sum_x2=0.0;
   Double_t wirechisq=0;
   Double_t x_first, x_last, a_pre, b_pre, X_pre, Th_pre, drift_min_esti;
   Double_t X_v1=1.0, Th_v1=1.0,chisq_v1=0.0;
   Int_t wireIDmin;
   Double_t tmpdd[20];
   Int_t tmpwire[20];
   Double_t pos[20],ang[20],chisq[20],a[20],b[20];
   Double_t pos1,ang1,chisq1,a1,b1;   
   Int_t nrwirescut=3;
   Int_t goodindex=0;

   for(i=1;i<20;i++){
	tmpdd[i]=0.0;
	tmpwire[i]=0;
	chisq[i]=1000;
	pos[i]=0;
	ang[i]=0;
	a[i]=0;
	b[i]=0;
   }

   for(int sw=1;sw<nrwirescut+1;sw++){     	//sw = start-wire    Here we simply take starting wires away and see how chisq improves or not
      for(i=sw;i<*_wire_num;i++){                         
         tmpdd[i-sw]=dd[i];
         tmpwire[i-sw]=wire[i];
      }
      *_wire_num=*_wire_num-sw;
      for(i=0;i<*_wire_num;i++){                         
         dd[i]=tmpdd[i];
         wire[i]=tmpwire[i];
      }
      *_wireID_last  = *_wire_num-1;       
      for(i=1;i<*_wire_num;i++){	              // Label wire associated with minimum drift distance
        if(dd[wireIDmin] > dd[i]) wireIDmin = i;      
      }
      *_wireID_min=wireIDmin;

      // Determine tentative ray: straigh line through first and last wires
      x_first = X_WIRESPACE*(wire[*_wireID_first]);  //pos in mm of first wire of group
      x_last  = X_WIRESPACE*(wire[*_wireID_last]);   //pos in mm of last wire of group

      a_pre = (dd[*_wireID_first]+dd[*_wireID_last])/(x_first-x_last);    // slope
      b_pre = dd[*_wireID_first]-a_pre*x_first;                        // y offset
      X_pre = (-1.)*b_pre/a_pre;
      Th_pre =(-1.)*57.29578*atan(a_pre);
     
      // Use the above information to determine sign of drift distance
      // + drift on K600 side, - drift downstream side
      for(i=0;i<*_wire_num;++i){     
        if( (a_pre*(X_WIRESPACE*wire[i])+b_pre) < 0 ){
           dd[i] *= -1.;
        }
      }

      // Now do the least square fit 
      // Note where we ignore the wire with the smallest drifttime below, as it causes inaccuracies in pos determination
      sum_0=0; sum_x=0; sum_z=0; sum_xz=0; sum_x2=0; 

      for(i=0;i<*_wire_num;++i){
        if(dd[i]!=dd[*_wireID_min]){                 //ignore the wire with smallest drifttime
          x_tttv = X_WIRESPACE*(float(wire[i]));   // left -> high Ex  
          sum_0  += 1.0;
          sum_x  += x_tttv;
          sum_z  += dd[i];
          sum_xz += x_tttv*dd[i];
          sum_x2 += pow(x_tttv,2);
        }
      }
      a1 = (sum_x*sum_z-sum_0*sum_xz)/(pow(sum_x,2)-sum_0*sum_x2);
      b1 = (sum_x*sum_xz-sum_x2*sum_z)/(pow(sum_x,2)-sum_0*sum_x2);  
      pos1  = (-1.)*(b1 / a1);
      ang1 = (-1.)*57.29578*atan(a1); // [deg.]
 
      for(i=0;i<*_wire_num;++i){
        if(dd[i]!=dd[*_wireID_min]){
        chisq_v1 += pow(dd[i]-(a1*X_WIRESPACE*(wire[i])+b1),2); 
        ++wirechisq;
        }
      }
      chisq1 /= wirechisq;
      //printf("chisq minimization = %f   step %i\n",chisq_v1,sw);	

      chisq[sw-1]=chisq1;
      pos[sw-1]=pos1;
      ang[sw-1]=ang1;
      a[sw-1]=a1;
      b[sw-1]=b1;
      
   }

   for(int i=1;i<nrwirescut;i++){            // decide which is the lowest chisq
     if( chisq[goodindex] > chisq[i] ) goodindex = i;    
   }

   *_Xttif=pos[goodindex];
   *_Thttif=ang[goodindex];
   *_attif=a[goodindex];
   *_bttif=b[goodindex];
   *_chisqttif=chisq[goodindex];
   
}



//--------------------------------------------------------------------------------------
void raytrace(Double_t dd[],Int_t wire[],Double_t *_X,Double_t *_Th,Double_t *_chisq,Int_t wire_num,Double_t res[],Int_t *_flag, Int_t chamber, Double_t wused[] , Double_t dused[], Int_t *_wire_used, Int_t *_badwirecount, Int_t *_multmin, Int_t *_chisqminimization){

   Int_t i;
   Double_t x_tttv, sum_0=0.0, sum_x=0.0, sum_z=0.0, sum_xz=0.0, sum_x2=0.0;
   Int_t    wireID_first=0, wireID_last=0, wireID_min=0;
   Double_t wirechisq=0;
   Double_t x_first, x_last, a1, b1, a, b, a_pre, b_pre, X_pre, Th_pre, drift_min_esti;
   Double_t X_v1=1.0, Th_v1=1.0,chisq_v1=0.0;
   Int_t badwire=1000;
   Int_t badwirecntA=0;   // nr to send back: nr of initial events that had double hits in them (i.e. 2 time per tdc chan)
   Int_t badwirecntB=0;   // after 1st cycle of correction: nr of initial events that had another double hit in them.
   Int_t multiplemin=-1;  // for value = 4 it means a Z event, with its value = 5 it is a W event
   Int_t flag1=0;
   Int_t doublemin=0;

   /*   Meaning of flag1 values:
   =0	good event
   =1	events with more than 1 drift distance local minimum; a z or a w
   =2	events with more than 1 drift distance local minimum: one of the minima at start of wiregroup
   =3   events with more than 1 drift distance local minimum: one of the minima at the end of the wiregroup
   =4	events with more than 1 drift distance local minimum; a Z
   =5	events with more than 1 drift distance local minimum; a W
   */

   //printevent2(wire_num, wire,dd);

   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 1: ID first, last and min dist wires ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
   wireID_first = 0;                // wireID_first is an array index nr, NOT a real wire number
   wireID_last  = wire_num-1;       // wireID_last  is an array index nr, NOT a real wire number
   for(i=1;i<wire_num;i++){	                   // Label wire associated with minimum drift distance
     if(dd[wireID_min] > dd[i]) wireID_min = i;    // Note: wireID_min an array index nr, NOT a real wire number
   }

   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 2: ID double hits per wire and fix them ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
   for(i=0;i<(wire_num-1);i++){
      if( wire[i]==wire[i+1] ) {
 	 badwire=i;
	 badwirecntA+=1;              // i.e. with no multiple hits, this is 0. For double hits, it is 1. For 3 hits, this is 2.
      }				      // OR if we have 2 sets of double hits, we also have a value of 2 for badwirecntA
   }
   if(badwirecntA!=0){
      //printf("Double events per wire: badwire=%i : dd(i+1)=%f : wire_num=%i \n",badwire,dd[badwire+1],wire_num);
      //printevent2(wire_num, wire,dd);
      fixDoubleHit(dd,wire,badwire,&wire_num,&wireID_min,&wireID_last);
      //printf("Correction after SECOND PASS: double events per wire===========\n"); printevent2(wire_num, wire,dd);  printf("\n");
   }

   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 3: ID any remaining double hits per wire and fix them ~~~~~~~~~~~~~~~~~~~~~~~~~~~
   for(i=0;i<(wire_num-1);i++){			
     if( wire[i]==wire[i+1] ) {
	 badwire=i;
	 badwirecntB+=1;               // i.e. with no multiple hits, this is 0. For double hits, it is 1. For 3 hits, this is 2.
     }				      // OR if we have 2 sets of double hits, we also have a value of 2 for badwirecntB
   }
   if(badwirecntB!=0){
      //printf("SECOND PASS: Double events per wire: badwire=%i : dd(i+1)=%f : wire_num=%i \n",badwire,dd[badwire+1],wire_num);
      //printevent2(wire_num, wire,dd);
      fixDoubleHit(dd,wire,badwire,&wire_num,&wireID_min,&wireID_last); 
      //printf("Correction after SECOND PASS: double events per wire===========\n"); printevent2(wire_num, wire,dd);  printf("\n");
   }
   
   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 4: ID and fix Z and W events ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
   for(i=1;i<(wire_num-1);i++){
     if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
       if(dd[i-1]==dd[wireID_first]) flag1=2;        // one of the minima at start of wiregroup, a simple Z
       else if(dd[i+1]==dd[wireID_last])  flag1=3;   // one of the minima at the end of the wiregroup, a simple Z
       else flag1=1;				     // a Z with 2 or more wires, or a W
     }
   }
   if(flag1>0 && flag1<4){
     //printf("\n Established group is a Z or W event \n");
     for(i=1;i<(wire_num-1);i++){
       if( (dd[i]<dd[i-1]) && (dd[i]<dd[i+1]) ) {     
         doublemin++;
       }
     }
     if (doublemin==1){
	//printf("\n Established group is a Z event \n");  printevent2(wire_num, wire,dd);
        fixZ(dd,wire,&flag1, &wire_num, &wireID_min,&wireID_last);
        multiplemin=4;  
        //printf("================After Z Fix attempt===\n"); printevent2(wire_num, wire,dd);
     }
     if (doublemin>1){
	//printf("\n Established group is a W event \n"); printevent2(wire_num, wire,dd);
        fixW(dd,wire,&flag1, &wire_num, &wireID_min,&wireID_last);		
        multiplemin=5;
        //printf("================After W Fix attempt===\n");printevent2(wire_num, wire,dd);
     }
   }

   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 5: 2nd round, ID and fix Z and W events ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
   for(i=1;i<(wire_num-1);i++){
     if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
       if(dd[i-1]==dd[wireID_first]) flag1=2;        // one of the minima at start of wiregroup, a simple Z
       else if(dd[i+1]==dd[wireID_last])  flag1=3;   // one of the minima at the end of the wiregroup, a simple Z
       else flag1=1;				     // a Z with 2 or more wires, or a W
     }
   }
   if(flag1>0 && flag1<4){
     //printf("\n Established group is a Z or W event \n");
     for(i=1;i<(wire_num-1);i++){
       if( (dd[i]<dd[i-1]) && (dd[i]<dd[i+1]) ) {     
         doublemin++;
       }
     }
     if (doublemin==1){
	//printf("\n Established group is a Z event \n");  printevent2(wire_num, wire,dd);
        fixZ(dd,wire,&flag1, &wire_num, &wireID_min,&wireID_last);		
        //printf("================After Z Fix attempt===\n"); printevent2(wire_num, wire,dd);
     }
     if (doublemin>1){
	//printf("\n Established group is a W event \n"); printevent2(wire_num, wire,dd);
        fixW(dd,wire,&flag1, &wire_num, &wireID_min,&wireID_last);		
        //printf("================After W Fix attempt===\n");printevent2(wire_num, wire,dd);
     }
   }

   //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 6: do raytracing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
   // Determine tentative ray: straigh line through first and last wires
   x_first = X_WIRESPACE*(wire[wireID_first]);  //pos in mm of first wire of group
   x_last  = X_WIRESPACE*(wire[wireID_last]);   //pos in mm of last wire of group

   a_pre = (dd[wireID_first]+dd[wireID_last])/(x_first-x_last);    // slope
   b_pre = dd[wireID_first]-a_pre*x_first;                        // y offset
   X_pre = (-1.)*b_pre/a_pre;
   Th_pre =(-1.)*57.29578*atan(a_pre);
     
   // Use the above information to determine sign of drift distance
   // + drift on K600 side, - drift downstream side
   for(i=0;i<wire_num;++i){     
     if( (a_pre*(X_WIRESPACE*wire[i])+b_pre) < 0 ){
        dd[i] *= -1.;
     }
   }

   // Now do the least square fit 
   sum_0=0; sum_x=0; sum_z=0; sum_xz=0; sum_x2=0; 

   for(i=0;i<wire_num;++i){
     if(dd[i]!=dd[wireID_min]){                 // Ignore the wire with smallest drifttime. It causes bad pos calculations
       x_tttv = X_WIRESPACE*(float(wire[i]));   // left -> high Ex  
       sum_0  += 1.0;
       sum_x  += x_tttv;
       sum_z  += dd[i];
       sum_xz += x_tttv*dd[i];
       sum_x2 += pow(x_tttv,2);
     }
   }
   a1 = (sum_x*sum_z-sum_0*sum_xz)/(pow(sum_x,2)-sum_0*sum_x2);
   b1 = (sum_x*sum_xz-sum_x2*sum_z)/(pow(sum_x,2)-sum_0*sum_x2);  
   X_v1  = (-1.)*b1/a1;
   Th_v1 = (-1.)*57.29578*atan(a1); // [deg.]
 
   for(i=0;i<wire_num;++i){
     if(dd[i]!=dd[wireID_min]){
       chisq_v1 += pow(dd[i]-(a1*X_WIRESPACE*(wire[i])+b1),2); 
       ++wirechisq;
     }
   }
   chisq_v1 /= wirechisq;


   //if (chisq_v1>1. && wire_num>6){
        //printevent2(wire_num, wire,dd); printf(" chisq  before = %f \n",chisq_v1); 
//	TryToImproveFit(dd,wire,&wire_num,&wireID_min,&wireID_first,&wireID_last,&X_v1,&Th_v1,&chisq_v1,&a1,&b1);		
        //printevent2(wire_num, wire,dd);
//	*_chisqminimization=1;
  // }


   //----- these values are sent back to the main routine---------
   *_X = X_v1; 
   *_Th = Th_v1;             
   *_chisq = chisq_v1;
   a=a1;
   b=b1;
   *_flag=flag1;
   *_wire_used=wire_num;
   *_badwirecount=badwirecntB;
   *_multmin=multiplemin;

   for(i=0;i<wire_num;++i){    //sending back info on which wires and dd was used for fit
	//if(dd[i]!=dd[wireID_min]){ 	
		wused[i]=wire[i];
		dused[i]=dd[i];
	//}
   }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 7: calculate resolution variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

   for(i=0;i<10;i++){
     res[i]=-100.;     // initial value of res array 
   }

   if(wireID_first!=wireID_min && wireID_last!=wireID_min && wire_num>3){  
   // min drift distance may not be on edge of wire group, and only look at events >3 wires
 
     res[0]=(*_X/4.0)-int(*_X/4.);           
     //res[0] is a relative position in drift cell where track cross wireplane 
 
     res[1]=(dd[wireID_min+1]-dd[wireID_min])/(wire[wireID_min+1]-wire[wireID_min]) 
       + ((dd[wireID_min-1]-dd[wireID_min])/(wire[wireID_min]-wire[wireID_min-1]));  
     //same as XSYS calc
     //res[1] is the diff in slope. !!!!! Since wireID_min is used this is not very accurate
  
     #ifdef _VDCRESCALCS
     //if(wireID_first+4 == wireID_last && wire_num==5){     // calculate only for good 5wire events
     if(wire_num>4){ 
       res[2] = (dd[0]-dd[1])/1. - (dd[3]-dd[4])/1.;             
       //if(chamber==1) printf("VDC1: dd0 dd1 dd3 dd4  res[2] %f %f %f %f   %f \n",dd[0],dd[1],dd[3],dd[4],res[2]);
     }
     //res[2] is difference in slopes; // The RTN/Bertozzi method, except I do not divide by 4mm

     drift_min_esti = a* (X_WIRESPACE*(wire[wireID_min])) + b;   
     res[3]=drift_min_esti-dd[wireID_min];
     //res[3] is difference in position calculated at wire of min drifttime, and measured at that wire

     drift_min_esti = abs((dd[wireID_min+1]-dd[wireID_min-1]))/2.; // drift_min_esti from RCNP reports
     res[4] =drift_min_esti - dd[wireID_min];  
     //res[4] is difference in position calculated at wire of min drifttime, and measured at that wire

     drift_min_esti=( (wireID_last-wireID_min)*dd[wireID_first] + (wireID_min-wireID_first)*dd[wireID_last] )
                    / (wireID_last - wireID_first);   // drift_min_esti from RCNP analyzer sorter_user.c
     res[5]=drift_min_esti- dd[wireID_min];
     //res[5] is difference in position calculated at wire of min drifttime, and measured at that wire
     
     res[6]=(dd[wireID_first]-dd[wireID_first+1]) - (dd[wireID_first+2] - dd[wireID_first+3]);
     //res[6] is similar to res[2]; the diff in slopes

     res[7]=dd[wireID_min];

     drift_min_esti = a* (X_WIRESPACE*(wire[wireID_min])) + b;   // drift_min_esti from EVAL code?????true???
     res[8]=drift_min_esti;
     #endif

   }
}





//--------------------------------------------------------------------------------------
void CalcThetaFP(Double_t X1, Double_t X2, Double_t *Theta)
{
   Double_t x;
   x=(X2 + globals.x_x1x2) - X1 ;               
   *Theta=57.29578*atan(globals.z_x1x2/x);
}

//--------------------------------------------------------------------------------------
void CalcThetaScat(Double_t Thetafp, Double_t X1, Double_t *Thetascat)
{
   Double_t A,B;
   A=gates.a0 + gates.a1*X1 + gates.a2*X1*X1;               
   B=gates.b0 + gates.b1*X1 + gates.b2*X1*X1;               
   *Thetascat=1*(A*Thetafp+B);
   //printf("Thetascat %f  \n",*Thetascat);
   //Theta_SCAT=(-0.741205 + -9.34925e-05*X)*Theta_FP + (25.5606 + -0.00193707*X) 
}

//--------------------------------------------------------------------------------------
void CalcPhiScat(Double_t Yfp, Double_t X1, Double_t ThSC, Double_t *Phiscat)
{
   //Double_t A,B;
   Double_t parA0,parA1,parA2,parA3;
   Double_t parB0,parB1,parB2,parB3;

   parA0=gates.c00 + gates.c01*X1 + gates.c02*X1*X1;               
   parA1=gates.c10 + gates.c11*X1 + gates.c12*X1*X1;               
   parA2=gates.c20 + gates.c21*X1 + gates.c22*X1*X1;               
   parA3=gates.c30 + gates.c31*X1 + gates.c32*X1*X1;               

   parB0=gates.d00 + gates.d01*X1 + gates.d02*X1*X1;               
   parB1=gates.d10 + gates.d11*X1 + gates.d12*X1*X1;               
   parB2=gates.d20 + gates.d21*X1 + gates.d22*X1*X1;               
   parB3=gates.d30 + gates.d31*X1 + gates.d32*X1*X1;     
 
   //*Phiscat=1*(A*Yfp+B);
   *Phiscat=1*((parA0 + parA1*ThSC + parA2*ThSC*ThSC + parA3*ThSC*ThSC*ThSC)*Yfp
	    +(parB0 + parB1*ThSC + parB2*ThSC*ThSC + parB3*ThSC*ThSC*ThSC));
}

//--------------------------------------------------------------------------------------
void CalcCorrX(Double_t X, Double_t Y, Double_t ThetaSCAT, Double_t *Xcorr)
//lineshape correction
{

  double result = 0;
  extern int NXThetaCorr;
  extern double *XThetaCorr;
  extern int NXY1Corr;
  extern double *XY1Corr;

  *Xcorr = 0;
  for(int i=0;i<NXThetaCorr;i++)
  {
    if(i==0)result = X;
    if(i>0)result += XThetaCorr[i] * pow(ThetaSCAT,i);//Correct the rigidity based on the ThetaSCAT value
      //printf("Xcorr: %f\n",*Xcorr);
  }

  //At this point, result is X1posC after the ThSCAT correction


  for(int i=0;i<NXY1Corr;i++)
    {
      if(i==0)result = result;
      if(i>0)result += XY1Corr[i] * pow(Y,i);
    }

  *Xcorr = result;

}

//--------------------------------------------------------------------------------------
double CalcQBrho(double Xcorr)
// Brho in Tm
{
  //double rig = 3.79765 + 3.24097e-4*Xcorr + 2.40685e-8*Xcorr*Xcorr;
  //double rig = 3.78562 + 0.000351737*Xcorr - 1.95361e-10*Xcorr*Xcorr;
  //std::cout << "rig: " << rig << std::endl;

  extern int NXRigidityPars;
  extern double *XRigidityPars;

  double rig = 0;
  for(int i=0;i<NXRigidityPars;i++)
    {
      //printf("XRigidityPars[%d]: %e\n",i,XRigidityPars[i]);
      rig += XRigidityPars[i] * pow(Xcorr,(double)i);
    }
  //std::cout << "rig: " << rig << std::endl;
  return rig;
}

//--------------------------------------------------------------------------------------
double CalcTfromXcorr(double Xcorr, double mass)
{
  double T = 0;

  double rig = CalcQBrho(Xcorr);

  double p = rig * TMath::C()/1e6;
  //std::cout << "p3: " << p3 << std::endl;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
  //std::cout << "T3: " << T << std::endl;
  return T;
}

//--------------------------------------------------------------------------------------
double CalcTfromRigidity(double rig, double mass)
{
  double T = 0;

  double p = rig * TMath::C()/1e6;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
}

//--------------------------------------------------------------------------------------
double CalcTfromP(double p, double mass)
{
  double T = 0;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
}

//--------------------------------------------------------------------------------------
double CalcEx(double Xcorr)
{
  double exE = 0;
  extern double T1;
  double  T2 = 0, T3 = 0, T4 = 0;//Energies in MeV

  double p1, p2, p3, p4;   //Momenta for each particle

  extern double *masses;  //This is a pointer array containing the information on the particle masses involved in the reaction
}

double CalcQBrho(double Xcorr)
{
  //double rig = 3.79765 + 3.24097e-4*Xcorr + 2.40685e-8*Xcorr*Xcorr;
  //double rig = 3.78562 + 0.000351737*Xcorr - 1.95361e-10*Xcorr*Xcorr;
  //std::cout << "rig: " << rig << std::endl;

  extern int NXRigidityPars;
  extern double *XRigidityPars;

  double rig = 0;
  for(int i=0;i<NXRigidityPars;i++)
    {
      //printf("XRigidityPars[%d]: %e\n",i,XRigidityPars[i]);
      rig += XRigidityPars[i] * pow(Xcorr,(double)i);
    }
  //std::cout << "rig: " << rig << std::endl;
  return rig;
}

double CalcTfromXcorr(double Xcorr, double mass)
{
  double T = 0;

  double rig = CalcQBrho(Xcorr);

  double p = rig * TMath::C()/1e6;
  //std::cout << "p3: " << p3 << std::endl;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
  //std::cout << "T3: " << T << std::endl;
  return T;
}

double CalcTfromRigidity(double rig, double mass)
{
  double T = 0;

  double p = rig * TMath::C()/1e6;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
}

double CalcTfromP(double p, double mass)
{
  double T = 0;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
}

double CalcExDirect(double Xcorr)
{
  double result = 0;
  result = 31.3029 - 0.0299994*Xcorr - 1.54103e-6*pow(Xcorr,2.);
  return result;
}

double CalcEx(double Xcorr)
{
  double exE = 0;

  extern double T1;
  double  T2 = 0, T3 = 0, T4 = 0;//Energies in MeV

  double p1, p2, p3, p4; //Momenta for each particle

  extern double *masses;//This is a pointer array containing the information on the particle masses involved in the reaction
>>>>>>> master

  extern double theta3;
  double theta4 = 0;

  extern bool TestInelastic;
  if(TestInelastic)
    {
      masses[2] = masses[0];
      masses[3] = masses[1];
    }

  p1 = sqrt(T1 * (T1 + 2*masses[0]));
  p2 = 0;
  p3 = CalcQBrho(Xcorr) * TMath::C()/1e6;
  //std::cout << "p3: " << p3 << std::endl;
  T3 = CalcTfromXcorr(Xcorr,masses[2]);
  //std::cout << "T3: " << T3 << std::endl;
  if(theta3 == 0)
    {
      theta4 = 0;
      
      p4 = p1 - p3;
      T4 = sqrt(p4*p4 + masses[3]*masses[3]) - masses[3];
      exE = T1 - T3 - T4;
    }
  else
    {
      theta4 = 180./TMath::Pi()*atan(sin(theta3*TMath::Pi()/180.)/(p1/p3 - cos(theta3*TMath::Pi()/180.)));
      //std::cout << "theta4: " << theta4 << std::endl;
      p4 = p3 * sin(theta3*TMath::Pi()/180.)/sin(theta4*TMath::Pi()/180.);
      T4 = CalcTfromP(p4,masses[3]);
      //std::cout << "T4: " << T4 << std::endl;
      exE = T1 - T3 - T4;
    }
  //std::cout << "exE: " << exE << std::endl;
  if(exE<0)exE=0;
  if(Xcorr>800)exE=0;
  return exE;
}

//--------------------------------------------------------------------------------------

//void testrunnr(Int_t runn)
//{
//  printf("\n==================================================================== %i  \n\n\n\n",runn);
//
//}

//--------------------------------------------------------------------------------------
void CalcYFP(Double_t x, Double_t u, Double_t thFP, Double_t *y)
{
  Double_t sinu=0.766044443; //sin(U_WIRE_ANGLE/57.2957); 
  Double_t tanu=1.191753593; //tan(U_WIRE_ANGLE/57.2957);
  Double_t tmp1,tmp2;
  Double_t tanfp=tan(thFP/57.2957);

// for UX configuration. See RN K600 book6 p20-23
//  tmp1=(u*tanfp+sinu*16);    
//  tmp2=sinu*tanfp;
//  *y=(tmp1/tmp2-x)*tanu+76.27 -50;  // the 76.27 is the offset due to first u and x wires not sharing the same origin
				      // the -50 is to put it around zero
// for XU configuration
  tmp1=(u*tanfp-sinu*16);    
  tmp2=sinu*tanfp;
  *y=-1*((tmp1/tmp2-x)*tanu+29.85);        
 
  //printf("x=%f  u=%f  ThFP=%f  sinu=%f  tanu=%f tanThFP=%f  y=%f \n",x,u,thFP,sinu,tanu,tanfp,(x-tmp1/tmp2)*tanu);
  //printf("%f %f %f %f %f %f %f \n",x,u,thFP,sinu,tanu,tanfp,(x-tmp1/tmp2)*tanu);
}

//--------------------------------------------------------------------------------------
void CalcPhiFP(Double_t X1, Double_t Y1, Double_t X2, Double_t Y2,  Double_t thFP, Double_t *Phi)
{
   Double_t y;
   y= Y2 - Y1 ;               
   *Phi=57.29578*atan(y*sin(thFP/57.29578)/globals.z_x1x2);
   //printf("y1=%f  y2=%f  z_x1x2=%f ThFP = %f phi=%f \n",Y1,Y2,globals.z_x1x2,thFP,57.29578*atan(y/(globals.z_x1x2/sin(thFP/57.29578))));
}

//--------------------------------------------------------------------------------------

double CalcThetaPrime(double X1, double ThFP)
{
  //Using the result of the fit:  
  //TF2 *fit2 = new TF2("fit2","y*([0]+[1]*x+[2]*x*x) + [3] + [4]*x + [5]*x*x",250,660,28,33); where y = ThFP and x = X1

  double *ThetaPrimePars = new double[6];
  ThetaPrimePars[0] = -0.872219;
  ThetaPrimePars[1] = -0.000434535;
  ThetaPrimePars[2] = 4.58708e-07;
  ThetaPrimePars[3] = 27.5189;
  ThetaPrimePars[4] = 0.0134774;
  ThetaPrimePars[5] = -1.77584e-05;

  double result = ThFP*(ThetaPrimePars[0] + ThetaPrimePars[1]*X1 + ThetaPrimePars[2]*X1*X1) +  ThetaPrimePars[3] +  ThetaPrimePars[4]*X1 +  ThetaPrimePars[5]*X1*X1;
  delete ThetaPrimePars;
  return result;
}

//--------------------------------------------------------------------------------------


//double CalcPhiPrime(double X1, double ThFP, double Y1)
//{
  
//}


//--------------------------------------------------------------------------------------

double CalcTheta(double X1, double ThFP, double Y1)
{
  extern double theta3;//K600 scattering angle

  double ThetaPrime = CalcThetaPrime(X1,ThFP);

  double PhiPrime = CalcPhiPrime(X1, ThFP, Y1);
  
  double result = -1;

  result = sqrt(pow(ThetaPrime + theta3,2.) + pow(PhiPrime,2.));
}

/*-- BOR routine --------------------Happens after init-----------------------------------------*/
INT focal_bor(INT run_number)
{
   //memset(scaler, 0, sizeof(scaler));
   GetODBRunInfo();       // this was put here so that one can get access to run nr info, if you want
		          // to set up the analyzer to look at runnr, and then decide which offsets
		          // or calib paraters it will use. See Fhum's email to me: 
		          // 	Subject: 	functions
			  //    Date: 	Thu, 7 Feb 2013 15:50:35 +0200 (SAST)
   
   GetODBGlobals();                // get globals that can be set in the ODB
   //PrintODBstuff();

   read_lut(lutx1,globals.lut_x1_offset,"lut-x1.dat");              
   read_lut(lutu1,globals.lut_u1_offset,"lut-u1.dat");         
   read_lut(lutx2,globals.lut_x2_offset,"lut-x2.dat");   
   read_lut(lutu2,globals.lut_u2_offset,"lut-u2.dat");              
     
   for(int j = 0; j < LUT_CHANNELS; j++) {                   // And immediately fill LUT spectra
     for(int k = 0; k < lutx1[j]*1000; k++) {
        hX1_lut->Fill(j);
     }
     for(int k = 0; k < lutx2[j]*1000; k++) {
        hX2_lut->Fill(j);
     }
     for(int k = 0; k < lutu1[j]*1000; k++) {
        hU1_lut->Fill(j);
     }
     for(int k = 0; k < lutu2[j]*1000; k++) {
        hU2_lut->Fill(j);
     }
   }
 
   printf("lut x1 offset: %d \n",globals.lut_x1_offset);
   printf("lut u1 offset: %d \n",globals.lut_u1_offset);
   printf("lut x2 offset: %d \n",globals.lut_x2_offset);
   printf("lut u2 offset: %d \n",globals.lut_u2_offset);

   switch(runinfo2.run_number){

       case 1089: x1offset=-0.9; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1090: x1offset=-0.9-0.109863; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1091: x1offset=-0.9-0.15863; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1092: x1offset=-0.9-0.207886; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1093: x1offset=-0.9-0.0908203; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1094: x1offset=-0.9-0.138123; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1095: x1offset=-0.9-0.057; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1096: x1offset=-0.9-0.0791626; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1099: x1offset=-0.9+0.0325928; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1100: x1offset=-0.9-0.0883179; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1101: x1offset=-0.9-0.0873413; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1106: x1offset=-0.9-0.195862; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1029: x1offset=-0.9-0.125366; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1035: x1offset=-0.9-0.230042; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1045: x1offset=-0.9+0.150452; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1046: x1offset=-0.9+0.278992; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1062: x1offset=-0.9+0.251282; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1080: x1offset=-0.9-0.211487; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   

   // in case of analysis of LiCO data
        case 1023: x1offset=0; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1024: x1offset=-0.0103149; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1025: x1offset=-0.0167236; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1026: x1offset=-0.0152588; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1030: x1offset=-0.00427246; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1032: x1offset=-0.00457764; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1033: x1offset=-0.0109253; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1034: x1offset=-0.0296631; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1036: x1offset=-0.0206909; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1039: x1offset=-0.0289307; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1041: x1offset=-0.0429688; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1043: x1offset=-0.0498657; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1044: x1offset=-0.0682983; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1047: x1offset=-0.0731812; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1048: x1offset=-0.0797119; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1052: x1offset=0.0429688; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1053: x1offset=0.0273438; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1056: x1offset=0.0322876; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1058: x1offset=0.0302124; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1059: x1offset=0.0234375; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1060: x1offset=0.0203857; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1070: x1offset=-0.0385132; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1071: x1offset=-0.0546265; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1073: x1offset=-0.0177612; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1075: x1offset=0.00714111; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1078: x1offset=0.0237427; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1079: x1offset=0.0909424; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1084: x1offset=0.233093; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1085: x1offset=0.71875; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1086: x1offset=0.379822; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 1087: x1offset=0.154419; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;  
   }
   return SUCCESS;
}


/*-- EOR routine -------------------------------------------------------------------------------*/
INT focal_eor(INT run_number)
{
   return SUCCESS;
}


/*-- init routine ---------------------Happens before BOR----------------------------------------*/
INT focal_init(void)                    
{
   char name[256];
   char title[256];

   setupchannel2wireXUXU();    
   //setupchannel2wireXoldXold();

   ParameterInit();

   extern bool VDC1_new, VDC2_new;

   //setupchannel2wireXoldXold();
   
   if(VDC1_new)
     {
       if(VDC2_new)
	 {
 	   //setupchannel2wireXUXU();
	 }
       else
	 {
	   printf("Setup for a new VDC1 and an old VDC2\n");
// 	   setupchannel2wireXUXold();
	 }
     }
   else
     {
       if(VDC2_new)
	 {
//  	   setupchannel2wireXoldXU();
	 }
       else
	 {
//  	   setupchannel2wireXoldXold();
	 }
     }
   
//    setupchannel2wireXUXold();

   #ifdef _MISALIGNTIME
   read_misalignment(&misaligntime,"misalignment.dat");
   printf("== Misalignment time cutoff (sec): %i ==\n",misaligntime);
   #endif

   read_cable(cableOffset,"CableLength.dat");

   open_subfolder("LUT&Cable");
     hX1_lut = H1_BOOK("hX1_lut","LUT X1",LUT_CHANNELS,0,LUT_CHANNELS);
     hU1_lut = H1_BOOK("hU1_lut","LUT U1",LUT_CHANNELS,0,LUT_CHANNELS);
     hX2_lut = H1_BOOK("hX2_lut","LUT X2",LUT_CHANNELS,0,LUT_CHANNELS);
     hU2_lut = H1_BOOK("hU2_lut","LUT U2",LUT_CHANNELS,0,LUT_CHANNELS);
     hCableOff = H1_BOOK("hCableOff","cable offsets)",896,0,896);
     hCableOfftmp = H1_BOOK("hCableOfftmp","cable offset aid: -1000 for all channels)",896,0,896);
   close_subfolder(); 

   for(int j = 0; j < 896; j++) {
     //printf("cable offset %f \n",cableOffset[j]);
     //     for(int k = 0; k < (int(cableOffset[j])+ALL_CABLE_OFFSET); k++) {  
     for(int k = 0; k < (int(cableOffset[j]+1000)); k++) {  
       hCableOff->Fill(j);                     
     }
     for(int k = 0; k < 1000; k++) {  
       hCableOfftmp->Fill(j);                     
     }
   }
   hCableOff->Add(hCableOfftmp,-1);   //because I do not know how to fill a neg histogram

   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   hEventID  = H1_BOOK("hEventID","each bin represents a type of event",30,0,30);
   hEventID2  = H1_BOOK("hEventID2","each bin represents a type of event",30,0,30);

   hPad1VsTofG  = H2_BOOK("hPad1VsTofG","Paddle 1 ave VS TOF (gated)", 2000,TDC_MIN_TIME,TDC_MAX_TIME, 1000, 0, 4095 );
   hPad1Pad2   = H2_BOOK("hPad1Pad2 ","Pad1(y-axis) vs Pad2(x-axis)",  1024, 0, 4096, 1024,0 , 4096);
   hPad1Pad2G  = H2_BOOK("hPad1Pad2G","Pad1(y-axis) vs Pad2(x-axis) (PID gated)",  1024, 0, 4096, 1024,0 , 4096);

   hTDCPerEventRaw = H1_BOOK("hTDCPerEventRaw","TDC channels/event (All data)",600,0,600);
   //hTDCPerEvent    = H1_BOOK("hTDCPerEvent","TDC channels/event (PID selected)",MAX_WIRES_PER_EVENT,0,MAX_WIRES_PER_EVENT);
   
   for(int counter=0;counter<NR_OF_TDCS;counter++){
	  sprintf(name,"hTDC2DModule%d",counter);
	  sprintf(title,"hTDC2DModule %d ",counter);
          hTDC2DModule[counter]=H2_BOOK(name,title, 4000, -8000, 14999,128,0,128);
   }

   hHitPatternRawTDC   = H1_BOOK("hHitPatternRawTDC","Hits per raw TDC chan",1000,0,1000);
   hHitPatternAll   = H1_BOOK("hHitPatternAll","Hits/Chan (ALL data)",1000,0,1000);
   hHitPatternPID   = H1_BOOK("hHitPatternPID","Hits/Chan (PID selected)",1000,0,1000);
  
   hChanVsTimeRef       = H2_BOOK("hChanVsRefTime","TDC channel vs time (ref times incl)", 3000, 0, 15000, 896, 0, 896);
   hChanVsTimeOffset    = H2_BOOK("hChanVsOffsetTime","TDC channel vs time (cablelenghts offsets incl)", 1500, 0, 15000, 896, 0, 896);
   hChanVsTimeOffsetPID = H2_BOOK("hChanVsOffsetTimePID","TDC channel vs time (cablelenghts offsets incl)", 1200, 4000, 10000, 896, 0, 896);
   hWireVsTimeOffset    = H2_BOOK("hWireVsOffsetTime","Wire channel vs time (cablelenghts offsets incl)", 1500, 0, 15000, 1000, 0, 1000);
   hWireVsTimeOffsetPID = H2_BOOK("hWireVsOffsetTimePID","Wire channel vs time (cablelenghts offsets incl) PID selected", 1200, 4000, 10000, 1000, 0, 1000);

   hX1_EffID    = H1_BOOK("hX1_EffID","VDC efficiency calculation: X1 ",20,0,20);
   hU1_EffID    = H1_BOOK("hU1_EffID","VDC efficiency calculation: U1 ",20,0,20);
   hX2_EffID    = H1_BOOK("hX2_EffID","VDC efficiency calculation: X2 ",20,0,20);
   hU2_EffID    = H1_BOOK("hU2_EffID","VDC efficiency calculation: U2 ",20,0,20);

   hX1_DriftLength = H1_BOOK("hX1_DriftLength","DriftLength X1 (mm)  ",110,-1,10);
   hX2_DriftLength = H1_BOOK("hX2_DriftLength","DriftLength X2 (mm)  ",110,-1,10);
   hU1_DriftLength = H1_BOOK("hU1_DriftLength","DriftLength U1 (mm)  ",110,-1,10);
   hU2_DriftLength = H1_BOOK("hU2_DriftLength","DriftLength U2 (mm)  ",110,-1,10);

   hX1_DriftTimeGood = H1_BOOK("hX1_DriftTimeGood","X1 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU1_DriftTimeGood = H1_BOOK("hU1_DriftTimeGood","U1 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hX2_DriftTimeGood = H1_BOOK("hX2_DriftTimeGood","X2 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU2_DriftTimeGood = H1_BOOK("hU2_DriftTimeGood","U2 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   #ifdef _FULLANALYSIS
   hDriftTimeRawAll  = H1_BOOK("hDriftTimeRawAll","All drifttimes (before subtracting ref times), 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeOffsetAll  = H1_BOOK("hDriftTimeOffsetAll","All drifttimes (cablelenghts offsets included), 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hX1_DriftTimeRef  = H1_BOOK("hX1_DriftTimeRef","X1 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hX2_DriftTimeRef  = H1_BOOK("hX2_DriftTimeRef","X2 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU1_DriftTimeRef  = H1_BOOK("hU1_DriftTimeRef","U1 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU2_DriftTimeRef  = H1_BOOK("hU2_DriftTimeRef","U2 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hX1_DriftTimeOffset = H1_BOOK("hX1_DriftTimeOff","X1 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hX2_DriftTimeOffset = H1_BOOK("hX2_DriftTimeOff","X2 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU1_DriftTimeOffset = H1_BOOK("hU1_DriftTimeOff","U1 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU2_DriftTimeOffset = H1_BOOK("hU2_DriftTimeOff","U2 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hX1_Hits  = H1_BOOK("hX1_Hits","X1 wires/event (all X1 events)",20,0,20);
   hX2_Hits  = H1_BOOK("hX2_Hits","X2 wires/event (all U1 events)",20,0,20);
   hU1_Hits  = H1_BOOK("hU1_Hits","U1 wires/event (all X2 events)",20,0,20);
   hU2_Hits  = H1_BOOK("hU2_Hits","U2 wires/event (all U2 events)",20,0,20);

   hX1_HitsG  = H1_BOOK("hX1_HitsG","X1 wires/event (good event)",20,0,20);
   hX2_HitsG  = H1_BOOK("hX2_HitsG","X2 wires/event (good event)",20,0,20);
   hU1_HitsG  = H1_BOOK("hU1_HitsG","U1 wires/event (good event)",20,0,20);
   hU2_HitsG  = H1_BOOK("hU2_HitsG","U2 wires/event (good event)",20,0,20);

   hX1_Chisq        = H1_BOOK("hX1_Chisq","Chi squared: X1 ",500,0,15);
   hX1_Eff          = H1_BOOK("hX1_Eff","VDC efficiency: X1 ",1000,0,100);
   hX1_PosEff       = H2_BOOK("hX1_PosEff","VDC efficiency along focal plane: X1 ",200,0,800,1000,0,100);
   hU1_Chisq     = H1_BOOK("hU1_Chisq","Chi squared: U1 ",500,0,15);
   hU1_Eff       = H1_BOOK("hU1_Eff","VDC efficiency: U1 ",1000,0,100);
   hX2_Chisq     = H1_BOOK("hX2_Chisq","Chi squared: X2 ",500,0,15);
   hX2_Eff       = H1_BOOK("hX2_Eff","VDC efficiency: X2 ",1000,0,100);
   hU2_Chisq     = H1_BOOK("hU2_Chisq","Chi squared: U2 ",500,0,15);
   hU2_Eff       = H1_BOOK("hU2_Eff","VDC efficiency: U2 ",1000,0,100);

   hX1_Res       = H1_BOOK("hX1_Res","X1 position resolution",400,-8.,8.);
   hX1_Res2dRCNP = H2_BOOK("hX1_Res2dRCNP","RCNP style resolution plot: X1", 200,-3,3,200,-3.,3.);
   hX1_Res2diTL  = H2_BOOK("hX1_Res2diTL","iTL style resolution plot for X1 (xaxis=pos-int(pos)) ", 200,-0.1,1.1,200,-3.,3.);
   hU1_Res       = H1_BOOK("hU1_Res","U1 position resolution",400,-8.,8.);
   hU1_Res2dRCNP = H2_BOOK("hU1_Res2dRCNP","Min.drift-Estimated VS Min.drift: U1", 200,-3,3,200,-3.,3.);
   hU1_Res2diTL  = H2_BOOK("hU1_Res2diTL","iTL style resolution plot: U1", 200,-0.1,1.1,200,-3.,3.);
   hX2_Res       = H1_BOOK("hX2_Res","X2 position resolution",400,-8.,8.);
   hX2_Res2dRCNP = H2_BOOK("hX2_Res2dRCNP","Min.drift-Estimated VS Min.drift: X2", 200,-3,3,200,-3.,3.);
   hX2_Res2diTL  = H2_BOOK("hX2_Res2diTL","iTL style resolution plot: X2", 200,-0.1,1.1,200,-3.,3.);
   hU2_Res       = H1_BOOK("hU2_Res","U2 position resolution",400,-8.,8.);
   hU2_Res2dRCNP = H2_BOOK("hU2_Res2dRCNP","Min.drift-Estimated VS Min.drift: U2", 200,-3,3,200,-3.,3.);
   hU2_Res2diTL  = H2_BOOK("hU2_Res2diTL","iTL style resolution plot: U2", 200,-0.1,1.1,200,-3.,3.);

   h_Y1   = H1_BOOK("h_Y1","Y1 ",400,-100,100);
   h_Y2   = H1_BOOK("h_Y2","Y2 ",400,-100,100);
   #endif

   #ifdef _JJAUTOTRIM                           // if you use JJ_autotrim.C
   open_subfolder("Individual TDC chan");
   open_subfolder("X1_Refs");
   for(int i=0;i<TDC_CHANNELS;i++){
	sprintf(name,"TDCchan_%d",i);
	sprintf(title,"TDC channel # %d (reftimes incl) ",i);
   	hTDC_REF[i] = H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   }
   close_subfolder();
   close_subfolder();   
   #endif

   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  t1->Branch("runtime",&t_runtime,"t_runtime/I");
  t1->Branch("evtcounter",&t_evtcounter,"t_evtcounter/I");
  t1->Branch("tdcsperevent",&t_tdcsperevent,"t_tdcsperevent/I");

  t1->Branch("triggerU",&t_triggerU,"t_triggerU/I");
  t1->Branch("triggerI",&t_triggerI,"t_triggerI/I");
  t1->Branch("CIU",&t_CIU,"t_CIU/I");
  t1->Branch("CII",&t_CII,"t_CII/I");

  t1->Branch("tof",&t_tof,"t_tof/I");
  t1->Branch("toftdc2",&t_toftdc2,"t_toftdc2/I");
  t1->Branch("toftdc3",&t_toftdc3,"t_toftdc3/I");
  t1->Branch("toftdc4",&t_toftdc4,"t_toftdc4/I");
  t1->Branch("toftdc5",&t_toftdc5,"t_toftdc5/I");
  t1->Branch("toftdc6",&t_toftdc6,"t_toftdc6/I");
  t1->Branch("toftdc7",&t_toftdc7,"t_toftdc7/I");

  t1->Branch("k600",&t_k600,"t_k600/I");

  t1->Branch("pad1",&t_pad1,"t_pad1/D");
  t1->Branch("pad2",&t_pad2,"t_pad2/D");
  t1->Branch("pad1hiP",&t_pad1hiP,"t_pad1hiP/D");
  t1->Branch("pad1lowP",&t_pad1lowP,"t_pad1lowP/D");
  t1->Branch("pad2hiP",&t_pad2hiP,"t_pad2hiP/D");
  t1->Branch("pad2lowP",&t_pad2lowP,"t_pad2lowP/D");
  t1->Branch("pad1hiPT",&t_pad1hiPT,"t_pad1hiPT/D");
  t1->Branch("pad1lowPT",&t_pad1lowPT,"t_pad1lowPT/D");
  t1->Branch("pad2hiPT",&t_pad2hiPT,"t_pad2hiPT/D");
  t1->Branch("pad2lowPT",&t_pad2lowPT,"t_pad2lowPT/D");

  t1->Branch("X1pos",&t_X1pos,"t_X1pos/D");
  t1->Branch("X1th",&t_X1th,"t_X1th/D");
  t1->Branch("X1flag",&t_X1flag,"t_X1flag/I");
  t1->Branch("X1chisq",&t_X1chisq,"t_X1chisq/D");
  t1->Branch("X1chisqminimization",&t_X1chisqminimization,"t_X1chisqminimization/I");
  t1->Branch("X1res0",&t_X1res0,"t_X1res0/D");
  t1->Branch("X1res1",&t_X1res1,"t_X1res1/D");
  t1->Branch("X1hits",&t_X1hits,"t_X1hits/I");
  //t1->Branch("X1effID",&t_X1effID,"t_X1effID/D");
  t1->Branch("X1effall",&t_X1effall,"t_X1effall/D");
  t1->Branch("X1effdt",&t_X1effdt,"t_X1effdt/D");
  t1->Branch("X1effgroup",&t_X1effgroup,"t_X1effgroup/D");
  t1->Branch("X1effgood",&t_X1effgood,"t_X1effgood/D");
  #ifdef _VDCRESCALCS
  t1->Branch("X1res2",&t_X1res2,"t_X1res2/D");
  t1->Branch("X1res3",&t_X1res3,"t_X1res3/D");
  t1->Branch("X1res4",&t_X1res4,"t_X1res4/D");
  t1->Branch("X1res5",&t_X1res5,"t_X1res5/D");
  t1->Branch("X1res6",&t_X1res6,"t_X1res6/D");
  t1->Branch("X1res7",&t_X1res7,"t_X1res7/D");
  t1->Branch("X1res8",&t_X1res8,"t_X1res8/D");
  #endif
  #ifdef _FULLANALYSIS
  t1->Branch("nX1wiresUsed",&t_nX1wiresUsed,"t_nX1wiresUsed/I");
  t1->Branch("X1doublewires",&t_X1doublewires,"t_X1doublewires/I");
  t1->Branch("X1multiplemin",&t_X1multiplemin,"t_X1multiplemin/I");
  t1->Branch("X1TimeDiff",&t_X1TimeDiff,"t_X1TimeDiff/D");
  t1->Branch("X1wireUsed",t_X1wireUsed,"t_X1wireUsed[t_nX1wiresUsed]/I");
  t1->Branch("X1distUsed",t_X1distUsed,"t_X1distUsed[t_nX1wiresUsed]/D");
  #endif

  t1->Branch("U1pos",&t_U1pos,"t_U1pos/D");
  t1->Branch("U1th",&t_U1th,"t_U1th/D");
  t1->Branch("U1flag",&t_U1flag,"t_U1flag/I");
  t1->Branch("U1chisq",&t_U1chisq,"t_U1chisq/D");
  t1->Branch("U1res0",&t_U1res0,"t_U1res0/D");
  t1->Branch("U1res1",&t_U1res1,"t_U1res1/D");
  //t1->Branch("U1effID",&t_U1effID,"t_U1effID/D");
  t1->Branch("U1hits",&t_U1hits,"t_U1hits/I");
  t1->Branch("U1effall",&t_U1effall,"t_U1effall/D");
  t1->Branch("U1effdt",&t_U1effdt,"t_U1effdt/D");
  t1->Branch("U1effgroup",&t_U1effgroup,"t_U1effgroup/D");
  t1->Branch("U1effgood",&t_U1effgood,"t_U1effgood/D");
  #ifdef _VDCRESCALCS
  t1->Branch("U1res2",&t_U1res2,"t_U1res2/D");
  t1->Branch("U1res3",&t_U1res3,"t_U1res3/D");
  t1->Branch("U1res4",&t_U1res4,"t_U1res4/D");
  t1->Branch("U1res5",&t_U1res5,"t_U1res5/D");
  t1->Branch("U1res6",&t_U1res6,"t_U1res6/D");
  t1->Branch("U1res7",&t_U1res7,"t_U1res7/D");
  t1->Branch("U1res8",&t_U1res8,"t_U1res8/D");
  #endif
  #ifdef _FULLANALYSIS
  t1->Branch("nU1wiresUsed",&t_nU1wiresUsed,"t_nU1wiresUsed/I");
  t1->Branch("U1doublewires",&t_U1doublewires,"t_U1doublewires/I");
  t1->Branch("U1multiplemin",&t_U1multiplemin,"t_U1multiplemin/I");
  t1->Branch("U1wireUsed",t_U1wireUsed,"t_U1wireUsed[t_nU1wiresUsed]/I");
  #endif

  t1->Branch("X2pos",&t_X2pos,"t_X2pos/D");
  t1->Branch("X2th",&t_X2th,"t_X2th/D");
  t1->Branch("X2flag",&t_X2flag,"t_X2flag/I");
  t1->Branch("X2chisq",&t_X2chisq,"t_X2chisq/D");
  t1->Branch("X2res0",&t_X2res0,"t_X2res0/D");
  t1->Branch("X2res1",&t_X2res1,"t_X2res1/D");
  //t1->Branch("X2effID",&t_X2effID,"t_X2effID/D");
  t1->Branch("X2hits",&t_X2hits,"t_X2hits/I");
  t1->Branch("X2effall",&t_X2effall,"t_X2effall/D");
  t1->Branch("X2effdt",&t_X2effdt,"t_X2effdt/D");
  t1->Branch("X2effgroup",&t_X2effgroup,"t_X2effgroup/D");
  t1->Branch("X2effgood",&t_X2effgood,"t_X2effgood/D");
  #ifdef _VDCRESCALCS
  t1->Branch("X2res2",&t_X2res2,"t_X2res2/D");
  t1->Branch("X2res3",&t_X2res3,"t_X2res3/D");
  t1->Branch("X2res4",&t_X2res4,"t_X2res4/D");
  t1->Branch("X2res5",&t_X2res5,"t_X2res5/D");
  t1->Branch("X2res6",&t_X2res6,"t_X2res6/D");
  t1->Branch("X2res7",&t_X2res7,"t_X2res7/D");
  t1->Branch("X2res8",&t_X2res8,"t_X2res8/D");
  #endif
  #ifdef _FULLANALYSIS
  t1->Branch("nX2wiresUsed",&t_nX2wiresUsed,"t_nX2wiresUsed/I");
  t1->Branch("X2doublewires",&t_X2doublewires,"t_X2doublewires/I");
  t1->Branch("X2multiplemin",&t_X2multiplemin,"t_X2multiplemin/I");
  t1->Branch("X2wireUsed",t_X2wireUsed,"t_X2wireUsed[t_nX2wiresUsed]/I");
  #endif

  t1->Branch("U2pos",&t_U2pos,"t_U2pos/D");
  t1->Branch("U2th",&t_U2th,"t_U2th/D");
  t1->Branch("U2flag",&t_U2flag,"t_U2flag/I");
  t1->Branch("U2chisq",&t_U2chisq,"t_U2chisq/D");
  t1->Branch("U2res0",&t_U2res0,"t_U2res0/D");
  t1->Branch("U2res1",&t_U2res1,"t_U2res1/D");
  //t1->Branch("U2effID",&t_U2effID,"t_U2effID/D");
  t1->Branch("U2hits",&t_U2hits,"t_U2hits/I");
  t1->Branch("U2effall",&t_U2effall,"t_U2effall/D");
  t1->Branch("U2effdt",&t_U2effdt,"t_U2effdt/D");
  t1->Branch("U2effgroup",&t_U2effgroup,"t_U2effgroup/D");
  t1->Branch("U2effgood",&t_U2effgood,"t_U2effgood/D");
  #ifdef _VDCRESCALCS
  t1->Branch("U2res2",&t_U2res2,"t_U2res2/D");
  t1->Branch("U2res3",&t_U2res3,"t_U2res3/D");
  t1->Branch("U2res4",&t_U2res4,"t_U2res4/D");
  t1->Branch("U2res5",&t_U2res5,"t_U2res5/D");
  t1->Branch("U2res6",&t_U2res6,"t_U2res6/D");
  t1->Branch("U2res7",&t_U2res7,"t_U2res7/D");
  t1->Branch("U2res8",&t_U2res8,"t_U2res8/D");
  #endif
  #ifdef _FULLANALYSIS
  t1->Branch("nU2wiresUsed",&t_nU2wiresUsed,"t_nU2wiresUsed/I");
  t1->Branch("U2doublewires",&t_U2doublewires,"t_U2doublewires/I");
  t1->Branch("U2multiplemin",&t_U2multiplemin,"t_U2multiplemin/I");
  t1->Branch("U2wireUsed",t_U2wireUsed,"t_U2wireUsed[t_nU2wiresUsed]/I");
  #endif

  t1->Branch("ThSCAT",&t_ThSCAT,"t_ThSCAT/D");
  t1->Branch("ThFP",&t_ThFP,"t_ThFP/D");
  t1->Branch("PhiFP",&t_PhiFP,"t_PhiFP/D");
  t1->Branch("ThFPx",&t_ThFPx,"t_ThFPx/D");
  t1->Branch("Y1",&t_Y1,"t_Y1/D");
  t1->Branch("Y2",&t_Y2,"t_Y2/D");
  t1->Branch("pulser",&t_pulser,"t_pulser/I");
  t1->Branch("X1posC",&t_X1posC,"t_X1posC/D");
  t1->Branch("Ex",&t_Ex,"t_Ex/D");
  t1->Branch("T3",&t_T3,"t_T3/D");
  t1->Branch("rigidity3",&t_rigidity3,"t_rigidity3/D");
  t1->Branch("theta",&t_theta,"t_theta/D");
  t1->Branch("phi",&t_phi,"t_phi/D");

  #ifdef _FULLANALYSIS
  t1->Branch("PhiSCAT",&t_PhiSCAT,"t_PhiSCAT/D");
  #endif

  #ifdef _ADC
//   ADC = new float[32*ADCModules];
  
  t1->Branch("NaI",&t_NaI,"t_NaI[7]/D");
  t1->Branch("NaIE",&t_NaIE,"t_NaIE[7]/D");
  t1->Branch("NaIEtot",&t_NaIEtot,"t_NaIEtot/D");
  t1->Branch("SiPside1",&t_SiPside1,"t_SiPside1[16]/D");
  t1->Branch("SiPside2",&t_SiPside2,"t_SiPside2[16]/D");
  t1->Branch("SiPside3",&t_SiPside3,"t_SiPside3[16]/D");
  t1->Branch("SiPside4",&t_SiPside4,"t_SiPside4[16]/D");
  t1->Branch("SiNside1",&t_SiNside1,"t_SiNside1[16]/D");
  t1->Branch("SiNside2",&t_SiNside2,"t_SiNside2[16]/D");
  t1->Branch("SiNside3",&t_SiNside3,"t_SiNside3[16]/D");
  t1->Branch("SiNside4",&t_SiNside4,"t_SiNside4[16]/D");
  t1->Branch("NaITDC",&t_NaITDC,"t_NaITDC[7]/D");
  t1->Branch("SiPside1TDC",&t_SiPside1TDC,"t_SiPside1TDC[16]/D");
  t1->Branch("SiPside2TDC",&t_SiPside2TDC,"t_SiPside2TDC[16]/D");
  t1->Branch("SiPside3TDC",&t_SiPside3TDC,"t_SiPside3TDC[16]/D");
  t1->Branch("SiPside4TDC",&t_SiPside4TDC,"t_SiPside4TDC[16]/D");
  
  
  #endif
 
  #ifdef _STRUCKADC
  t1->Branch("StruckADC1",&t_StruckADC1,"t_StruckADC1[8]/D");
  #endif

  #ifdef _POLARIZATION
  t1->Branch("polu",&t_polu,"t_polu/I");   //PR153, polarized beam
  t1->Branch("pold",&t_pold,"t_pold/I");   //PR153, polarized beam
  #endif

  #ifdef _MOVIE   
  t1->Branch("nX1wires",&t_nX1wires,"t_nX1wires/I"); //for dynamic size of X1dt in tree
  t1->Branch("nU1wires",&t_nU1wires,"t_nU1wires/I"); //for dynamic size of U1dt in tree
  t1->Branch("nX2wires",&t_nX2wires,"t_nX2wires/I"); //for dynamic size of X2dt in tree
  t1->Branch("nU2wires",&t_nU2wires,"t_nU2wires/I"); //for dynamic size of X2dt in tree
  t1->Branch("X1dt",t_X1dt,"t_X1dt[t_nX1wires]/D");
  t1->Branch("U1dt",t_U1dt,"t_U1dt[t_nU1wires]/D");
  t1->Branch("X2dt",t_X2dt,"t_X2dt[t_nX2wires]/D");
  t1->Branch("U2dt",t_U2dt,"t_U2dt[t_nU2wires]/D");
  t1->Branch("X1wire",t_X1wire,"t_X1wire[t_nX1wires]/I");
  t1->Branch("U1wire",t_U1wire,"t_U1wire[t_nU1wires]/I");
  t1->Branch("X2wire",t_X2wire,"t_X2wire[t_nX2wires]/I");
  t1->Branch("U2wire",t_U2wire,"t_U2wire[t_nU2wires]/I");
  #endif


#ifdef _SILICONDATA
//   printf("L2108\n");
  gROOT->ProcessLine(".L Parameters.c+");
  gROOT->ProcessLine("#include \"SiliconData.h\"");
  gROOT->ProcessLine(".L SiliconData.c+");
  t1->Branch("SiliconInfo","SiliconData",&si);

  //  MMMLoadCuts(si);
#endif

#ifdef _GAMMADATA
  gROOT->ProcessLine(".L Parameters.c+");
  gROOT->ProcessLine(".L GammaData.c+");
  t1->Branch("GammaInfo","GammaData",&gammy);
#endif
  
#ifdef _RAWDATA
  gROOT->ProcessLine(".L Parameters.c+");
  gROOT->ProcessLine(".L RawData.c+");
  printf("RawData Init\n");
  t1->Branch("RawInfo","RawData",&raw);
#endif
   return SUCCESS;
}


/*-- event routine -----------------------------------------------------------------------------*/
INT focal_event(EVENT_HEADER * pheader, void *pevent)
{
//   printf("L2218\n");

  extern int TDCModules;//Number of TDC modules as declared in Parameters.c and set by the user on the config file

   DWORD *ptdc;
   Int_t ntdc = 0;
   Int_t tdc1190datacode, tdc1190error, tdc1190trailerstatus;
   Int_t tdctrigtimetag;
   Int_t channel, channelnew, time;
   Int_t tdcmodule, wire;
   Int_t ref_time, offset_time;
   Int_t *reftimes = new int[TDCModules]; 
   for(int i=0;i<TDCModules;i++)reftimes[i]=0;

   Int_t tof=0,toftdc2=0,toftdc3=0,toftdc4=0,toftdc5=0,toftdc6=0,toftdc7=0;//This should be changed at some point to be int tof[TDCModules] and then set according to the TDC module number
   //int tof[TDCmodules];
   //for(int i=0;i<TDCModules;i++)tof[i] = 0;

   Double_t resolution[10];                 // a array of numbers used in res plots
   Int_t tdcevtcount = 0;
   Int_t addwiregap=0;
   Double_t pad1hipt, pad1lowpt, pad2hipt, pad2lowpt;
   float PsideTDC[80];

   extern float NaI[8];					// defined, declared and used in adc.c  
   extern float Nside[80],Pside[80];			// defined, declared and used in adc.c  
   extern float Pside1[16],Pside2[16],Pside3[16],Pside4[16];	// defined, declared and used in adc.c  
   extern float Nside1[16],Nside2[16],Nside3[16],Nside4[16]; // defined, declared and used in adc.c  
   extern float pad1,pad2;                            // defined, declared and used in qdc.c
   extern float pad1hip,pad1lowp,pad2hip,pad2lowp;    // defined, declared and used in qdc.c
   extern int qdcevtcount;   			      // defined, declared and used in qdc.c
   extern int empty_tdc_counter;		      // defined; declared in analyzer.c	
   extern int tdc_counter;		 	      // defined; declared in analyzer.c	
   extern int toolarge_tdc_event_counter;             // defined; declared in analyzer.c	
   extern int trailer_TDCERR_counter;                 // defined; declared in analyzer.c	
   extern int trailer_triglost_counter;               // defined; declared in analyzer.c	
   extern int trailer_bufoverflow_counter;            // defined; declared in analyzer.c	
   extern int runtime;				      // defined; declared in scaler.c	
   extern int qdc_counter1;
   extern int triggerU,triggerI,CIU,CII;	      // defined; declared in scaler.c	

   //
//    float ADC_export[160];
   int *TDC_channel_export;
   float *TDC_value_export;				//Defined here. Storage structure for TDC information to be exported to be used for ancillary detectors. Filled below.

   std::vector<int> TDCChannelExportStore;
   std::vector<float> TDCValueExportStore;
   
   #ifdef _MISALIGNTIME
   if (runtime>misaligntime) {
	return SUCCESS;         // What I am trying here is to stop analysis if we have misalignment beyond time misaligntime
   }
   #endif 

   // Used in histogram hEventID
   Int_t ev_id_all=0, ev_id_noTDC=1, ev_id_pid=2, ev_id_pid_nozero=3;
   Int_t ev_id_X1_PID=5,ev_id_X1_wires=6, ev_id_X1_good=7;
   Int_t ev_id_U1_PID=9,ev_id_U1_wires=10, ev_id_U1_good=11;
   Int_t ev_id_X2_PID=13,ev_id_X2_wires=14, ev_id_X2_good=15;
   Int_t ev_id_U2_PID=17,ev_id_U2_wires=18, ev_id_U2_good=19;

   // Used in histogram hEventID, trying to figure out which wireplanes fired
   Int_t ev_id_4planes=21,  ev_id_X1X2U2=22 , ev_id_U1X2U2=23;
   Int_t ev_id_X1U1U2=24,   ev_id_X1U1X2=25;

   // Used in histogram e.g. X1_EffID etc. Help id eff problems
   Int_t ev_all=1, ev_PID=2;       
   Int_t ev_drifttime=3, ev_wiresperevent=4;
   Int_t ev_wiregap=5, ev_good=6, ev_multiplemin=10;

   // parameters in focal plane
   Int_t    X1hits = 0,   X2hits = 0,   U1hits = 0,   U2hits = 0;
   Int_t    X1hits_dt=0,  X2hits_dt=0,  U1hits_dt=0,  U2hits_dt=0;
   Double_t X1pos=-100.0, X2pos=-100.0, U1pos=-100.0, U2pos=-100.0;
   Double_t X1th=-100.0,  X2th=-100.0,  U1th=-100.0,  U2th=-100.0;
   Double_t X1chisq=15.0, X2chisq=15.0, U1chisq=15.0, U2chisq=15.0;
   Int_t    X1flag=-100,  X2flag=-100,  U1flag=-100,  U2flag=-100;
   Double_t X1eff=0,      X2eff=0,      U1eff=0,      U2eff=0;
   Double_t ThFP=-100, PhiFP=-100;  Double_t ThFPx=-100;
   Double_t ThSCAT=-100, PhiSCAT=-100;
   Double_t Y1=-100.0,Y2=-100.0;
   Double_t Xcorr=-100;
   Int_t X1wires_used=0,X2wires_used=0,U1wires_used=0,U2wires_used=0;
   Int_t X1doublewires=0,X2doublewires=0,U1doublewires=0,U2doublewires=0;
   Int_t X1multiplemin=0,X2multiplemin=0,U1multiplemin=0,U2multiplemin=0;
   Int_t X1chisqminimization=0,U1chisqminimization=0,X2chisqminimization=0,U2chisqminimization=0;

   //---------------------------------------------------------------------
   // Start analysis
   t_evtcounter=qdc_counter1;

//   Int_t PaddlePIDGatesFlagb=0;
//   if(pad1>gates.lowpad1b && pad1< gates.hipad1b && pad2>gates.lowpad2b && pad2< gates.hipad2b){   
//     PaddlePIDGatesFlagb=1;
//   }
//   if(PaddlePIDGatesFlagb==1){    //only keep analyzer busy with events that pass this rough cut

   // Before you do anything: Each RAW event goes to ch 'ev_id_all' in the EventID histogram
   hEventID->Fill(ev_id_all);  
   hPad1Pad2->Fill(pad2,pad1);  // Raw data. 
 
   Int_t PaddlePIDGatesFlag=0;
   if(pad1>gates.lowpad1 && pad1< gates.hipad1 && pad2>gates.lowpad2 && pad2< gates.hipad2){   
     PaddlePIDGatesFlag=1;
   }

   ZeroTTreeVariables();         // zero the values to be used in TTree
   ZeroFPWireTimeDist();         // zero the values of the struct X1 X2 U1 U2

   t_pad1=pad1;
   t_pad2=pad2;	       
   t_pad1hiP=pad1hip;
   t_pad1lowP=pad1lowp;	    
   t_pad2hiP=pad2hip;
   t_pad2lowP=pad2lowp;	    
   t_runtime=runtime;

   //---------------------------------------------------------------------
   //Put ADC info into TTree
   #ifdef _ADC

 
   Int_t Pside1Pedestal[16]={350,300,300,300,300,300,300,300,300,320,360,360,360,360,360,360};
   Int_t Pside2Pedestal[16]={250,280,280,280,280,280,280,280,300,220,300,300,300,300,300,300};
   Int_t Pside3Pedestal[16]={300,300,300,300,300,300,300,300,300,220,300,300,300,300,300,300};
   Int_t Pside4Pedestal[16]={300,300,300,300,300,300,300,300,300,220,300,300,300,300,300,300};

   Int_t Nside1Pedestal[16]={300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300};
   Int_t Nside2Pedestal[16]={300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300};
   Int_t Nside3Pedestal[16]={300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300};
   Int_t Nside4Pedestal[16]={300,300,300,300,300,300,300,300,300,300,300,300,300,300,300,300};

   for(int i = 0; i < 16 ; i++) { 
      if(Pside1[i]>Pside1Pedestal[i]) t_SiPside1[i]=Pside1[i];		
      if(Pside2[i]>Pside2Pedestal[i]) t_SiPside2[i]=Pside2[i];		
      if(Pside3[i]>Pside3Pedestal[i]) t_SiPside3[i]=Pside3[i];		
      if(Pside4[i]>Pside4Pedestal[i]) t_SiPside4[i]=Pside4[i];		
   }
   for(int i = 0; i < 16 ; i++) {  //funny ribbon cables to caen invert order of Si chan, but we fixed it in ECL/NIM breakout board
      if(Nside1[i]>Nside1Pedestal[i]) t_SiNside1[i]=Nside1[i];  
      if(Nside2[i]>Nside2Pedestal[i]) t_SiNside2[i]=Nside2[i];  
      if(Nside3[i]>Nside3Pedestal[i]) t_SiNside3[i]=Nside3[i];		 
      if(Nside4[i]>Nside4Pedestal[i]) t_SiNside4[i]=Nside4[i];		 
   }
   for(int i = 0; i < 8 ; i++) { 
      t_NaI[i]=NaI[i];
    }

   #endif
   

   //----------------------------------------------------------------------
   // look for TDC0 bank 
 
   ntdc = bk_locate(pevent, "TDC0", &ptdc);  // TDC bank in analyzer.c defined as TDC0. ntdc is nr of values in the bank
   //printf("nr of TDC datawords:                    %d words\n",ntdc);
   tdc_counter++;
   hTDCPerEventRaw->Fill(ntdc);    
   t_tdcsperevent=ntdc;
   //if (ntdc == 0){                      // events with no TDC data. Ignore the event
        //hEmptyTDCBankRaw->Fill(2);
   //	#ifdef _PRINTTOSCREEN
     //   printf("Event with no TDC datawords. Data ignored:  %i \n",empty_tdc_counter);
   //	#endif
     //   hEventID->Fill(ev_id_noTDC);          
   //	empty_tdc_counter++;
	//return 1;
   //}
   //else
   //{

   if(ntdc!=0)tdcevtcount=(ptdc[1]>>5)&0xfffff; // the 'evtnr' 
       //}

   // test for misaligned events in the MIDAS bank. The QDC and TDC event nr must agree. --> there are problems with QDC evtcounter from board?!
   //printf("tdc event nr from trailer:  %i\n",(ptdc[1]>>5)&0xfffff); 
   //printf("qdc event nr: %i\n",qdcevtcount);
   //if (tdcevtcount!=qdcevtcount) {
   //	printf("Event misaligment: tdc evt nr =  %i,  qdc evt nr  %i  \n",tdcevtcount,qdcevtcount);
   //	//exit(1);
   //}

   if(qdc_counter1 != tdc_counter) printf("qdc and tdc event counters out of sync: qdccounter=%i     tdccounter=%i\n",qdc_counter1,tdc_counter);

   //if (tdcevtcount==75778) {			// see run 30315. For some reason there is a seg fault when going ahead with event 75778
   //	printf("at the problematic event  \n"); 
   //	//return 1;				// here I simply ignore that event. Still do not underestand why that event is problematic...
   //}
   // test to see that the nr of tdc datawords are not too many. Since if they are, the analyzer will crash. 
   // See e.g. PR183 run 30315. If you look at the dmp file, then there are 58 tdc words for event 75778.
   // But the "ntdc = bk_locate(pevent, "TDC0", &ptdc)" yields 795.
   // So the test should actually be to see that these 2 nrs agree, and not simply if the valye of ntdc is too big.

   if (ntdc>MAX_WIRES_PER_EVENT) {      // if too many tdc words per event, ignore the event
        toolarge_tdc_event_counter++; 
	#ifdef _PRINTTOSCREEN
	printf("Large nr of TDC datawords in event# %d. Data ignored:                    %d words\n",tdcevtcount,ntdc);
	#endif
	return 1;
   }

   
   if (ntdc == 0){                      // events with no TDC data. Ignore the event
        //hEmptyTDCBankRaw->Fill(2);
	#ifdef _PRINTTOSCREEN
        printf("Event with no TDC datawords. Data ignored:  %i \n",empty_tdc_counter);
	#endif
        hEventID->Fill(ev_id_noTDC);          
	empty_tdc_counter++;
	#ifdef _TDCs
	return 1;//aborts doing data if no TDCs
	#endif
   }
  
   getRefTimes(reftimes,ntdc,ptdc);       // reftimes[i] is copy of trigger in TDC i. Each TDC should only have 1 value/event

   // loop through all the TDC datawords===================================================================================================
   //hTDCPerEvent->Fill(ntdc);          // a diagnostic: to see how many TDC channels per event 
   
   //TDC_channel_export = new int[ntdc]; //<- Declare the size of the array for the TDC data to go to any external sorts
   //TDC_value_export = new float[ntdc];
   
   for(int i = 0; i < ntdc; i++) {
      if((((ptdc[i])>>27)&0x1f)==0x1f){      // to determine TDC module nr. the frontend creates a dataword that
        tdcmodule=(ptdc[i])&0xf;             // has bits 27-31 =1. Then ch nr goes into bits 1-5.
        continue;                            // go back to beginning of for loop as this word only tells us about tdcmodule nr
      }                                      // Raw data: 1st word has TDC module nr, then info on that module, then go to next tdcmodule
      tdc1190datacode = 0x1F&((ptdc[i])>>27);

      #ifdef _PRINTTOSCREEN
      if((((ptdc[i])>>27)&0x1f)==4){      // to determine TDC error, if 'error word' writing is enabled
        tdc1190error=(ptdc[i])&0x7fff;    // first 5 bits are 00100
        printf("--->  tdc error %i at runtime= %i, tdc module %i    ",tdc1190error,runtime,tdcmodule); 
	switch(tdc1190error){
		case 0: printf("Error code = Hit lost in group 0 from readout FIFO overflow\n "); break;
		case 1: printf("Error code = Hit lost in group 0 from L1 buffer overflow\n "); break;
		case 2: printf("Error code = Hit error have been detected in group 0 \n"); break;
		case 3: printf("Error code = Hit lost in group 1 from read-out FIFO overflow \n"); break;
		case 4: printf("Error code = Hit lost in group 1 from L1 buffer overflow \n"); break;
		case 5: printf("Error code = Hit error have been detected in group 1. \n"); break;
		case 6: printf("Error code = Hit data lost in group 2 from read-out FIFO overflow. \n"); break;
		case 7: printf("Error code = Hit lost in group 2 from L1 buffer overflow \n"); break;
		case 8: printf("Error code = Hit error have been detected in group 2. \n"); break;
		case 9: printf("Error code = Hit lost in group 3 from read-out FIFO overflow. \n"); break;
		case 10: printf("Error code = Hit lost in group 3 from L1 buffer overflow \n"); break;
		case 11: printf("Error code = Hit error have been detected in group 3.\n"); break;
		case 12: printf("Error code = Hits rejected because of programmed event size limit \n"); break;
		case 13: printf("Error code = Event lost (trigger FIFO overflow) \n"); break;
		case 14: printf("Error code = Internal fatal chip error has been detected \n"); break;
	}
        continue;                         
      }  
      if((((ptdc[i])>>27)&0x1f)==0x11){     // to get Extended Trigger Time Tag, if activated
	tdctrigtimetag=(ptdc[i]&0x7ffffff);
        printf("--->  tdc data word %i , tdc trigger time tag %i , tdc module %i   \n",i,tdctrigtimetag,tdcmodule);
      }
      #endif

      if((((ptdc[i])>>27)&0x1f)==0x10){     
 	tdc1190trailerstatus=(0x7&((ptdc[i])>>24));
        //printf("--->  tdc trailer status %i at runtime= %i, tdc module %i   \n",tdc1190trailerstatus,runtime,tdcmodule); 
	switch(tdc1190trailerstatus){
		case 4: //printf("tdc trailer status: TRIGGER LOST (at least 1 trigger->event lost) at runtime= %i, tdc evt nr=%i, tdc module %i \n ",runtime,tdcevtcount,tdcmodule); 
			trailer_triglost_counter++; break;
		case 2: //printf("tdc trailer status: OUTPUT BUFFER OVERFLOW (buffer overflow, possible data loss) at runtime= %i, tdc evt nr=%i, tdc module %i \n",runtime,tdcevtcount,tdcmodule); 
			trailer_bufoverflow_counter++; break;
		case 1: //printf("tdc trailer status: TDC ERROR (at least 1 TDC chip in error) at runtime= %i, tdc evt nr=%i, tdc module %i \n ",runtime,tdcevtcount,tdcmodule); 
			trailer_TDCERR_counter++; break;
	}
      }
  
      if(tdc1190datacode!=0) continue;       //ignore rest of for loop if we only have header/trailer word, BUT for valid measurent word, continue
      channel = (0x7F&((ptdc[i])>>19));      // channel is per TDC; i.e. 0-127
      time = 0x7FFFF&(ptdc[i]);

      if(tdcmodule<0 || tdcmodule>7){
	printf("bad tdc module nr %d\n",tdcmodule);   // error condition
      }

      
      
      ref_time = reftimes[tdcmodule] - time;     // to get accurate time info that does not suffer from trigger jitter
      
      if(tdcmodule<8){
	hTDC2DModule[tdcmodule]->Fill(ref_time,channel);
	
	
	
#ifdef _ADC
	if(tdcmodule==6){
	  if(channel>63 && channel<80){
	    t_SiPside1TDC[channel-64]=ref_time;
	  }
	  if(channel>79 && channel<96){
	    t_SiPside2TDC[channel-80]=ref_time;
	  }
	  if(channel>95 && channel<112){
	    t_SiPside3TDC[channel-96]=ref_time;
	  }
	  if(channel>111 && channel<128){
	    t_SiPside4TDC[channel-112]=ref_time;
	  }
	}
	if(tdcmodule==5){
	  if(channel>24 && channel<31){
	    t_NaITDC[channel-25]=ref_time;
	  }
	}
#endif
      }

      channel = channel+tdcmodule*128;                     // convert channel nr to nr in range: 0-(nr_of_tdcs)*128
      
      offset_time = ref_time - int(cableOffset[channel]);  // in CableLength.dat: line nr = y bin nr in hChanVsOffsetTime

      //printf("ntdc: %d \t tdc_counter: %d \t channel: %d \t value: %d \n",ntdc,tdc_counter,channel,offset_time);
      //TDC_channel_export[i] = channel;
      //TDC_value_export[i] = offset_time;
      TDCChannelExportStore.push_back(channel);
      TDCValueExportStore.push_back(offset_time);

      switch(channel){
		case 3:  t_k600=1; break;
		case 9:  pad1hipt=ref_time;t_pad1hiPT=pad1hipt; break;
		case 10: pad1lowpt=ref_time;t_pad1lowPT=pad1lowpt; break;
		case 11: pad2hipt=ref_time; t_pad2hiPT=pad2hipt;break;
		case 12: pad2lowpt=ref_time; t_pad2lowPT=pad2lowpt;break;
		case TOF_TDC_CHAN: if(t_tof==0) {tof=ref_time; t_tof=tof;} break;  // this ensures only the 1st signal, not last of multiple hits, gets digitized
		case (TOF_TDC_CHAN+1*128): if(t_toftdc2==0) toftdc2=ref_time; t_toftdc2=toftdc2; break;
		case (TOF_TDC_CHAN+2*128): if(t_toftdc3==0) toftdc3=ref_time; t_toftdc3=toftdc3; break;
		case (TOF_TDC_CHAN+3*128): if(t_toftdc4==0) toftdc4=ref_time; t_toftdc4=toftdc4; break;
		case (TOF_TDC_CHAN+4*128): if(t_toftdc5==0) toftdc5=ref_time; t_toftdc5=toftdc5; break;
		case (TOF_TDC_CHAN+5*128): if(t_toftdc6==0) toftdc6=ref_time; t_toftdc6=toftdc6; break;
		case (TOF_TDC_CHAN+6*128): if(t_toftdc7==0) toftdc7=ref_time; t_toftdc7=toftdc7; break;
		case (TDC_CHAN_PULSER): t_pulser=1; break; 
		#ifdef _POLARIZATION  		
		case (TDC_CHAN_POLU): t_polu=1; break;  // for polarized beam
		case (TDC_CHAN_POLD): t_pold=1; break;  // for polarized beam
		#endif
      }

      hHitPatternRawTDC->Fill(channel);            
      hChanVsTimeRef->Fill(ref_time,channel);        // 2D: chan vs reference times
      hChanVsTimeOffset->Fill(offset_time,channel);  // 2D: chan vs offset corrected times

      #ifdef _JJAUTOTRIM  
      if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){		// if you want to use JJ_autotrim.C
         hTDC_REF[channel]->Fill(ref_time);                                     // fill spectra for individual wires   	
      }											
      #endif

      //continue;  // If you analyze the pulser-only data uncomment this line otherwise you have a segmentation fault
 
      // NB I prefer to do the offset correction with this 'channel' that is physically given in the
      // order of the TDC channels, as different cable lenghts and other physical causes for
      // different time measurements should be handled on a physical basis 
      // as in TDC per TDC, and when channel is converted into the correct drift wire order everything becomes more complicated

      // Up to this point "channel" is a number from 0-(nr_of_tdcs)*128. E.g for 6 TDCs it is 0-767.
      // And if TOF signal goes to ch 1 of 1st TDC then it is in chan 1.
      // Now we convert these TDC-related channel numbers connected to wireplanes
      // X1 wires 1-198 = channel nrs 1-198
      // U1 wires 1-143 = channel nrs 301-444
      // X2 wires 1-198 = channel nrs 501-698
      // U2 wires 1-143 = channel nrs 801-944
      // Later on these new channel numbers are converted to real wire numbers

      channelnew=Channel2Wire[channel];

      //for(int i=0;i<300;i++){
      //     printf("chan2wire[%d] = %d   \n",i, Channel2Wire[i]);
      //}

      if(channelnew<0) continue;     // all reftimes and TOF were placed in ch -1 in channel2wire conversion. They are not to be plotted.
      
      hHitPatternAll->Fill(channelnew);            
      #ifdef _FULLANALYSIS
      hDriftTimeRawAll->Fill(time);	                // raw times
      hDriftTimeOffsetAll->Fill(offset_time);	                // reference times
      #endif
      hWireVsTimeOffset->Fill(offset_time,channelnew);  // ref and offset corrected times
      if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	hHitPatternPID->Fill(channelnew);
	hChanVsTimeOffsetPID->Fill(offset_time,channel);  // ref and offset corrected times
	hWireVsTimeOffsetPID->Fill(offset_time,channelnew);  // ref and offset corrected times
      }

      //---------  Now create structs X1,U1,X2,U2 --------
      // I want everything that goes into the TTree NOT to be affected by the PID gates set in the ODB.
      // But there are things that goes into the histograms that I want to be for PID gated events only.
      // Hence the lot of PID gate tests.
	
      if((channelnew >= globals.x1_1st_wire_chan) && (channelnew < globals.x1_last_wire_chan)  ){         
        //if(channelnew==111 || channelnew==113){  //PR167 WE3; X1 ch 113 is bad, so ignore it in analysis
	//  addwiregap=1;
        //  //printf("addwiregap %i \n",addwiregap);
        //}
	//if(channelnew >= globals.x1_1st_wire_chan+35) t_X1effall=1;         // SPECIFIC for ZERO DEGREE EXPERIMENT PR183/PR184
	t_X1effall=1;         
	#ifdef _FULLANALYSIS
	if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	  hX1_DriftTimeRef->Fill(ref_time);	
	  hX1_DriftTimeOffset->Fill(offset_time);	
	}
	#endif
	wire = channelnew-globals.x1_1st_wire_chan +1;    	
	#ifdef _MOVIE
	t_X1dt[X1hits]=offset_time;                                 
	t_X1wire[X1hits]=wire;
	#endif
	X1hits++;
	if((offset_time >= gates.x1_driftt_low) && (offset_time <= gates.x1_driftt_hi)){          //drifttime gate       
	  t_X1effdt=1; 
	  X1.wire[X1hits_dt]=wire;
	  X1.time[X1hits_dt]=offset_time;
          X1hits_dt++;
	}
      }
      else if ((channelnew >= globals.u1_1st_wire_chan) && (channelnew < globals.u1_last_wire_chan)) {    //only for U1 wireplane
	//if (channelnew >= globals.u1_1st_wire_chan+27) t_U1effall=1;         // SPECIFIC for ZERO DEGREE EXPERIMENT PR183/PR184
	t_U1effall=1;        
	#ifdef _FULLANALYSIS
	if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	  hU1_DriftTimeRef->Fill(ref_time);	
	  hU1_DriftTimeOffset->Fill(offset_time);	
	}
  	#endif
	wire = channelnew-globals.u1_1st_wire_chan +1;               
	#ifdef _MOVIE
	t_U1dt[U1hits]=offset_time;                                 
	t_U1wire[U1hits]=wire;
	#endif
	U1hits++;
	if((offset_time >= gates.u1_driftt_low) && (offset_time <= gates.u1_driftt_hi)){            //drifttime gate 
	  t_U1effdt=1; 
	  U1.wire[U1hits_dt]=wire;
	  U1.time[U1hits_dt]=offset_time;
	  U1hits_dt++;
	}
      }
      //else if ((channelnew >= globals.x2_1st_wire_chan) && (channelnew < globals.x2_last_wire_chan) && (channelnew!=482) ) {   //only for X2 wireplane
      else if ((channelnew >= globals.x2_1st_wire_chan) && (channelnew < globals.x2_last_wire_chan)) {   //only for X2 wireplane
						// chan 482 looks suspicious in white tune run 23088
	//if(channelnew >= globals.x2_1st_wire_chan+15) t_X2effall=1; 	// SPECIFIC for ZERO DEGREE EXPERIMENT PR183/PR184
	t_X2effall=1; 	
	#ifdef _FULLANALYSIS
	if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	  hX2_DriftTimeRef->Fill(ref_time);	
	  hX2_DriftTimeOffset->Fill(offset_time);	
	}
  	#endif
	wire = channelnew-globals.x2_1st_wire_chan +1;               
	#ifdef _MOVIE
	t_X2dt[X2hits]=offset_time;                                 
	t_X2wire[X2hits]=wire;
	#endif
	X2hits++;
	if((offset_time >= gates.x2_driftt_low) && (offset_time <= gates.x2_driftt_hi)){            //drifttime gate 
	  t_X2effdt=1; 
	  X2.wire[X2hits_dt]=wire;
	  X2.time[X2hits_dt]=offset_time;
	  X2hits_dt++;
	}
      }    
      else if ((channelnew >= globals.u2_1st_wire_chan) && (channelnew < globals.u2_last_wire_chan)) {    //only for U2 wireplane 
	//if(channelnew >= globals.u2_1st_wire_chan+15) t_U2effall=1;  // SPECIFIC for ZERO DEGREE EXPERIMENT PR183/PR184
	t_U2effall=1;  
	#ifdef _FULLANALYSIS
	if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	  hU2_DriftTimeRef->Fill(ref_time);	
	  hU2_DriftTimeOffset->Fill(offset_time);	
	}
	#endif
	wire = channelnew-globals.u2_1st_wire_chan +1;               
	#ifdef _MOVIE
	t_U2dt[U2hits]=offset_time;                                 
	t_U2wire[U2hits]=wire;
	#endif
	U2hits++;
	if((offset_time >= gates.u2_driftt_low) && (offset_time <= gates.u2_driftt_hi)){            //drifttime gate 
	  t_U2effdt=1; 
	  U2.wire[U2hits_dt]=wire;
	  U2.time[U2hits_dt]=offset_time;
	  U2hits_dt++;
	}
      }
   } // end of loop over all the TDC words=======================================================================================


   //--------------------------------------------------------------------------------------------------------
   // Now fill a few histograms that aids in detector efficiency optimization and event ID
   //--------------------------------------------------------------------------------------------------------
   if(X1hits!=0) hX1_EffID->Fill(ev_all);    // to see if any wire at all of X1 fired
   if(U1hits!=0) hU1_EffID->Fill(ev_all);
   if(X2hits!=0) hX2_EffID->Fill(ev_all);
   if(U2hits!=0) hU2_EffID->Fill(ev_all);
   
   if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){   
      hPad1Pad2G->Fill(pad2,pad1);
      hPad1VsTofG->Fill(tof,pad1);

      hEventID->Fill(ev_id_pid);           // all trigger bank events that pass through PID gates
      if(X1hits>1 && U1hits>1 && X2hits>1 && U2hits>1){
	hEventID->Fill(ev_id_pid_nozero);  // all trigger bank events that pass through PID gate+ at least 2 wires per chamber fired
      }

      #ifdef _FULLANALYSIS
      if(X1hits>1) hX1_Hits->Fill(X1hits); 
      if(U1hits>1) hU1_Hits->Fill(U1hits); 
      if(X2hits>1) hX2_Hits->Fill(X2hits); 
      if(U2hits>1) hU2_Hits->Fill(U2hits); 
      #endif

      if(X1hits>1) hX1_EffID->Fill(ev_PID);  // all X1 events that pass through PID gates
      if(U1hits>1) hU1_EffID->Fill(ev_PID);  // the >1 conditions excludes 1 wire events from 'ALL' events in _Eff calculation
      if(X2hits>1) hX2_EffID->Fill(ev_PID);
      if(U2hits>1) hU2_EffID->Fill(ev_PID);

      if(X1hits>1) hEventID->Fill(ev_id_X1_PID);   // the >1 conditions excludes 1 wire events from 'ALL' events in eff calculation
      if(U1hits>1) hEventID->Fill(ev_id_U1_PID);
      if(X2hits>1) hEventID->Fill(ev_id_X2_PID);
      if(U2hits>1) hEventID->Fill(ev_id_U2_PID);

      if(X1hits_dt>1) hX1_EffID->Fill(ev_drifttime); 
      if(U1hits_dt>1) hU1_EffID->Fill(ev_drifttime);
      if(X2hits_dt>1) hX2_EffID->Fill(ev_drifttime);  // If I test only !=0 then in EffID this can be bigger than just PID values
      if(U2hits_dt>1) hU2_EffID->Fill(ev_drifttime);

      //trying to establish which chambers fired in coincidence
      //NOTE this is not for good events, only PID selected and events with some decent drifttimes
      if(X1hits_dt>1 && U1hits_dt>1 && X2hits_dt>1 && U2hits_dt>1) hEventID->Fill(ev_id_4planes); // 4 fired 
      if(X1hits_dt>1 && X2hits_dt>1 && U2hits_dt>1 ) hEventID->Fill(ev_id_X1X2U2);                // 3 fired X1 X2 U2 
      if(U1hits_dt>1 && X2hits_dt>1 && U2hits_dt>1 ) hEventID->Fill(ev_id_U1X2U2);                // 3 fired U1 X2 U2 
      if(X1hits_dt>1 && U1hits_dt>1 && U2hits_dt>1 ) hEventID->Fill(ev_id_X1U1U2);                // 3 fired X1 U1 U2 
      if(X1hits_dt>1 && U1hits_dt>1 && X2hits_dt>1 ) hEventID->Fill(ev_id_X1U1X2);                // 3 fired X1 U1 X2 

   }    //end loop for PID gates

   //--------------------------------------------------------------------------------------------------------
   // We've gone through all the TDC words associated with this event we obtained data for the 4
   // structures X1,U1,X2 and U2. Now for position and angle determination...
   //--------------------------------------------------------------------------------------------------------

   sortTDCs(X1hits_dt, X1.wire, X1.time);     //Bubble sort so that order of wireplane.wire[]
   sortTDCs(X2hits_dt, X2.wire, X2.time);     //is from lowest wire to highest wire. 
   sortTDCs(U1hits_dt, U1.wire, U1.time);     //It makes things easier if the arrays are ordered.
   sortTDCs(U2hits_dt, U2.wire, U2.time);

   //printevent(X1hits_dt, X1.wire, X1.time);     

   Int_t wrangeX1 = 1 + X1.wire[X1hits_dt-1]-X1.wire[0];  // wire range 
   Int_t wrangeX2 = 1 + X2.wire[X2hits_dt-1]-X2.wire[0];
   Int_t wrangeU1 = 1 + U1.wire[U1hits_dt-1]-U1.wire[0];
   Int_t wrangeU2 = 1 + U2.wire[U2hits_dt-1]-U2.wire[0];

   //DriftLength spectra fill
   Int_t drifttime=0;
   for(int i = 0; i < X1hits_dt ; i++) { 
     drifttime=X1.time[i];
     X1.dist[i]=lutx1[drifttime]*DRIFTLENGTH;     //X1.dist is driftlength in actual mm
     hX1_DriftLength->Fill(X1.dist[i]);
   }
   for(int i = 0; i < X2hits_dt ; i++) { 
     drifttime=X2.time[i];
     X2.dist[i]=lutx2[drifttime]*DRIFTLENGTH;
     hX2_DriftLength->Fill(X2.dist[i]);
   }
   for(int i = 0; i < U1hits_dt ; i++) { 
     drifttime=U1.time[i];
     U1.dist[i]=lutu1[drifttime]*DRIFTLENGTH;
     hU1_DriftLength->Fill(U1.dist[i]);
   }
   for(int i = 0; i < U2hits_dt ; i++) { 
     drifttime=U2.time[i];
     U2.dist[i]=lutu2[drifttime]*DRIFTLENGTH;
     hU2_DriftLength->Fill(U2.dist[i]);
   }
   

   //Gates on number of wires, number of missing wires etc
   if(X1hits_dt>=globals.min_x_wires  &&  X1hits_dt<globals.max_x_wires+1){
     if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1)  hX1_EffID->Fill(ev_wiresperevent);
     if((globals.misswires+addwiregap)>(wrangeX1-X1hits_dt)){
       hEventID->Fill(ev_id_X1_wires);  // events in X1 that pass through wire requirement gates 
       t_X1effgroup=1; 
       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1) hX1_EffID->Fill(ev_wiregap);
       #ifdef _FULLANALYSIS
       t_X1TimeDiff=(X1.time[0]-X1.time[1])-(X1.time[4]-X1.time[3]); // See Hall A paper manuscript: NIMA 474 (2001) 108 fig13
       #endif
       //raytrace(X1.dist, X1.wire, &X1pos, &X1th, &X1chisq, X1hits_dt, resolution, &X1flag,1, &X1wires_used, &X1doublewires, &X1multiplemin); 

       raytrace(X1.dist, X1.wire, &X1pos, &X1th, &X1chisq, X1hits_dt, resolution, &X1flag,1, X1wirefit, X1distfit, &X1wires_used, &X1doublewires, &X1multiplemin, &X1chisqminimization); 

       //for(int i = 0; i<X1wires_used; i++){
       //  printf("evt nr=%i : i=%i,  wire(i)=%i : X1hits_dt=%i : X1wires_used %i \n",tdc_counter,i,X1.wire[i],X1hits_dt,X1wires_used);
       //}
       if(X1flag==0) t_X1effgood=1;
       // Remember: X1hits_dt goes into raytrace as a fixed nr. But after raytrace this does not
       // mean that we used these nr of wires for fitting. Choices in raytrace could have used only 2-3wires less.

       t_X1pos=X1pos - x1offset;         //for current clumsy implementation of TTree. for good events: must plot with X1flag=0!!!!!!!!
       t_X1th=X1th;           //global scope.
       t_X1flag=X1flag;
       t_X1chisq=X1chisq;
       t_X1res0=resolution[0];
       t_X1res1=resolution[1];
       #ifdef _VDCRESCALCS
       t_X1res2=resolution[2];
       t_X1res3=resolution[3];
       t_X1res4=resolution[4];
       t_X1res5=resolution[5];
       t_X1res6=resolution[6];
       t_X1res7=resolution[7];
       t_X1res8=resolution[8];
       #endif
       t_X1hits=X1hits_dt; 
       #ifdef _FULLANALYSIS
       t_nX1wiresUsed=X1wires_used; 
       t_X1doublewires=X1doublewires; 
       t_X1multiplemin=X1multiplemin; 
       for(int i = 0; i<X1wires_used; i++){
         t_X1wireUsed[i]=X1.wire[i];
         t_X1distUsed[i]=X1.dist[i];
         //printf("evt nr=%i : i=%i,  wire(i)=%i : X1hits_dt=%i : X1wires_used %i \n",tdc_counter,i,X1.wire[i],X1hits_dt,X1wires_used);
       }
       //printf("\n");
       #endif

       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){   //This to allow sensible histos to be filled
	 if (X1flag!=0){
	   hX1_EffID->Fill(ev_multiplemin);
	 }
	 else if(X1flag==0){	  
	   hX1_EffID->Fill(ev_good);
	   hEventID->Fill(ev_id_X1_good);  // good X1 events 
	   for(int i = 0; i < X1hits_dt ; i++) { 
		//if(X1pos>0 && X1pos<249){
	     hX1_DriftTimeGood->Fill(X1.time[i]);	
                //}
	   }
	   #ifdef _FULLANALYSIS
	   hX1_Chisq->Fill(X1chisq);
	   hX1_HitsG->Fill(X1hits_dt); 		
	   hX1_Res->Fill(resolution[6]); 
	   hX1_Res2dRCNP->   Fill(resolution[5],resolution[8]);
	   hX1_Res2diTL->Fill(resolution[0],resolution[1]);
	   #endif
	 }	 
       }
     }
   }  

   if(U1hits_dt>=globals.min_u_wires  &&  U1hits_dt<globals.max_u_wires){   
     if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1)	 hU1_EffID->Fill(ev_wiresperevent);
     if(globals.misswires>(wrangeU1-U1hits_dt)){
       hEventID->Fill(ev_id_U1_wires);  // events in U1 that pass through the wire requirement gates 
       t_U1effgroup=1; 
       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1)  hU1_EffID->Fill(ev_wiregap);

       raytrace(U1.dist, U1.wire, &U1pos, &U1th, &U1chisq, U1hits_dt, resolution, &U1flag,2, U1wirefit, U1distfit,&U1wires_used, &U1doublewires, &U1multiplemin, &U1chisqminimization); 

       //raytrace(U1.dist, U1.wire, &U1pos, &U1th, &U1chisq, U1hits_dt, resolution, &U1flag,2,&U1wires_used, &U1doublewires, &U1multiplemin); 
       if(U1flag==0) t_U1effgood=1;

       //U1pos=U1pos/sin(U_WIRE_ANGLE/57.2957);  // since wires at 50degr to horizontal, they are 1/sin(50degr) further 
                                               // apart in x, so U1pos is in terms of real pos along the wires, in mm

       t_U1pos=U1pos;         //for current clumsy implementation of TTree. I get problems if I move U1pos etc to
       t_U1th=U1th;           //global scope.
       t_U1flag=U1flag;
       t_U1chisq=U1chisq;
       t_U1res0=resolution[0];	  
       t_U1res1=resolution[1];
       #ifdef _VDCRESCALCS
       t_U1res2=resolution[2];
       t_U1res3=resolution[3];
       t_U1res4=resolution[4];
       t_U1res5=resolution[5];
       t_U1res6=resolution[6];
       t_U1res7=resolution[7];
       t_U1res8=resolution[8];
       #endif
       t_U1hits=U1hits_dt;
       #ifdef _FULLANALYSIS
       t_nU1wiresUsed=U1wires_used; 
       t_U1doublewires=U1doublewires; 
       t_U1multiplemin=U1multiplemin; 
       for(int i = 0; i<U1wires_used; i++){
         t_U1wireUsed[i]=U1.wire[i];
       }
       #endif

       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	 if (U1flag!=0){
         }
	 else if(U1flag==0){
	   hU1_EffID->Fill(ev_good);
	   hEventID->Fill(ev_id_U1_good);  // good U1 events 
	   for(int i = 0; i < U1hits_dt ; i++) { 
	     hU1_DriftTimeGood->Fill(U1.time[i]);	
	   }
	   #ifdef _FULLANALYSIS
	   hU1_Chisq->Fill(U1chisq);
	   hU1_HitsG->Fill(U1hits_dt); 
	   hU1_Res->Fill(resolution[6]);
	   hU1_Res2dRCNP->Fill(resolution[5],resolution[8]);
	   hU1_Res2diTL->Fill(resolution[0],resolution[1]);
	   #endif
	 }	 
       }
     }
   }

   if(X2hits_dt>=globals.min_x_wires  &&  X2hits_dt<globals.max_x_wires){
     if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1) hX2_EffID->Fill(ev_wiresperevent);
     if(globals.misswires>(wrangeX2-X2hits_dt)){
       hEventID->Fill(ev_id_X2_wires);  // events in X2 that pass through wire requirement gates 
       t_X2effgroup=1; 
       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1)	 hX2_EffID->Fill(ev_wiregap);
      
       raytrace(X2.dist, X2.wire, &X2pos, &X2th, &X2chisq, X2hits_dt, resolution, &X2flag,3, X2wirefit, X2distfit,&X2wires_used, &X2doublewires, &X2multiplemin, &X2chisqminimization); 
       //raytrace(X2.dist, X2.wire, &X2pos, &X2th, &X2chisq, X2hits_dt, resolution, &X2flag,3,&X2wires_used, &X2doublewires, &X2multiplemin); 
       if(X2flag==0) t_X2effgood=1;

       t_X2pos=X2pos;         //for current clumsy implementation of TTree. I get problems if I move X2pos etc to
       t_X2th=X2th;           //global scope.
       t_X2flag=X2flag;
       t_X2chisq=X2chisq;
       t_X2res0=resolution[0];	    
       t_X2res1=resolution[1];
       #ifdef _VDCRESCALCS
       t_X2res2=resolution[2];
       t_X2res3=resolution[3];
       t_X2res4=resolution[4];
       t_X2res5=resolution[5];
       t_X2res6=resolution[6];
       t_X2res7=resolution[7];
       t_X2res8=resolution[8];
       #endif
       t_X2hits=X2hits_dt;
       #ifdef _FULLANALYSIS
       t_nX2wiresUsed=X2wires_used; 
       t_X2doublewires=X2doublewires; 
       t_X2multiplemin=X2multiplemin; 
       for(int i = 0; i<X2wires_used; i++){
         t_X2wireUsed[i]=X2.wire[i];
       }
       #endif

       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	 if (X2flag!=0){
	   hX2_EffID->Fill(ev_multiplemin);
	 }
	 else if(X2flag==0){
	   hX2_EffID->Fill(ev_good);
	   hEventID->Fill(ev_id_X2_good);  // good X2 events 
	   for(int i = 0; i < X2hits_dt ; i++) { 
	     hX2_DriftTimeGood->Fill(X2.time[i]);	
	   }
	   #ifdef _FULLANALYSIS
	   hX2_Chisq->Fill(X2chisq);
	   hX2_HitsG->Fill(X2hits_dt);
	   hX2_Res->Fill(resolution[6]); 
	   hX2_Res2dRCNP->Fill(resolution[5],resolution[8]);
	   hX2_Res2diTL->Fill(resolution[0],resolution[1]);
	   #endif
	 }
       }  
     }
   }
      
   if(U2hits_dt>=globals.min_u_wires  &&  U2hits_dt<globals.max_u_wires){    
     if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1) hU2_EffID->Fill(ev_wiresperevent);
     if(globals.misswires>(wrangeU2-U2hits_dt)){
       hEventID->Fill(ev_id_U2_wires);  // events in U1 that pass through the wire requirement gates 
       t_U2effgroup=1; 
       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1)  hU2_EffID->Fill(ev_wiregap);

       raytrace(U2.dist, U2.wire, &U2pos, &U2th, &U2chisq, U2hits_dt, resolution, &U2flag,4, U2wirefit, U2distfit,&U2wires_used, &U2doublewires, &U2multiplemin, &U2chisqminimization); 
       //raytrace(U2.dist, U2.wire, &U2pos, &U2th, &U2chisq, U2hits_dt, resolution, &U2flag,4,&U2wires_used, &U2doublewires, &U2multiplemin); 
       if(U2flag==0) t_U2effgood=1;

       //U2pos=U2pos/sin(U_WIRE_ANGLE/57.2957);  //since wires at 50deg to horiz,they are 1/sin(50degr) further apart in x
       
       t_U2pos=U2pos;         //for current clumsy implementation of TTree. I get problems if I move U2pos etc to
       t_U2th=U2th;           //global scope.
       t_U2flag=U2flag;
       t_U2chisq=U2chisq;
       t_U2res0=resolution[0];	    
       t_U2res1=resolution[1];
       #ifdef _VDCRESCALCS
       t_U2res2=resolution[2];
       t_U2res3=resolution[3];
       t_U2res4=resolution[4];
       t_U2res5=resolution[5];
       t_U2res6=resolution[6];
       t_U2res7=resolution[7];
       t_U2res8=resolution[8];
       #endif
       t_U2hits=U2hits_dt;
       #ifdef _FULLANALYSIS
       t_nU2wiresUsed=U2wires_used; 
       t_U2doublewires=U2doublewires; 
       t_U2multiplemin=U2multiplemin; 
       for(int i = 0; i<U2wires_used; i++){
         t_U2wireUsed[i]=U2.wire[i];
       }
       #endif

       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1){
	 if (U2flag!=0){
	 }
	 else if(U2flag==0){
	   hU2_EffID->Fill(ev_good);
	   hEventID->Fill(ev_id_U2_good);  // good U2 events 	 
	   for(int i = 0; i < U2hits_dt ; i++) { 
	     hU2_DriftTimeGood->Fill(U2.time[i]);	
	   }
	   #ifdef _FULLANALYSIS
	   hU2_Chisq->Fill(U2chisq);
	   hU2_HitsG->Fill(U2hits_dt);
	   hU2_Res->Fill(resolution[6]);
	   hU2_Res2dRCNP->Fill(resolution[5],resolution[8]);
	   hU2_Res2diTL->Fill(resolution[0],resolution[1]);
	   #endif
	 }
       }  
     }
   }   

   //--------------------------------------------------------------------------------------------------------
   // Now calculate and fill spectra for calculated angles using 2 driftchambers, and calculate Ypos
   // Note that if X1flag==0 then the event passed all gates: pid, dt, group. It is for good events only
   //--------------------------------------------------------------------------------------------------------
   CalcThetaFP(X1pos,X2pos,&ThFPx);  
   CalcThetaFP(U1pos,U2pos,&ThFP);  
   CalcThetaScat(ThFPx,X1pos,&ThSCAT);
   t_ThFP=ThFP;
   t_ThSCAT=ThSCAT;
   t_ThFPx=ThFPx;

   CalcYFP(X1pos,U1pos,X1th,&Y1);  
   //CalcYFP(X1pos,U2pos,ThFP,&Y1);  
   t_Y1=Y1;
   #ifdef _FULLANALYSIS
   h_Y1->Fill(Y1);
   CalcPhiScat(Y1,X1pos,ThSCAT,&PhiSCAT);
   t_PhiSCAT=PhiSCAT;
   #endif

   CalcYFP(X2pos,U2pos,ThFP,&Y2);  // I get funny double locus if I use calc theta // changed by AT to be used 
   t_Y2=Y2;
   #ifdef _FULLANALYSIS
   h_Y2->Fill(Y2);
   #endif

   CalcPhiFP(X1pos,Y1,X2pos,Y2,ThFP,&PhiFP);
   t_PhiFP=PhiFP;

   //CalcCorrX(X1pos-x1offset, (Y2+10), ThSCAT, &Xcorr);
   CalcCorrX(X1pos-x1offset, Y1, ThSCAT, &Xcorr);
   t_X1posC=Xcorr;

   t_Ex = CalcExDirect(Xcorr);


   extern double *masses;

   t_T3 = CalcTfromXcorr(Xcorr, masses[2]);

   t_rigidity3 = CalcQBrho(Xcorr);

   t_theta = CalcThetaPrime(Xcorr,ThFPx);
   t_phi = CalcPhiPrime(Xcorr,ThFPx,Y1);

   //--------------------------------------------------------------------------------------------------------
   // Calculate and plot wirechamber efficiencies
   //--------------------------------------------------------------------------------------------------------
   #ifdef _FULLANALYSIS
   if( hEventID->GetBinContent(ev_id_X1_PID+1) !=0){
     X1eff=100.*float(hEventID->GetBinContent(ev_id_X1_good+1)) / float(hEventID->GetBinContent(ev_id_X1_PID+1));
     hX1_Eff->Fill(X1eff);
     hX1_PosEff->Fill(X1pos,X1eff);
   }
   if( hEventID->GetBinContent(ev_id_U1_PID+1) !=0){
     U1eff=100.*float(hEventID->GetBinContent(ev_id_U1_good+1)) / float(hEventID->GetBinContent(ev_id_U1_PID+1));
     hU1_Eff->Fill(U1eff);
   }
   if( hEventID->GetBinContent(ev_id_X2_PID+1) !=0){
     X2eff=100.*float(hEventID->GetBinContent(ev_id_X2_good+1)) / float(hEventID->GetBinContent(ev_id_X2_PID+1));
     hX2_Eff->Fill(X2eff);
   }
   if( hEventID->GetBinContent(ev_id_U2_PID+1) !=0){
     U2eff=100.*float(hEventID->GetBinContent(ev_id_U2_good+1)) / float(hEventID->GetBinContent(ev_id_U2_PID+1));
     hU2_Eff->Fill(U2eff);
   }
   #endif

   if(X1flag==0 && U1flag==0 && X2flag==0 && U2flag==0){
     hEventID2->Fill(ev_id_4planes);
   }
   if(U1flag==0 && X2flag==0 && U2flag==0){
     hEventID2->Fill(ev_id_U1X2U2);
   }
   if(X1flag==0 && X2flag==0 && U2flag==0){
     hEventID2->Fill(ev_id_X1X2U2);
   }
   if(X1flag==0 && U1flag==0 && U2flag==0){
     hEventID2->Fill(ev_id_X1U1U2);
   }
   if(X1flag==0 && U1flag==0 && X2flag==0){
     hEventID2->Fill(ev_id_X1U1X2);
   }

   	////// SIS0 gubbins ///////
   #ifdef _STRUCKADC
		INT i, nSISwords;

//  each fired channel of the ADC generates a 6 words sentence
    INT wordsPerEvent = 6;
    INT ChannelIndex, EnergyIndex;
    INT StChannel;

    DWORD *pSIS;
//    uint32_t *pSIS;

   //float *adc = new float[32*ADCModules];  
//    printf("adc initialisation: %d\n",32*ADCModules);
   //int adcchan,adcnr;
   //extern int adc_counter1, adc_counter2;   // defined; declared in analyzer.c


 		nSISwords=bk_locate(pevent, "SIS0", &pSIS);
    
		int nStHits = nSISwords/wordsPerEvent;

//		if(nSISwords>0){
//
//		for (i = 0; i < nSISwords; i++){
//			printf("pSIS[%d]: %d\n",i,pSIS[i]);
//		}

		for(int i =0; i<nStHits;i++){\

// The channel of the ADC is the last number of the first word of the sentence
		ChannelIndex = i*wordsPerEvent;
		EnergyIndex  = (i+1)*wordsPerEvent -4;

//		StChannel = (pSIS[ChannelIndex]>>16)&0x7;
		StChannel = pSIS[ChannelIndex]&0x7;

 		uint32_t fADC_energy_max_value = pSIS[EnergyIndex];
//		uint32_t fADC_energy_max_value = pSIS[EnergyIndex]&0xfffffff;
		printf("Test for Struck Channel: %d\n", StChannel);		printf("fADC_energy_max_value: %d\n",fADC_energy_max_value);
		if(StChannel<8)t_StruckADC1[StChannel] = fADC_energy_max_value;
		}
		///////////////////////////

#endif

   TDC_channel_export = new int[TDCChannelExportStore.size()];
   TDC_value_export = new float[TDCValueExportStore.size()];
   //printf("\n TDCValueExportStore.size(): %d \n",TDCValueExportStore.size());

   //printf("TDCChannelExportStore.size(): %d \t TDCValueExportStore.size(): %d\n",TDCChannelExportStore.size(),TDCValueExportStore.size());

   int TDCHits = 0;
   if(TDCChannelExportStore.size()==TDCValueExportStore.size())TDCHits = TDCValueExportStore.size();
   else{TDCHits = 0; printf("TDC Channel/Value mismatch - not going to process external data");}

   for(unsigned int p=0;p<TDCChannelExportStore.size();p++)TDC_channel_export[p] = TDCChannelExportStore[p];
   for(unsigned int p=0;p<TDCValueExportStore.size();p++)TDC_value_export[p] = TDCValueExportStore[p];
   //Now, process ADC and TDC_export through any ancillary sorts to get silicon/NaI/HPGe data into the output ROOT TTree


#ifdef _GAMMADATA
gammy = new GammaData();
#endif

#ifdef _RAWDATA
  if(raw)
  {
    raw = RawDataDump(ADC,TDCHits,TDC_channel_export, TDC_value_export, QDC);
  }
#endif
  
#ifdef _MMM
    if(si)
    {
      MMMSiliconSort(ADC, TDCHits, TDC_channel_export, TDC_value_export, si);
    }
#endif

#ifdef _W1
    if(si)
    {
      W1SiliconSort(ADC, TDCHits, TDC_channel_export, TDC_value_export, si);
    }
#endif

#ifdef _HAGAR
    if(gammy)
    {
      //gammy = HagarSort(ADC, TDCHits, TDC_channel_export, TDC_value_export);
      HagarSort(ADC, TDCHits, TDC_channel_export, TDC_value_export, gammy);
    }
#endif

#ifdef _CLOVER
    if(gammy)
    {
      //gammy = CloverSort(ADC, TDCHits, TDC_channel_export, TDC_value_export);
      CloverSort(ADC, TDCHits, TDC_channel_export, TDC_value_export, gammy);
    }
#endif

#ifdef _GATEAU
  if(fatty)
  {
    GateauSort(TDCHits, TDC_channel_export, TDC_value_export, fatty);
  }
#endif

   //--------------------------------------------------------------------------------------------------------
   // Fill TTrees
   //--------------------------------------------------------------------------------------------------------

   #ifdef _MOVIE
   if(X1hits>0 || U1hits>0  || X2hits>0){
        t_nX1wires=X1hits;
        t_nU1wires=U1hits;
        t_nX2wires=X2hits;
        t_nU2wires=U2hits;
   }
   #endif
   
   t1->Fill();    // fill the tree t1

#ifdef _SILICONDATA
   si->ClearEvent();//Clear the SiliconData gubbins at the end of the event in order to make sure that we don't fill the disk up with bollocks
//    delete si;//Delete the pointer otherwise we lose access to the memory and start to crash the machine
#endif

#ifdef _GAMMADATA
   gammy->ClearEvent();//See comment above about GammaData::ClearEvent()
   delete gammy;//See comment above about deleting *gammy
#endif

#ifdef _RAWDATA
  delete raw;
#endif

#ifdef _GATEAU
  fatty->ClearEvent();
  delete fatty;
#endif
  
#ifdef _ADC
    ADCClear();
#endif
  
   delete [] TDC_channel_export;
   delete [] TDC_value_export;
   TDCChannelExportStore.clear();
   TDCValueExportStore.clear();
   return SUCCESS;
}

//==================================================================================
//==============================DIE EINDE===========================================
//==================================================================================



