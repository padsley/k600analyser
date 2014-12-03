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

/*SDJ includes */
#include <math.h>

/* root includes */
#include <TH1F.h>
#include <TH2F.h>     
#include <TTree.h>
#include <TFile.h>
#include <TRandom3.h>   

#include "SiliconData.h"
#include "MMM.h"
#include "W1.h"

#include "CloverData.h"
#include "PR194CloverSort.h"

#include "HagarData.h"

#include "RawData.h"
/*------------definitions to change analysis------------------------*/
//#define _POLARIZATION
//#define _MOVIE
//#define _JJAUTOTRIM
//#define _PRINTTOSCREEN
//#define _VDCRESCALCS
//#define _FULLANALYSIS
//#define _MISALIGNTIME
#define _ADC
float *ADC;
extern int ADCModules;
//Uncomment for silicon analysis
#define _SILICONDATA 
  
//#define _MMM
 #define _W1
//Uncomment for clover analysis
 //#define _CLOVERDATA 
 
#define _RAWDATA
#define _HAGARDATA

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
const int TDC_CHANNELS=896;
const int TDC_MAX_TIME=14999;
const int TDC_MIN_TIME=0;
const int TDC_N_BINS=14999;
const int TOF_TDC_CHAN=1;     // ch1 in 1st TDC; i.e. the 2nd channel     
const int MAX_WIRES_PER_EVENT = 400;  // If more wires than this fire, the event data was bad  -- RN random choice  -- 
				      // to be replaced by something else?!?! really only used for array definition
				      // MUST FIND A BETTER WAY ... THIS VARIABLE NOT GOOD?
const int NR_OF_TDCS=7;
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

// general TDC variables for TTree, all with prefix "t_"
Double_t t_pad1,t_pad2;        
Double_t t_pad1hiP = 0, t_pad1lowP = 0, t_pad2hiP = 0, t_pad2lowP = 0;
Double_t t_pad1hiPT = 0, t_pad1lowPT = 0, t_pad2hiPT = 0, t_pad2lowPT = 0;
Int_t    t_tof,t_toftdc2,t_toftdc3,t_toftdc4,t_toftdc5,t_toftdc6, t_toftdc7;
Int_t    t_k600;
Int_t    t_runtime=0;
Int_t    t_evtcounter=0;
Int_t    t_tdcsperevent=0;
Double_t    x1offset=0.0;

// focal plane variables for TTree
Int_t    t_X1hits = 0,  t_X2hits = 0,   t_U1hits = 0,   t_U2hits = 0;
Double_t t_X1pos=-100.0,t_X2pos=-100.0, t_U1pos=-100.0, t_U2pos=-100.0;
Double_t t_X1th=-100.0, t_X2th=-100.0,  t_U1th=-100.0,  t_U2th=-100.0;
Double_t t_X1chisq=15.0,t_X2chisq=15.0, t_U1chisq=15.0, t_U2chisq=15.0;
Int_t    t_X1flag=-100, t_X2flag=-100,  t_U1flag=-100,  t_U2flag=-100;
Double_t t_X1effID=0,   t_X2effID=0,    t_U1effID=0,    t_U2effID=0;    // these are at present (31may10) not useful in TREE
Double_t t_X1posC=-100.0;

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

#ifdef _SILICONDATA
SiliconData *si;
#endif

#ifdef _CLOVERDATA
CloverData *clov;
#endif

#ifdef _RAWDATA
RawData *raw;
#endif

#ifdef _HAGARDATA
HagarData *hag;
#endif

Int_t t_pulser=0;    // a pattern register equivalent

#ifdef _POLARIZATION  
Int_t t_polu=0, t_pold=0;   // a pattern register equivalent
#endif

#ifdef _MOVIE  // if you want to use the 'movie' script
Double_t t_X1dt[MAX_WIRES_PER_EVENT];                                    
Double_t t_U1dt[MAX_WIRES_PER_EVENT];
Double_t t_X2dt[MAX_WIRES_PER_EVENT];
Double_t t_U2dt[MAX_WIRES_PER_EVENT];
Int_t    t_nX1wires,t_nX2wires,t_nU1wires,t_nU2wires;     //for dynamic size of t_X1dt etc in tree    
Int_t t_X1wire[MAX_WIRES_PER_EVENT];
Int_t t_X2wire[MAX_WIRES_PER_EVENT];
Int_t t_U1wire[MAX_WIRES_PER_EVENT];
Int_t t_U2wire[MAX_WIRES_PER_EVENT];
#endif


/*--------Histogramming Data Structures ---------------------------------------------*/
TFile *f1=new TFile("output.root","RECREATE");
TTree *t1=new TTree("DATA","K600 data");
TTree *t2=new TTree("DATA2","Small dataset");

static TH1F *hX1_lut, *hX2_lut, *hU1_lut, *hU2_lut;
static TH1F *hCableOff, *hCableOfftmp;

static TH1F *hEventID, *hEventID2;

static TH2F *hPad1VsTofG, *hPad1Pad2G;

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
                    printf("chan2wire[%d]  = %d  \n",tdcchan,Channel2Wire[tdcchan]);
                    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, Channel2Wire[tdcchan]);
                  }
		}
		else{
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
		    Channel2Wire[tdcchan]=i;
                    printf("chan2wire[%d] = %d   \n",tdcchan, Channel2Wire[tdcchan]);
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
   memset(time,0,7*sizeof(int));   
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
   t_X1chisq=-100.; t_X2chisq=-100.; t_U1chisq=-100.; t_U2chisq=-100.;
   t_X1flag=-100;   t_X2flag=-100;   t_U1flag=-100;   t_U2flag=-100;
   t_X1effID=-100.; t_X2effID=-100.; t_U1effID=-100.; t_U2effID=-100.;   
   t_X1res0=-100.0; t_X2res0=-100.0; t_U1res0=-100.0; t_U2res0=-100.0;
   t_X1res1=-100.0; t_X2res1=-100.0; t_U1res1=-100.0; t_U2res1=-100.0;   
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
void fixdoublehit(Double_t dd[],Int_t wire[], Double_t baddist, Int_t badwire, Int_t *_wire_num, Int_t *_wireID_min){
// USED IN RAYTRACE
   // Originally I, right away, got rid of the wires where a wire gives 2 drifttimes: throw them out...
   //  17 Nov 2011: in a more sophistacated fix, I look at which of the 2 wires that shows a multihit
   //	            has the biggest driftdistance (usually close to the 8mm) and simply takes that one out.
   //		    Note: if we have 2 sets of double wires, we deal with the second one first as badwire will be
   //		    pointing to the second incidence of the doublehit.
   // NOTE: this procedure cannot handle the scenario where 3 or more wirenumbers are the same. JJ saw these in PR153 Oct 2011
   //       but hopefully the nr of wires will then become very low, and =4 will be set, so as long 
   //       as we only plot events with =0 then we should be fine? But eff will suffer? Well, depends how it is calculated.
   /*  17 Nov 2011: in a more sophistacated fix, I look at which of the 2 wires that shows a multihit
	            has the biggest driftdistance (usually close to the 8mm) and simply takes that one out.
		    Note: if we have 2 sets of double wires, we deal with the second one first as badwire will be
		    pointing to the second incidence of the doublehit.                                                    */

   Double_t tmpdd[20];
   Int_t tmpwire[20];
   Int_t i;

      //printf("BEFORE=======\n");
      //printevent2(*_wire_num, wire,dd);

      //==================for when 2nd tdc time is such that dd is almost 8mm: problem usually in 2nd instance of wire, except at start and end of matrix

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
}

//------------------------------------------------------------------------------
void fixDoubleMinEdge(Double_t dd[],Int_t wire[], Int_t *_flag1, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_first){
// USED IN RAYTRACE
   // Originally I did not know how to fix problems where the 2nd minimum is inside the group of
   // of wires. Those events were thrown away, i.e. they receive flag=1 and not flag=0.
   // For this and the next step I id those cases where the 2nd distance min is at the edge of the wiregroud, and
   // I then simply cut these wires out of the wiregroup to be used for fitting.

   Double_t tmpdd[20];
   Int_t tmpwire[20];
   Int_t wireID_last;
   Int_t i;

   //---------------------------------fix if 2nd min is 1st wire; throw it out----------------------------
   if(*_flag1==2){        // if I know the extra min is first wire of group 
      for(i=1;i<*_wire_num;i++){                         
         tmpdd[i-1]=dd[i];
         tmpwire[i-1]=wire[i];
         dd[i-1]=tmpdd[i-1];
         wire[i-1]=tmpwire[i-1];
      }
      *_wire_num=*_wire_num-1;
      wireID_last= *_wire_num;

      // Now again label wire associated with minimum drift distance
      *_flag1=0;
      for(i=1;i<*_wire_num;i++){
         if(dd[*_wireID_min] > dd[i]) *_wireID_min = i;       //*_wireID_min an array index nr, NOT a real wire number
      }

      Int_t counter=1;
      for(i=0;i<(*_wire_num-1);i++){        // Identify monotinically increasing sets of dd; for later use?
         if( dd[i]>dd[i+1]) counter+=1;    
      }    
      if(counter==*_wire_num) *_flag1=5;

      counter=1;
      for(i=0;i<(*_wire_num-1);i++){       // Identify monotinically decreasing  sets of dd; for later use?
         if( dd[i]<dd[i+1] )  counter+=1;    
      }    
      if(counter==*_wire_num) *_flag1=6;

      // Label events with more than 1 drift distance local minimum
      for(i=1;i<(*_wire_num-1);i++){
         if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {
  	    *_flag1=1;    
	    if(dd[i-1]==dd[*_wireID_first]) *_flag1=2;  // is one of the minima at start of wiregroup?
	    if(dd[i+1]==dd[wireID_last]) *_flag1=3;   // is one of the minima at the end of the wiregroup?
        }
      }
      if(*_wire_num<3) *_flag1=4;   // flag used later to only plot positions with good nr of wires used
   }
   //-----------------------------END:fix if 2nd min is 1st wire; throw it out----------------------------


   //---------------------------------fix if 2nd min is last wire; throw it out----------------------------
   if(*_flag1==3){        // if I know the extra min is last wire of group 
      for(i=0;i<(*_wire_num-1);i++){                         
         tmpdd[i]=dd[i];
         tmpwire[i]=wire[i];
         dd[i]=tmpdd[i];
         wire[i]=tmpwire[i];
      }
      *_wire_num=*_wire_num-1;
      wireID_last= *_wire_num;

      // Now again label wire associated with minimum drift distance
      *_flag1=0;
      for(i=1;i<*_wire_num;i++){
         if(dd[*_wireID_min] > dd[i]) *_wireID_min = i;       //*_wireID_min an array index nr, NOT a real wire number
      }
     
      Int_t counter=1;
      for(i=0;i<(*_wire_num-1);i++){        // Identify monotinically increasing sets of dd; for later use?
         if( dd[i]>dd[i+1] )  counter+=1;    
      }     
      if(counter==*_wire_num) *_flag1=5;
 
      counter=1;
      for(i=0;i<(*_wire_num-1);i++){       // Identify monotinically decreasing  sets of dd; for later use?
         if( dd[i]<dd[i+1] )  counter+=1;    
      }     
      if(counter==*_wire_num) *_flag1=6;

      // Label events with more than 1 drift distance local minimum
      for(i=1;i<(*_wire_num-1);i++){
         if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {
  	    *_flag1=1;    
	    if(dd[i-1]==dd[*_wireID_first]) *_flag1=2;   // is one of the minima at start of wiregroup?
	    if(dd[i+1]==dd[wireID_last]) *_flag1=3;   // is one of the minima at the end of the wiregroup?
         }
      }
      if(*_wire_num<3) *_flag1=4;   // flag used later to only plot positions with good nr of wires used     
   }
   //---------------------------------END:fix if 2nd min is last wire; throw it out----------------------------
}

//------------------------------------------------------------------------------
void fixDoubleMinMiddle(Double_t dd[],Int_t wire[], Int_t *_flag1, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_first, Int_t *_multiplemin){
// USED IN RAYTRACE

   Double_t tmpdd[20];
   Int_t tmpwire[20];
   Int_t wireID_last;
   Int_t i;
   Int_t badwire2=1000;
   Int_t mindtwire=1000;

	if(*_wire_num==6){
	   badwire2=-1000;
	   for(i=1;i<(*_wire_num-1);i++){
	        if( (dd[i]<dd[i-1]) && (dd[i]<dd[i+1]) ) {     
		    mindtwire=i;
	        }
	        if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
		    badwire2=i;
	        }
	   }

	   if( (badwire2!=1000) && (badwire2<mindtwire)  ){
	        if (badwire2==*_wire_num-3){   // similar to badwire2>mindtwire scenario, except we have 3 bad wires at end, not 2.
	   //printf("\n\n========Multiple min: wireID_min=%i : mindtwire=%i : badwire2=%i : flag=%i \n",wireID_min,mindtwire,badwire2,*_flag1);
	   //printevent2(*_wire_num, wire,dd);
	            *_wire_num=*_wire_num-2;
	   //printf("\n=======\n");
 	   //printevent2(*_wire_num, wire,dd);
		    *_flag1=0;
		    *_multiplemin=1;
                }
	   }	
        }
	if(*_wire_num==7){
	   badwire2=-1000;
	   for(i=1;i<(*_wire_num-1);i++){
	        if( (dd[i]<dd[i-1]) && (dd[i]<dd[i+1]) ) {     
		    mindtwire=i;
	        }
	        if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
		    badwire2=i;
	        }
	   }
	   if( (badwire2!=1000) && (badwire2>mindtwire)) {
		*_wire_num=badwire2+1;	 
		if( abs(dd[0]-dd[1])<0.5) {
               	    for(i=1;i<*_wire_num;i++){              
	                 tmpdd[i]=dd[i];
	                 tmpwire[i]=wire[i];
	                 dd[i-1]=tmpdd[i];
	                 wire[i-1]=tmpwire[i];
		     }
		     *_wire_num=*_wire_num-1;
	             *_flag1=0;
		     *_multiplemin=1;
		}
		else if (dd[0]<dd[1]){
               	    for(i=1;i<*_wire_num;i++){              
	                 tmpdd[i]=dd[i];
	                 tmpwire[i]=wire[i];
	                 dd[i-1]=tmpdd[i];
	                 wire[i-1]=tmpwire[i];
		     }
		     *_wire_num=*_wire_num-1;
	             *_flag1=0;
		     *_multiplemin=1;
		}
	   //printf("\n=======\n");
 	   //printevent2(*_wire_num, wire,dd);
	   }
	   if( (badwire2!=1000) && (badwire2<mindtwire)  ){
	   //printf("\n\n========Multiple min: wireID_min=%i : mindtwire=%i : badwire2=%i : flag=%i \n",wireID_min,mindtwire,badwire2,*_flag1);
	   //printevent2(*_wire_num, wire,dd);
	        if(badwire2==2){
                     for(i=2;i<*_wire_num;i++){              
                        tmpdd[i]=dd[i];
                        tmpwire[i]=wire[i];
                        dd[i-2]=tmpdd[i];
                        wire[i-2]=tmpwire[i];
	             }
	             *_wire_num=*_wire_num-2;
	             *_flag1=0;
		     *_multiplemin=1;
                }
	        else if (badwire2==*_wire_num-3){   // similar to badwire2>mindtwire scenario, except we have 3 bad wires at end, not 2.
	             *_wire_num=*_wire_num-2;
	             //printf("!@#\n");
	             *_flag1=0;
		     *_multiplemin=1;
                }

	        if(abs(dd[0]-dd[1])<0.5) {
               	     for(i=1;i<*_wire_num;i++){              
	                  tmpdd[i]=dd[i];
	                  tmpwire[i]=wire[i];
	                  dd[i-1]=tmpdd[i];
	                  wire[i-1]=tmpwire[i];
	     	     }
		     *_wire_num=*_wire_num-1;
	             *_flag1=0;
		     *_multiplemin=1;
	             //printf("!!!!n");
	        }
	   //printf("\n=======\n");
 	   //printevent2(*_wire_num, wire,dd);
	   }       	       
	}
	if(*_wire_num==8){
	   badwire2=-1000;
	   for(i=1;i<(*_wire_num-1);i++){
	        if( (dd[i]<dd[i-1]) && (dd[i]<dd[i+1]) ) {     
		    mindtwire=i;
	        }
	        if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
		    badwire2=i;
	        }
	   }
	   if( (badwire2!=1000) && (badwire2<mindtwire)  ){
	        if(badwire2==3){
	 	     //printf("\n\n========Multiple min: wireID_min=%i : mindtwire=%i : badwire2=%i : flag=%i \n",wireID_min,mindtwire,badwire2,*_flag1);
	 	     //printevent2(*_wire_num, wire,dd);
                     for(i=3;i<*_wire_num;i++){              
                        tmpdd[i]=dd[i];
                        tmpwire[i]=wire[i];
                        dd[i-3]=tmpdd[i];
                        wire[i-3]=tmpwire[i];
	             }
	             *_wire_num=*_wire_num-3;
	             *_flag1=0;
		     *_multiplemin=1;
	   	     //printf("\n======= flag=%i \n",*_flag1);
 	   	     //printevent2(*_wire_num, wire,dd);
                }
	   }       	       
	}
        // now label wire associated with minimum drift distance anew, since previous wire_min may have been deleted
   	wireID_last  = *_wire_num-1;       // *_wireID_last  is an array index nr, NOT a real wire number
        for(i=1;i<*_wire_num;i++){
           if(dd[*_wireID_min] > dd[i]) *_wireID_min = i;       //wireID_min an array index nr, NOT a real wire number
        }
}

//--------------------------------------------------------------------------------------
void raytrace(Double_t dd[],Int_t wire[],Double_t *_X,Double_t *_Th,Double_t *_chisq,Int_t wire_num,Double_t res[],Int_t *_flag, Int_t chamber,Int_t *_wire_used, Int_t *_badwirecount, Int_t *_multmin){

   Int_t i;
   Double_t x_tttv, sum_0=0.0, sum_x=0.0, sum_z=0.0, sum_xz=0.0, sum_x2=0.0;
   Int_t    wireID_first=0, wireID_last=0, wireID_min=0;
   Double_t wirechisq=0;
   Double_t x_first, x_last, a1, b1, a, b, a_pre, b_pre, X_pre, Th_pre, drift_min_esti;
   Double_t X_v1=1.0, Th_v1=1.0,chisq_v1=0.0;
   Int_t badwire=1000;
   Int_t badwirecntA=0;   // nr to send back: nr of initial events that had double hits in them (i.e. 2 time per tdc chan)
   Int_t badwirecntB=0;   // after 1st cycle of correction: nr of initial events that had another double hit in them.
   Int_t multiplemin=-1;
   Int_t flag1=0;
   /*   Meaning of flag1 values:
   =0	good event
   =1	events with more than 1 drift distance local minimum
   =2	events with more than 1 drift distance local minimum: one of the minima at start of wiregroup
   =3   events with more than 1 drift distance local minimum: one of the minima at the end of the wiregroup
   =4   if effective nr or wires used for the final fit is less than 3
   =5	monotonically increasing sets of dd
   =6	monotonically decreasing sets of dd
   */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 0: ID first, last and min dist wires ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

   wireID_first = 0;                // wireID_first is an array index nr, NOT a real wire number
   wireID_last  = wire_num-1;       // wireID_last  is an array index nr, NOT a real wire number

   for(i=1;i<wire_num;i++){	                   // Label wire associated with minimum drift distance
     if(dd[wireID_min] > dd[i]) wireID_min = i;    // Note: wireID_min an array index nr, NOT a real wire number
   }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 1: ID double hits per wire and fix them ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
   for(i=0;i<(wire_num-1);i++){
      if( wire[i]==wire[i+1] ) {
 	 badwire=i;
	 badwirecntA+=1;              // i.e. with no multiple hits, this is 0. For double hits, it is 1. For 3 hits, this is 2.
      }				      // OR if we have 2 sets of double hits, we also have a value of 2 for badwirecntA
   }
   Double_t baddist = 7.9;
   if(badwirecntA!=0){
      //printf("========Double events per wire: badwire=%i : baddist(i+1)=%f : wire_num=%i \n",badwire,dd[badwire+1],wire_num);
      //printevent2(wire_num, wire,dd);
      fixdoublehit(dd,wire,baddist,badwire,&wire_num,&wireID_min);
      wireID_last  = wire_num-1;       
      for(i=1;i<wire_num;i++){	                       // Label wire associated with minimum drift distancei
        if(dd[wireID_min] > dd[i]) wireID_min = i;     // Note: wireID_min an array index nr, NOT a real wire number
      }
      //printf("========After 1st fix: badwire=%i : baddist(i+1)=%f : wire_num=%i \n",badwire,dd[badwire+1],wire_num);
      //printevent2(wire_num, wire,dd);
   }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 2: ID any remaining double hits per wire and fix them ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */ 
   for(i=0;i<(wire_num-1);i++){			
     if( wire[i]==wire[i+1] ) {
	 badwire=i;
	 badwirecntB+=1;               // i.e. with no multiple hits, this is 0. For double hits, it is 1. For 3 hits, this is 2.
     }				      // OR if we have 2 sets of double hits, we also have a value of 2 for badwirecntB
   }
   if(badwirecntB!=0){
      //printf("======SECOND PASS: Double events per wire: badwire=%i : baddist(i+1)=%f : wire_num=%i \n",badwire,dd[badwire+1],wire_num);
      //printevent2(wire_num, wire,dd);
      fixdoublehit(dd,wire,baddist,badwire,&wire_num,&wireID_min); 
      wireID_last  = wire_num-1;       
      for(i=1;i<wire_num;i++){	                       // Label wire associated with minimum drift distancei
        if(dd[wireID_min] > dd[i]) wireID_min = i;     // Note: wireID_min an array index nr, NOT a real wire number
      }
      //printf("=====correction after SECOND PASS: double events per wire==============================\n");
      //printevent2(wire_num, wire,dd);
      //printf("\n");
   }
   
/*~~~~~~~~~~~~~~~~~~~~~~~~STEP 3: FIRST PASS: id events with multiple distance local minima: fix those with min at edges ~~~~~~~~~~~~~~~~~~ */
   for(i=1;i<(wire_num-1);i++){
     if( (dd[i]>dd[i-1]) && (dd[i]>dd[i+1]) ) {     
       flag1=1;    
       if(dd[i-1]==dd[wireID_first]) flag1=2;   // is one of the minima at start of wiregroup?
       if(dd[i+1]==dd[wireID_last])  flag1=3;   // is one of the minima at the end of the wiregroup?
     }
   }
   //if(flag1>1){
   //      printf("==========================================Prior to FIRST PASS===\n");
   //      printf("========Multiple min: max at=%i : dd(i)=%f : wire_num=%i : flag=%i \n",i,dd[i],wire_num,flag1);
   //      printevent2(wire_num, wire,dd);
   //}
   fixDoubleMinEdge(dd,wire,&flag1, &wire_num, &wireID_min,&wireID_first);
   //if(flag1>1){
   //      printf("====AFTER THE FIRST PASS===\n");
   //      printf("========Multiple min: max at=%i : dd(i)=%f : wire_num=%i : flag=%i \n",i,dd[i],wire_num,flag1);
   //      printevent2(wire_num, wire,dd);
   //}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 4: SECOND PASS: id remaining events with multiple distance minima: fix those with min at edges ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
   // NOTE: Events with more than 1 drift distance local minimum have already been identified at the end of pass 1
   fixDoubleMinEdge(dd,wire,&flag1, &wire_num, &wireID_min,&wireID_first);
   //if(flag1>1){
   //      printf("====AFTER THE SECOND PASS===\n");
   //      printf("========Multiple min: max at=%i : dd(i)=%f : wire_num=%i : flag=%i \n",i,dd[i],wire_num,flag1);
   //      printevent2(wire_num, wire,dd);
   //}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 5: ID and fix some of the events with multiple distance minima in middle of group ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
   //      printf("==========================================Prior to FIRST PASS===\n");
   //      printf("========Multiple min: max at=%i : dd(i)=%f : wire_num=%i : flag=%i \n",i,dd[i],wire_num,flag1);
   //      printevent2(wire_num, wire,dd);
   if(flag1==1){
       //printf("==========Before Fix attempt===\n");
       //printf("========Multiple min: max at=%i : dd(i)=%f : wire_num=%i : flag=%i \n",i,dd[i],wire_num,flag1);
       //printevent2(wire_num, wire,dd);
       fixDoubleMinMiddle(dd,wire,&flag1, &wire_num, &wireID_min,&wireID_first,&multiplemin);		
       //printf("================After Fix attempt===\n");
       //printf("========Multiple min: max at=%i : dd(i)=%f : wire_num=%i : flag=%i \n",i,dd[i],wire_num,flag1);
       //printevent2(wire_num, wire,dd);
   }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~STEP 6: do raytracing ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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
   // Note where we ignore the wire with the smallest drifttime below, as it causes inaccuracies in pos determination
   sum_0=0; sum_x=0; sum_z=0; sum_xz=0; sum_x2=0; 

   for(i=0;i<wire_num;++i){
     if(dd[i]!=dd[wireID_min]){                 //ignore the wire with smallest drifttime
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
   //Double_t C1,C2,C3,C4;
   //C1=gates.a0xcorr + gates.a1xcorr*X1 + gates.a2xcorr*X1*X1 ;    
   //C2=gates.b0xcorr + gates.b1xcorr*X1 + gates.b2xcorr*X1*X1 ;   
   //C3=gates.c0xcorr + gates.c1xcorr*X1 + gates.c2xcorr*X1*X1 ;   
   //C4=gates.d0xcorr + gates.d1xcorr*X1 + gates.d2xcorr*X1*X1 ;   
   //*X1corr=X1-C1*ThetaSCAT-C2*ThetaSCAT*ThetaSCAT-C3*ThetaSCAT*ThetaSCAT*ThetaSCAT-C4*ThetaSCAT*ThetaSCAT*ThetaSCAT*ThetaSCAT;
   *Xcorr= X - (gates.a0xcorr*ThetaSCAT + gates.a1xcorr*ThetaSCAT*ThetaSCAT + gates.a2xcorr*ThetaSCAT*ThetaSCAT*ThetaSCAT 
		+ gates.a3xcorr*ThetaSCAT*ThetaSCAT*ThetaSCAT*ThetaSCAT 
	 	+ gates.b0xcorr*Y + gates.b1xcorr*Y*Y) ;
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
  *y=-1*((tmp1/tmp2-x)*tanu+26.21);        
 
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
	case 33116: x1offset=0.; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33123: x1offset=0.312561; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33132: x1offset=0.964111; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33139: x1offset=1.57703; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33148: x1offset=1.62781; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33154: x1offset=0.341736; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33159: x1offset=-0.1604; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33166: x1offset=-0.612671; printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
	case 33175: x1offset=0.674438; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
        case 33177: x1offset=0.61554; 	printf("run %d: x1 offset= %f \n",runinfo2.run_number,x1offset); break;   
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

   setupchannel2wireXoldXold();    

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
  t1->Branch("X1res0",&t_X1res0,"t_X1res0/D");
  t1->Branch("X1res1",&t_X1res1,"t_X1res1/D");
  t1->Branch("X1hits",&t_X1hits,"t_X1hits/I");
  t1->Branch("X1effID",&t_X1effID,"t_X1effID/D");
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
  t1->Branch("U1effID",&t_U1effID,"t_U1effID/D");
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
  t1->Branch("X2effID",&t_X2effID,"t_X2effID/D");
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
  t1->Branch("U2effID",&t_U2effID,"t_U2effID/D");
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

  #ifdef _FULLANALYSIS
  t1->Branch("PhiSCAT",&t_PhiSCAT,"t_PhiSCAT/D");
  #endif

  #ifdef _ADC
  ADC = new float[32*ADCModules];
  
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

  t2->Branch("tof",&t_tof,"t_tof/I");
  t2->Branch("pad1",&t_pad1,"t_pad1/D");
  t2->Branch("X1pos",&t_X1pos,"t_X1pos/D");
  t2->Branch("ThSCAT",&t_ThSCAT,"t_ThSCAT/D");
  t2->Branch("Y1",&t_Y1,"t_Y1/D");
  #ifdef _ADC
  //t2->Branch("NaIEtot",&t_NaIEtot,"t_NaIEtot/D");
  //t2->Branch("Clover1Etot",&t_Clover1Etot,"t_Clover1Etot/D");
  //t2->Branch("Clover2Etot",&t_Clover2Etot,"t_Clover2Etot/D");
  //t2->Branch("NaITDC",&t_NaITDC,"t_NaITDC[7]/D");
  //t2->Branch("Clover1TDC",&t_Clover1TDC,"t_Clover1TDC[4]/D");
  //t2->Branch("Clover2TDC",&t_Clover2TDC,"t_Clover2TDC[4]/D");
  #endif
  
#ifdef _SILICONDATA
//   printf("L2108\n");
  gROOT->ProcessLine("#include \"SiliconData.h\"");
  gROOT->ProcessLine(".L SiliconData.c+");
  t1->Branch("SiliconInfo","SiliconData",&si);
  MMMLoadCuts(si);
#endif

#ifdef _CLOVERDATA
  gROOT->ProcessLine(".L CloverData.c+");
  t1->Branch("CloverInfo","CloverData",&clov);
#endif
  
#ifdef _RAWDATA
  gROOT->ProcessLine(".L RawData.c+");
  t1->Branch("RawInfo","RawData",&raw);
#endif
   return SUCCESS;
}


/*-- event routine -----------------------------------------------------------------------------*/
INT focal_event(EVENT_HEADER * pheader, void *pevent)
{
//   printf("L2218\n");
   DWORD *ptdc;
   Int_t ntdc = 0;
   Int_t tdc1190datacode, tdc1190error, tdc1190trailerstatus;
   Int_t tdctrigtimetag;
   Int_t channel, channelnew, time;
   Int_t tdcmodule, wire;
   Int_t ref_time, offset_time;
   Int_t reftimes[7]; 
   Int_t tof=0,toftdc2=0,toftdc3=0,toftdc4=0,toftdc5=0,toftdc6=0,toftdc7=0;
   Double_t resolution[10];                 // a array of numbers used in res plots
   Int_t tdcevtcount;
   Int_t addwiregap=0;
   Double_t pad1hipt, pad1lowpt, pad2hipt, pad2lowpt;
   float PsideTDC[80];

//    extern float ADC[128];         			// defined, declared and used in adc.c     
//    printf("f-plane: ADC initialiation: %d\n",32*ADCModules);
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

   //
   float ADC_export[160];
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
   //printf(" nr of TDC datawords:                    %d words\n",ntdc);
   tdc_counter++;
   hTDCPerEventRaw->Fill(ntdc);    
   t_tdcsperevent=ntdc;
   tdcevtcount=(ptdc[1]>>5)&0xfffff;  // the 'evtnr' 

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
	return 1;
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
      
      if(tdcmodule<7){
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
       raytrace(X1.dist, X1.wire, &X1pos, &X1th, &X1chisq, X1hits_dt, resolution, &X1flag,1, &X1wires_used, &X1doublewires, &X1multiplemin); 
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

       raytrace(U1.dist, U1.wire, &U1pos, &U1th, &U1chisq, U1hits_dt, resolution, &U1flag,2,&U1wires_used, &U1doublewires, &U1multiplemin); 
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
      
       raytrace(X2.dist, X2.wire, &X2pos, &X2th, &X2chisq, X2hits_dt, resolution, &X2flag,3,&X2wires_used, &X2doublewires, &X2multiplemin); 
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

       raytrace(U2.dist, U2.wire, &U2pos, &U2th, &U2chisq, U2hits_dt, resolution, &U2flag,4,&U2wires_used, &U2doublewires, &U2multiplemin); 
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
   CalcThetaScat(ThFP,X1pos,&ThSCAT);
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

   CalcCorrX(X1pos-x1offset, (Y2+10), ThSCAT, &Xcorr);
   t_X1posC=Xcorr;

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

   TDC_channel_export = new int[TDCChannelExportStore.size()];
   TDC_value_export = new float[TDCValueExportStore.size()];printf("\n TDCValueExportStore.size(): %d \n",TDCValueExportStore.size());

   //printf("TDCChannelExportStore.size(): %d \t TDCValueExportStore.size(): %d\n",TDCChannelExportStore.size(),TDCValueExportStore.size());

   int TDCHits = 0;
   if(TDCChannelExportStore.size()==TDCValueExportStore.size())TDCHits = TDCValueExportStore.size();
   else{TDCHits = 0; printf("TDC Channel/Value mismatch - not going to process external data");}

   for(unsigned int p=0;p<TDCChannelExportStore.size();p++)TDC_channel_export[p] = TDCChannelExportStore[p];
   for(unsigned int p=0;p<TDCValueExportStore.size();p++){TDC_value_export[p] = TDCValueExportStore[p];printf("\n TDC_value_export[%d]: %d\n",p,TDC_value_export[p]);}
   //Now, process ADC and TDC_export through any ancillary sorts to get silicon/NaI/HPGe data into the output ROOT TTree
//#ifdef _SILICONDATA
#ifdef _RAWDATA
  for(int p=0;p<160;p++)ADC_export[p] = ADC[p];
  if(raw)
  {
    raw = RawDataDump(ADC_export,TDCHits,TDC_channel_export, TDC_value_export);
  }
#endif
  
#ifdef _MMM
   for(int p=0;p<128;p++)ADC_export[p] = ADC[p];//Populate ADC_export from the ADC array. This is itself created in adc.c. Remember to change the maximum limit for the loop depending on what you need to loop over. If you have n ADCs, you shoul use 32*n as that limit
    if(si)
    {
      si = MMMSiliconSort(ADC_export, TDCHits, TDC_channel_export, TDC_value_export);
    }
#endif

#ifdef _W1
for(int p=0;p<160;p++)ADC_export[p] = ADC[p];//Populate ADC_export from the ADC array. This is itself created in adc.c. Remember to change the maximum limit for the loop depending on what you need to loop over. If you have n ADCs, you shoul use 32*n as that limit
    if(si)
    {
      si = W1SiliconSort(ADC_export, TDCHits, TDC_channel_export, TDC_value_export);
    }
#endif

#ifdef _CLOVERDATA
//   clov = new CloverData();
  for(int p=0;p<128;p++)ADC_export[p] = ADC[p];
  if(clov)
  {
    clov = PR194CloverSort(ADC_export, TDCHits, TDC_channel_export, TDC_value_export);
  }
#endif

#ifdef _HAGARDATA
    for(int p=0;p<160;p++)ADC_export[p] = ADC[p];
    if(hag)
    {
      hag = HagarDataSort(ADC_export, TDCHits, TDC_channel_export, TDC_value_export);
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
   t2->Fill();    // fill the tree t2 - PA
//    si->PrintEvent();
#ifdef _SILICONDATA
   si->ClearEvent();//Clear the SiliconData gubbins at the end of the event in order to make sure that we don't fill the disk up with bollocks
   delete si;//Delete the pointer otherwise we lose access to the memory and start to crash the machine
#endif
#ifdef _CLOVERDATA
   clov->ClearEvent();//See comment above about SiliconData::ClearEvent()
   delete clov;//See comment above about deleting *si
#endif
#ifdef _RAWDATA
  delete raw;
#endif
#ifdef _HAGARDATA
  delete hag;
#endif
  
   //delete ADC;
  
   delete TDC_channel_export;
   delete TDC_value_export;
   TDCChannelExportStore.clear();
   TDCValueExportStore.clear();
   return SUCCESS;
}

//==================================================================================
//==============================DIE EINDE===========================================
//==================================================================================



