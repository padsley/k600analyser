
/********************************************************************************\
  Name:         main.c
  Created by:   Retief Neveling
  Contents:     The central analysis module. 

		Contains the Initialization routine, the Begin-Of-Run routine, 
		the Event routine, and the End-of-Run routine

		Currently undergoing some changes to take out VDC 
		stuff from this and leave this as the main routine

\********************************************************************************/


/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"

/* root includes */
#include <TH1F.h>
#include <TH2F.h>     
#include <TTree.h>
#include <TFile.h>
#include <TRandom3.h>
#include <TMath.h>
#include "TGraph.h"

/* home-made includes */
#include "Parameters.h"
#include "SiliconData.h"
#include "MMM.h"
#include "W1.h"
#include "GammaData.h"
#include "HagarSort.h"
#include "RawData.h"
#include "FocalPlane.h"

#include "CloverSort.h"
#include "ScintillatorSort.h"

/*------------Preprocessor Directives to change analysis------------*/
//#define _POLARIZATION
//#define _MOVIE
//#define _JJAUTOTRIM
#define _PRINTTOSCREEN
//#define _VDCRESCALCS
#define _FULLANALYSIS
//#define _MISALIGNTIME
#define _RAWDATA
//#define _SILICONDATA 
//#define _MMM
//#define _W1
#define _GAMMADATA
// #define _HAGAR
#define _SCINTILLATOR
#define _CLOVER

/*-- For ODB: from /Analyzer/Parameters and /Equipment/-------------*/
//FOCALPLANE_PARAM gates;     // these are to be found in experim.h
MAIN_PARAM gates;     // these are to be found in experim.h
GLOBAL_PARAM globals;
WIRECHAMBER_SETTINGS WireChamberSettings;
RUNINFO runinfo2;
  // At present we use 2 methods to get to ODB data:/
  // -the globals we get through GetODBGlobals() subroutine
  // -the gates we used to get directly from the ODB, see focalplane_module declaration
  // below. The gates will thus change upon changing an ODB entry 'on-the-fly' 
  // while the globals will only change for each initialzation as it is
  // obtained only in the init routine.

/*---- MIDAS Module declaration ------------------------------------*/
INT main_event(EVENT_HEADER *, void *);
INT main_bor(INT run_number);
INT main_eor(INT run_number);
INT main_init(void);

//FOCALPLANE_PARAM_STR(focalplane_param_str);
MAIN_PARAM_STR(main_param_str);

ANA_MODULE main_module = {
   "main",                	/* module name           */
   "Retief Neveling",           /* author                */
   main_event,                  /* event routine         */
   main_bor,                    /* BOR routine           */
   main_eor,                    /* EOR routine           */
   main_init,                   /* init routine          */
   NULL,                        /* exit routine          */
   &gates,                      /* parameter structure   */
   sizeof(gates),               /* structure size        */
   main_param_str,        /* initial parameters IN ODB????   */
};

/*-------------------- defined somewhere else ----------------------*/
extern float *ADC;
extern int *ADCchannel;
extern int ADCModules;
extern float *QDC;
extern int TDCModules;


/*-------------------- global variables for main --------------------*/

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
Double_t t_pad1=0,t_pad2=0, t_pad1raw=0; //padoffsets correction        
Double_t t_pad1hiP = 0, t_pad1lowP = 0, t_pad2hiP = 0, t_pad2lowP = 0;
Double_t t_pad1hiPT = 0, t_pad1lowPT = 0, t_pad2hiPT = 0, t_pad2lowPT = 0;
Int_t    t_tof,t_toftdc1,t_toftdc2,t_toftdc3,t_toftdc4,t_toftdc5,t_toftdc6, t_toftdc7;
Int_t    t_k600;
Int_t    t_runtime=0;
Int_t    t_evtcounter=0;
Int_t    t_tdcsperevent=0;
Int_t    t_triggerI=0;
Int_t    t_triggerU=0;
Int_t    t_CII=0;
Int_t    t_CIU=0;



// focal plane variables for TTree
Int_t    t_X1hits = 0,  t_X2hits = 0,   t_U1hits = 0,   t_U2hits = 0;
Double_t t_X1pos=-100.0,t_X2pos=-100.0, t_U1pos=-100.0, t_U2pos=-100.0;
Double_t t_X1th=-100.0, t_X2th=-100.0,  t_U1th=-100.0,  t_U2th=-100.0;
Double_t t_X1chisq=15.0,t_X2chisq=15.0, t_U1chisq=15.0, t_U2chisq=15.0;
Int_t    t_X1flag=-100, t_X2flag=-100,  t_U1flag=-100,  t_U2flag=-100;
Double_t t_X1effID=0,   t_X2effID=0,    t_U1effID=0,    t_U2effID=0;    // these are at present (31may10) not useful in TREE
Double_t t_X1posO=-100.0;  // for offset added position
Double_t t_X1posC=-100.0, t_X1posCTOF=-100;
double t_Ex = -0.;
double t_ExC = -0.;
double t_T3 = -0.;
double t_rigidity3 = -0.;
double t_thetaSCAT = -90.;
double t_phiSCAT = -180;
double t_theta = -90;
Double_t t_thetaFP=-100; 
Double_t t_thetaFPx=-100;
Double_t t_phiFP=-100;

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


#ifdef _SILICONDATA
SiliconData *si;
#endif

// #ifdef _CLOVERDATA
// CloverData *clov;
// #endif

#ifdef _RAWDATA
RawData *raw;
#endif

#ifdef _GAMMADATA
GammaData *gammy;
#endif

// #ifdef _SCINTILLATOR
// ScintillatorData *scint;
// #endif


Int_t t_pulser=0;    // a pattern register equivalent
Int_t t_cloverpulser=0;

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

Double_t X1wirefit[10], X1distfit[10];      
Double_t U1wirefit[10], U1distfit[10];      
Double_t X2wirefit[10], X2distfit[10];      
Double_t U2wirefit[10], U2distfit[10];  

Double_t x1offset=0.0;
Int_t TOFoffset=0;
Double_t Padoffset=0;


/*-----------------------------------------------------------------------------------*/
/*--------Histogramming Data Structures ---------------------------------------------*/
/*-----------------------------------------------------------------------------------*/

TFile *f1=new TFile("output.root","RECREATE");
TTree *t1=new TTree("DATA","K600 data");

static TH1F *hX1_lut, *hX2_lut, *hU1_lut, *hU2_lut;
static TH1F *hCableOff, *hCableOfftmp;

static TH1F *hEventID, *hEventID2;

static TH2F *hPad1VsTofG, *hPad1Pad2G;

TH2F **hTDC2DModule;
TH2F *hADCChannels_vs_TDCChannels;

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
const int TDC_CHANNELS=896;
static TH1F *hTDC_REF[TDC_CHANNELS];   // for use of JJ_autotrim.C
#endif


//----------------------------------------------------------------------------------------//
void GetODBGlobals()
// Get globals from the ODB; not needed if we use Parameters and Module Declaration path
{
    HNDLE hDB, hKey;

   // open ODB structures 
   cm_get_experiment_database(&hDB, NULL);
   db_find_key(hDB, 0,  (char *)"/Analyzer/Parameters/GLOBAL", &hKey);
   if(db_open_record(hDB, hKey, &globals, sizeof(GLOBAL_PARAM), MODE_READ, NULL, NULL) != DB_SUCCESS) {
      cm_msg(MERROR,  (char *)"analyzer_init",  (char *)"Cannot open \"/Analyzer/Parameters/GLOBAL\" tree in ODB");
      exit(1);//return 0;
   }
   //printf("ODB test : %d  \n",globals.misswires);
   
   db_close_record(hDB,hKey);

   //printf("globals.x1_1st_wire_chan: %d  \n",globals.x1_1st_wire_chan);
   //printf("globals.x2_1st_wire_chan: %d  \n",globals.x2_1st_wire_chan);  
   //printf("globals.u1_1st_wire_chan: %d  \n",globals.u1_1st_wire_chan);  
   //printf("globals.u2_1st_wire_chan: %d  \n",globals.u2_1st_wire_chan);

}



//----------------------------------------------------------------------------------------//
void GetODBRunInfo()
// Get globals from the ODB; not needed if we use Parameters and Module Declaration path
{
    HNDLE hDB, hKey;

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

  printf("lut x1 offset: %d \n",globals.lut_x1_offset);
  printf("lut u1 offset: %d \n",globals.lut_u1_offset);
  printf("lut x2 offset: %d \n",globals.lut_x2_offset);
  printf("lut u2 offset: %d \n",globals.lut_u2_offset);

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
   t_pad1=-1;  t_pad2=-1;  t_pad1raw=-1; 
   t_pad1hiP=-1;  t_pad1lowP=-1;  t_pad2hiP=-1;  t_pad2lowP=-1;
   t_pad1hiPT=-1; t_pad1lowPT=-1; t_pad2hiPT=-1; t_pad2lowPT=-1;
   t_tof=0; t_toftdc1=0; t_toftdc2=0; t_toftdc3=0; t_toftdc4=0; t_toftdc5=0; t_toftdc6=0; t_toftdc7=0;
   t_k600=0; t_runtime=-1;
   t_triggerI=0, t_triggerU=0, t_CII=0, t_CIU=0;
   t_X1hits = -100; t_X2hits = -100; t_U1hits = -100; t_U2hits = -100;
   t_X1pos=-100.; t_X2pos=-100.; t_U1pos=-100.; t_U2pos=-100.;
   t_X1th=-100.;  t_X2th=-100.;  t_U1th=-100.;  t_U2th=-100.;
   t_X1posO=-100.;
   t_X1posC=-100., t_X1posCTOF=-100.;

   t_Ex=-1.;
   t_ExC = -1.;
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
   t_thetaFP=-100; t_thetaFPx=-100; t_phiFP=-100;
   t_thetaSCAT=-100; t_phiSCAT=-100; 
   t_theta=-100;
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
   t_cloverpulser=0;


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





//================================================================================================
/*-- init routine ---------------------Happens before BOR---------------------------------------*/
//================================================================================================
INT main_init(void)                    
{
   char name[256];
   char title[256];

   //ParameterInit();     // now called inside init routine of analyzer.c

   extern bool VDC1_new, VDC2_new;
   extern bool VDC1_new_UX, VDC2_new_UX;

   if(VDC1_new)
     {
       if(VDC2_new)
	 {
	   if(VDC1_new_UX && VDC2_new_UX){
	       setupchannel2wireUXUX(Channel2Wire);
	     }
	   else setupchannel2wireXUXU(Channel2Wire);
	 }
       else
	 {
 	   setupchannel2wireXUXold(Channel2Wire);
	   //printf("Probably not implemented");
	 }
     }
   else
     {
       if(VDC2_new)
	 {
 	   setupchannel2wireXoldXU(Channel2Wire);
	 }
       else
	 {
	   setupchannel2wireXoldXold(Channel2Wire);
	 }
     }


   #ifdef _MISALIGNTIME
   read_misalignment(&misaligntime,"misalignment.dat");
   printf("== Misalignment time cutoff (sec): %i ==\n",misaligntime);
   #endif

   read_cable(cableOffset,(char *)"CableLength.dat");

   open_subfolder((char *)"LUT&Cable");
     hX1_lut = new TH1F("hX1_lut","LUT X1",LUT_CHANNELS,0,LUT_CHANNELS);
     hU1_lut = new TH1F("hU1_lut","LUT U1",LUT_CHANNELS,0,LUT_CHANNELS);
     hX2_lut = new TH1F("hX2_lut","LUT X2",LUT_CHANNELS,0,LUT_CHANNELS);
     hU2_lut = new TH1F("hU2_lut","LUT U2",LUT_CHANNELS,0,LUT_CHANNELS);
     hCableOff = new TH1F("hCableOff","cable offsets)",896,0,896);
     hCableOfftmp = new TH1F("hCableOfftmp","cable offset aid: -1000 for all channels)",896,0,896);
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

   hEventID  = new TH1F("hEventID","each bin represents a type of event",30,0,30);
   //hEventID  = new TH1F("hEventID","each bin represents a type of event",30,0,30);
   hEventID2  = new TH1F("hEventID2","each bin represents a type of event",30,0,30);

   hPad1VsTofG  = new TH2F("hPad1VsTofG","Paddle 1 ave VS TOF (gated)", 2000,TDC_MIN_TIME,TDC_MAX_TIME, 1000, 0, 4095 );
   hPad1Pad2G  = new TH2F("hPad1Pad2G","Pad1(y-axis) vs Pad2(x-axis) (PID gated)",  1024, 0, 4096, 1024,0 , 4096);

   hTDCPerEventRaw = new TH1F("hTDCPerEventRaw","TDC channels/event (All data)",600,0,600);
   //hTDCPerEvent    = new TH1F("hTDCPerEvent","TDC channels/event (PID selected)",MAX_WIRES_PER_EVENT,0,MAX_WIRES_PER_EVENT);

   hTDC2DModule = new TH2F*[TDCModules];
   for(int counter=0;counter<TDCModules;counter++){
	  sprintf(name,"hTDC2DModule%d",counter);
	  sprintf(title,"hTDC2DModule %d ",counter);
          hTDC2DModule[counter]=new TH2F(name,title, 4000, -8000, 14999,128,0,128);
   }
   
    
   hHitPatternRawTDC   = new TH1F("hHitPatternRawTDC","Hits per raw TDC chan",1000,0,1000);
   hHitPatternAll   = new TH1F("hHitPatternAll","Hits/Chan (ALL data)",1000,0,1000);
   hHitPatternPID   = new TH1F("hHitPatternPID","Hits/Chan (PID selected)",1000,0,1000);
  
   hChanVsTimeRef       = new TH2F("hChanVsRefTime","TDC channel vs time (ref times incl)", 3000, 0, 15000, 896, 0, 896);
   hChanVsTimeOffset    = new TH2F("hChanVsOffsetTime","TDC channel vs time (cablelenghts offsets incl)", 1500, 0, 15000, 896, 0, 896);
   hChanVsTimeOffsetPID = new TH2F("hChanVsOffsetTimePID","TDC channel vs time (cablelenghts offsets incl)", 1200, 4000, 10000, 896, 0, 896);
   hWireVsTimeOffset    = new TH2F("hWireVsOffsetTime","Wire channel vs time (cablelenghts offsets incl)", 1500, 0, 15000, 1000, 0, 1000);
   hWireVsTimeOffsetPID = new TH2F("hWireVsOffsetTimePID","Wire channel vs time (cablelenghts offsets incl) PID selected", 1200, 4000, 10000, 1000, 0, 1000);

   hX1_EffID    = new TH1F("hX1_EffID","VDC efficiency calculation: X1 ",20,0,20);
   hU1_EffID    = new TH1F("hU1_EffID","VDC efficiency calculation: U1 ",20,0,20);
   hX2_EffID    = new TH1F("hX2_EffID","VDC efficiency calculation: X2 ",20,0,20);
   hU2_EffID    = new TH1F("hU2_EffID","VDC efficiency calculation: U2 ",20,0,20);

   hX1_DriftLength = new TH1F("hX1_DriftLength","DriftLength X1 (mm)  ",110,-1,10);
   hX2_DriftLength = new TH1F("hX2_DriftLength","DriftLength X2 (mm)  ",110,-1,10);
   hU1_DriftLength = new TH1F("hU1_DriftLength","DriftLength U1 (mm)  ",110,-1,10);
   hU2_DriftLength = new TH1F("hU2_DriftLength","DriftLength U2 (mm)  ",110,-1,10);

   hX1_DriftTimeGood = new TH1F("hX1_DriftTimeGood","X1 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU1_DriftTimeGood = new TH1F("hU1_DriftTimeGood","U1 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hX2_DriftTimeGood = new TH1F("hX2_DriftTimeGood","X2 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU2_DriftTimeGood = new TH1F("hU2_DriftTimeGood","U2 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   #ifdef _FULLANALYSIS
   hDriftTimeRawAll  = new TH1F("hDriftTimeRawAll","All drifttimes (before subtracting ref times), 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeOffsetAll  = new TH1F("hDriftTimeOffsetAll","All drifttimes (cablelenghts offsets included), 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hX1_DriftTimeRef  = new TH1F("hX1_DriftTimeRef","X1 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hX2_DriftTimeRef  = new TH1F("hX2_DriftTimeRef","X2 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU1_DriftTimeRef  = new TH1F("hU1_DriftTimeRef","U1 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU2_DriftTimeRef  = new TH1F("hU2_DriftTimeRef","U2 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hX1_DriftTimeOffset = new TH1F("hX1_DriftTimeOff","X1 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hX2_DriftTimeOffset = new TH1F("hX2_DriftTimeOff","X2 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU1_DriftTimeOffset = new TH1F("hU1_DriftTimeOff","U1 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hU2_DriftTimeOffset = new TH1F("hU2_DriftTimeOff","U2 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hX1_Hits  = new TH1F("hX1_Hits","X1 wires/event (all X1 events)",20,0,20);
   hX2_Hits  = new TH1F("hX2_Hits","X2 wires/event (all U1 events)",20,0,20);
   hU1_Hits  = new TH1F("hU1_Hits","U1 wires/event (all X2 events)",20,0,20);
   hU2_Hits  = new TH1F("hU2_Hits","U2 wires/event (all U2 events)",20,0,20);

   hX1_HitsG  = new TH1F("hX1_HitsG","X1 wires/event (good event)",20,0,20);
   hX2_HitsG  = new TH1F("hX2_HitsG","X2 wires/event (good event)",20,0,20);
   hU1_HitsG  = new TH1F("hU1_HitsG","U1 wires/event (good event)",20,0,20);
   hU2_HitsG  = new TH1F("hU2_HitsG","U2 wires/event (good event)",20,0,20);

   hX1_Chisq     = new TH1F("hX1_Chisq","Chi squared: X1 ",500,0,15);
   hX1_Eff       = new TH1F("hX1_Eff","VDC efficiency: X1 ",1000,0,100);
   hX1_PosEff    = new TH2F("hX1_PosEff","VDC efficiency along focal plane: X1 ",200,0,800,1000,0,100);
   hU1_Chisq     = new TH1F("hU1_Chisq","Chi squared: U1 ",500,0,15);
   hU1_Eff       = new TH1F("hU1_Eff","VDC efficiency: U1 ",1000,0,100);
   hX2_Chisq     = new TH1F("hX2_Chisq","Chi squared: X2 ",500,0,15);
   hX2_Eff       = new TH1F("hX2_Eff","VDC efficiency: X2 ",1000,0,100);
   hU2_Chisq     = new TH1F("hU2_Chisq","Chi squared: U2 ",500,0,15);
   hU2_Eff       = new TH1F("hU2_Eff","VDC efficiency: U2 ",1000,0,100);

   hX1_Res       = new TH1F("hX1_Res","X1 position resolution",400,-8.,8.);
   hX1_Res2dRCNP = new TH2F("hX1_Res2dRCNP","RCNP style resolution plot: X1", 200,-3,3,200,-3.,3.);
   hX1_Res2diTL  = new TH2F("hX1_Res2diTL","iTL style resolution plot for X1 (xaxis=pos-int(pos)) ", 200,-0.1,1.1,200,-3.,3.);
   hU1_Res       = new TH1F("hU1_Res","U1 position resolution",400,-8.,8.);
   hU1_Res2dRCNP = new TH2F("hU1_Res2dRCNP","Min.drift-Estimated VS Min.drift: U1", 200,-3,3,200,-3.,3.);
   hU1_Res2diTL  = new TH2F("hU1_Res2diTL","iTL style resolution plot: U1", 200,-0.1,1.1,200,-3.,3.);
   hX2_Res       = new TH1F("hX2_Res","X2 position resolution",400,-8.,8.);
   hX2_Res2dRCNP = new TH2F("hX2_Res2dRCNP","Min.drift-Estimated VS Min.drift: X2", 200,-3,3,200,-3.,3.);
   hX2_Res2diTL  = new TH2F("hX2_Res2diTL","iTL style resolution plot: X2", 200,-0.1,1.1,200,-3.,3.);
   hU2_Res       = new TH1F("hU2_Res","U2 position resolution",400,-8.,8.);
   hU2_Res2dRCNP = new TH2F("hU2_Res2dRCNP","Min.drift-Estimated VS Min.drift: U2", 200,-3,3,200,-3.,3.);
   hU2_Res2diTL  = new TH2F("hU2_Res2diTL","iTL style resolution plot: U2", 200,-0.1,1.1,200,-3.,3.);

   h_Y1   = new TH1F("h_Y1","Y1 ",400,-100,100);
   h_Y2   = new TH1F("h_Y2","Y2 ",400,-100,100);
   #endif

   #ifdef _JJAUTOTRIM                           // if you use JJ_autotrim.C
   open_subfolder("Individual TDC chan");
   open_subfolder("X1_Refs");
   for(int i=0;i<TDC_CHANNELS;i++){
	sprintf(name,"TDCchan_%d",i);
	sprintf(title,"TDC channel # %d (reftimes incl) ",i);
   	hTDC_REF[i] = new TH1F(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   }
   close_subfolder();
   close_subfolder();   
   #endif

   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  t1->Branch("runtime",&t_runtime,"t_runtime/I");
  t1->Branch("evtcounter",&t_evtcounter,"t_evtcounter/I");
  t1->Branch("tdcsperevent",&t_tdcsperevent,"t_tdcsperevent/I");

  t1->Branch("triggerI",&t_triggerI,"t_triggerI/I");
  t1->Branch("triggerU",&t_triggerU,"t_triggerU/I");
  t1->Branch("CIU",&t_CIU,"t_CIU/I");
  t1->Branch("CII",&t_CII,"t_CII/I");

  t1->Branch("tof",&t_tof,"t_tof/I");
  t1->Branch("toftdc1",&t_toftdc1,"t_toftdc1/I");
  t1->Branch("toftdc2",&t_toftdc2,"t_toftdc2/I");
  t1->Branch("toftdc3",&t_toftdc3,"t_toftdc3/I");
  t1->Branch("toftdc4",&t_toftdc4,"t_toftdc4/I");
  t1->Branch("toftdc5",&t_toftdc5,"t_toftdc5/I");
  t1->Branch("toftdc6",&t_toftdc6,"t_toftdc6/I");
  t1->Branch("toftdc7",&t_toftdc7,"t_toftdc7/I");

  t1->Branch("k600",&t_k600,"t_k600/I");

  t1->Branch("pad1",&t_pad1,"t_pad1/D");
  t1->Branch("pad2",&t_pad2,"t_pad2/D");
  t1->Branch("pad1raw",&t_pad1raw,"t_pad1raw/D"); //padoffsets correction 
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

  t1->Branch("thetaFP",&t_thetaFP,"t_thetaFP/D");
  t1->Branch("thetaFPx",&t_thetaFPx,"t_thetaFPx/D");
  t1->Branch("phiFP",&t_phiFP,"t_phiFP/D");
  t1->Branch("Y1",&t_Y1,"t_Y1/D");
  t1->Branch("Y2",&t_Y2,"t_Y2/D");
  t1->Branch("pulser",&t_pulser,"t_pulser/I");
  t1->Branch("cloverpulser",&t_cloverpulser,"t_cloverpulser/I");
  t1->Branch("X1posC",&t_X1posC,"t_X1posC/D");

  t1->Branch("X1posCTOF",&t_X1posCTOF,"t_X1posCTOF/D");

  t1->Branch("X1posO",&t_X1posO,"t_X1posO/D");
  t1->Branch("Ex",&t_Ex,"t_Ex/D");
  t1->Branch("ExC",&t_ExC,"t_ExC/D");
  t1->Branch("T3",&t_T3,"t_T3/D");
  t1->Branch("rigidity3",&t_rigidity3,"t_rigidity3/D");
  t1->Branch("theta",&t_theta,"t_theta/D");
  t1->Branch("thetaSCAT",&t_thetaSCAT,"t_thetaSCAT/D");
  t1->Branch("phiSCAT",&t_phiSCAT,"t_phiSCAT/D");

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
  //printf("L2108\n");
  gROOT->ProcessLine(".L Parameters.c+");
  gROOT->ProcessLine("#include \"SiliconData.h\"");
  gROOT->ProcessLine(".L SiliconData.c+");
  t1->Branch("SiliconInfo","SiliconData",&si);
  //MMMLoadCuts(si);
#endif

#ifdef _MMM
  MMMInit();    // At this moment it only sets up the angle definitions
#endif


#ifdef _GAMMADATA
  //printf("L2108\n");
  gROOT->ProcessLine(".L Parameters.c+");
  gROOT->ProcessLine("#include \"GammaData.h\"");
  gROOT->ProcessLine(".L GammaData.c+");
  t1->Branch("GammaInfo","GammaData",&gammy);
  //MMMLoadCuts(si);
#endif

// #ifdef _CLOVERDATA
//   gROOT->ProcessLine(".L Parameters.c+");
//   gROOT->ProcessLine(".L CloverData.c+");
//   t1->Branch("CloverInfo","CloverData",&clov);
// #endif
  
#ifdef _RAWDATA
  gROOT->ProcessLine(".L Parameters.c+");
  gROOT->ProcessLine(".L RawData.c+");
  t1->Branch("RawInfo","RawData",&raw);
#endif

hADCChannels_vs_TDCChannels = new TH2F("hADCChannels_vs_TDCChannels", "hADCChannels_vs_TDCChannels", 128*TDCModules, 0., 128*TDCModules, 32*ADCModules, 0., 32*ADCModules);//Moved declaration of this histogram to not within the main loop -> i.e. don't declare it new each loop.

   
   return SUCCESS;
}


//================================================================================================
/*-- BOR routine (BeginOfRun)--------------Happens after init------------------------------------*/
//================================================================================================
INT main_bor(INT run_number)
{
   //memset(scaler, 0, sizeof(scaler));
   GetODBRunInfo();       // this was put here so that one can get access to run nr info, if you want
		          // to set up the analyzer to look at runnr, and then decide which offsets
		          // or calib paraters it will use. See Fhum's email to me: 
		          // 	Subject: 	functions
			  //    Date: 	Thu, 7 Feb 2013 15:50:35 +0200 (SAST)
   
   GetODBGlobals();                // get globals that can be set in the ODB
   //GetODBfocalplaneGates();        // get from ODB parameters in /Analyzer/Parameters/focalplane
   //PrintODBstuff();

   read_lut(lutx1,globals.lut_x1_offset,(char *)"lut-x1.dat");              
   read_lut(lutu1,globals.lut_u1_offset,(char *)"lut-u1.dat");         
   read_lut(lutx2,globals.lut_x2_offset,(char *)"lut-x2.dat");   
   read_lut(lutu2,globals.lut_u2_offset,(char *)"lut-u2.dat");              
     
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
	
   extern int RunNumber;          // defined in Parameters.c,  the REAL run number you are analyzing
   extern double *X1Offsets;	        // from Parameters.c 
   extern int *RunNrForX1Offsets;       // from Parameters.c  
   extern int NrOfRunsForX1Offsets;     // nr of runs for which we have x1offsets read it via Parameters.c
   extern int *TOFOffsets;	        // from Parameters.c 
   extern int *RunNrForTOFOffsets;       // from Parameters.c  
   extern int NrOfRunsForTOFOffsets;     // nr of runs for which we have TOFoffsets read it via Parameters.c
   extern int *PadOffsets;	        // from Parameters.c 
   extern int *RunNrForPadOffsets;       // from Parameters.c  
   extern int NrOfRunsForPadOffsets;     // nr of runs for which we have Padoffsets read it via Parameters.c

   x1offset =0.0;   // set it to zero, so that if nothing happens inside IF loop you have a value for it
   for (int i = 0; i< NrOfRunsForX1Offsets;i++){
       if( RunNrForX1Offsets[i] == RunNumber) x1offset=X1Offsets[i];  
   }
   printf("run %d: x1 offset= %f \n",RunNumber,x1offset);


   TOFoffset =0;   // set it to zero, so that if nothing happens inside IF loop you have a value for it
   for (int i = 0; i< NrOfRunsForTOFOffsets;i++){
       if( RunNrForTOFOffsets[i] == RunNumber) TOFoffset=TOFOffsets[i]; // as defined in Parameter.c 
   }
   printf("run %d: TOF offset= %d \n",RunNumber,TOFoffset);

   Padoffset =0;   // set it to zero, so that if nothing happens inside IF loop you have a value for it
   for (int i = 0; i< NrOfRunsForPadOffsets;i++){
       if( RunNrForPadOffsets[i] == RunNumber) Padoffset=PadOffsets[i]; // as defined in Parameter.c 
   }
   printf("run %d: Paddle offset= %d \n",RunNumber,Padoffset);

   return SUCCESS;
}




//================================================================================================
/*-- event routine -----------------------------------------------------------------------------*/
//================================================================================================
INT main_event(EVENT_HEADER * pheader, void *pevent)
{
   //printf("main.c L949\n");
   DWORD *ptdc;
   Int_t ntdc = 0;
   Int_t tdc1190datacode, tdc1190error, tdc1190trailerstatus;
   Int_t tdctrigtimetag;
   Int_t channel, channelnew, time;
   Int_t tdcmodule, wire;
   Int_t ref_time, offset_time;
   Int_t reftimes[10]; 
   Int_t tof=0,toftdc1=0,toftdc2=0,toftdc3=0,toftdc4=0,toftdc5=0,toftdc6=0,toftdc7=0;
   Double_t resolution[10];                 // a array of numbers used in res plots
   Int_t tdcevtcount = 0;
   Int_t addwiregap=0;
   Double_t pad1hipt, pad1lowpt, pad2hipt, pad2lowpt;
   float pad1raw=0; //padoffsets correction
   float PsideTDC[80];

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
   extern int triggerI, triggerU, CII, CIU;   
 
   int *TDC_channel_export;
   float *TDC_value_export;	//Defined here. Storage structure for TDC information to be exported to be used for ancillary detectors. Filled below.

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
   Double_t thetaFP=-100;   
   Double_t thetaFPx=-100;
   Double_t thetaSCAT=-100, phiSCAT=-100;
   Double_t Y1=-100.0,Y2=-100.0;
   Double_t Xcorr=-100, Xcorr2=-100;
   Int_t X1wires_used=0,X2wires_used=0,U1wires_used=0,U2wires_used=0;
   Int_t X1doublewires=0,X2doublewires=0,U1doublewires=0,U2doublewires=0;
   Int_t X1multiplemin=0,X2multiplemin=0,U1multiplemin=0,U2multiplemin=0;
   Int_t X1chisqminimization=0,U1chisqminimization=0,X2chisqminimization=0,U2chisqminimization=0;

   //---------------------------------------------------------------------
   // Start analysis
   t_evtcounter=qdc_counter1;
   //printf("main.c L1046: Event nr : %d\n",qdc_counter1);
   

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

   // padoffsets correction the pad1 will be the corrected and pad1raw not
   pad1raw=pad1; 
   pad1=pad1raw+Padoffset; 
   t_pad1=pad1;
   t_pad1raw=pad1raw; 


//   t_pad1=pad1;
   t_pad2=pad2;	       
   t_pad1hiP=pad1hip;
   t_pad1lowP=pad1lowp;	    
   t_pad2hiP=pad2hip;
   t_pad2lowP=pad2lowp;	    
   t_runtime=runtime;
   t_triggerI=triggerI;
   t_triggerU=triggerU;
   t_CII=CII;
   t_CIU=CIU;
   

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
       tdcevtcount=(ptdc[1]>>5)&0xfffff;  // the 'evtnr' 
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
	return 1;
   }

   getRefTimes(reftimes,ntdc,ptdc);       // reftimes[i] is copy of trigger in TDC i. Each TDC should only have 1 value/event
 
   // ===loop through all the TDC datawords================================================================================================

   //hTDCPerEvent->Fill(ntdc);          // a diagnostic: to see how many TDC channels per event    
   //TDC_channel_export = new int[ntdc]; //<- Declare the size of the array for the TDC data to go to any external sorts
   //TDC_value_export = new float[ntdc];
   
   for(int i = 0; i < ntdc; i++) {
      if((((ptdc[i])>>27)&0x1f)==0x1f){      // to determine TDC module nr. the frontend creates a dataword that
        tdcmodule=(ptdc[i])&0xf;             // has bits 27-31 =1. Then ch nr goes into bits 1-5.
        continue;                            // go back to beginning of for loop as this word only tells us about tdcmodule nr
      }                                      // Raw data: 1st word has TDC module nr, then info on that module, then go to next tdcmodule
      if(tdcmodule<0 || tdcmodule>TDCModules){
	printf("bad tdc module nr %d\n",tdcmodule);   // error condition
      }
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

 
      ref_time = reftimes[tdcmodule] - time;     // to get accurate time info that does not suffer from trigger jitter
      
      if(tdcmodule<TDCModules){
	hTDC2DModule[tdcmodule]->Fill(ref_time,channel); 		
      }

      channel = channel+tdcmodule*128;                     // convert channel nr to nr in range: 0-(nr_of_tdcs)*128
      
      offset_time = ref_time - int(cableOffset[channel]);  // in CableLength.dat: line nr = y bin nr in hChanVsOffsetTime

      //printf("ntdc: %d \t tdc_counter: %d \t channel: %d \t value: %d \n",ntdc,tdc_counter,channel,offset_time);
      //TDC_channel_export[i] = channel;
      //TDC_value_export[i] = offset_time;
      TDCChannelExportStore.push_back(channel);
      TDCValueExportStore.push_back(offset_time);

      switch(channel){
		case (TDC_CHAN_PULSER): t_pulser=1; break;            // see Parameters.h. The chan = 2

		case 9:  pad1hipt=ref_time;t_pad1hiPT=pad1hipt; break;
		case 10: pad1lowpt=ref_time;t_pad1lowPT=pad1lowpt; break;
		case 11: pad2hipt=ref_time; t_pad2hiPT=pad2hipt;break;
		case 12: pad2lowpt=ref_time; t_pad2lowPT=pad2lowpt;break;
		case 14: t_cloverpulser=1;break;

		case TOF_TDC_CHAN: if(t_tof==0) {toftdc1=ref_time; 
						 tof=ref_time+TOFoffset; 
						 t_tof=tof;
						 t_toftdc1=toftdc1;} break;  // this ensures only the 1st signal, not last of multiple hits, gets digitized
		case (TOF_TDC_CHAN+1*128): if(t_toftdc2==0) toftdc2=ref_time; t_toftdc2=toftdc2; break;
		case (TOF_TDC_CHAN+2*128): if(t_toftdc3==0) toftdc3=ref_time; t_toftdc3=toftdc3; break;
		case (TOF_TDC_CHAN+3*128): if(t_toftdc4==0) toftdc4=ref_time; t_toftdc4=toftdc4; break;
		case (TOF_TDC_CHAN+4*128): if(t_toftdc5==0) toftdc5=ref_time; t_toftdc5=toftdc5; break;
		case (TOF_TDC_CHAN+5*128): if(t_toftdc6==0) toftdc6=ref_time; t_toftdc6=toftdc6; break;
		case (TOF_TDC_CHAN+6*128): if(t_toftdc7==0) toftdc7=ref_time; t_toftdc7=toftdc7; break;
		#ifdef _POLARIZATION  		
		case (TDC_CHAN_POLU): t_polu=1; break;  // for polarized beam
		case (TDC_CHAN_POLD): t_pold=1; break;  // for polarized beam
		#endif
      }

      hHitPatternRawTDC->Fill(channel);            
      hChanVsTimeRef->Fill(ref_time,channel);        // 2D: chan vs reference times
      hChanVsTimeOffset->Fill(offset_time,channel);  // 2D: chan vs offset corrected times

      #ifdef _JJAUTOTRIM  
      if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1 && channel<TDC_CHANNELS){		// if you want to use JJ_autotrim.C
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
      else if ((channelnew >= globals.x2_1st_wire_chan) && (channelnew < globals.x2_last_wire_chan) && (channelnew!=604) ) {   //only for X2 wireplane
      //else if ((channelnew >= globals.x2_1st_wire_chan) && (channelnew < globals.x2_last_wire_chan)) {   //only for X2 wireplane
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
   
   //printf("min x wires %i,  max x wires %i \n",globals.min_x_wires, globals.max_x_wires);
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

       t_X1pos=X1pos;         //for current clumsy implementation of TTree. for good events: must plot with X1flag=0!!!!!!!!

       t_X1posO=X1pos + x1offset;     

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
//       raytrace(U1.dist, U1.wire, &U1pos, &U1th, &U1chisq, U1hits_dt, resolution, &U1flag,2,&U1wires_used, &U1doublewires, &U1multiplemin); 
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
     if(globals.misswires+1>(wrangeX2-X2hits_dt)){
       hEventID->Fill(ev_id_X2_wires);  // events in X2 that pass through wire requirement gates 
       t_X2effgroup=1; 
       if(tof>gates.lowtof && tof<gates.hitof && PaddlePIDGatesFlag==1)	 hX2_EffID->Fill(ev_wiregap);
      
       raytrace(X2.dist, X2.wire, &X2pos, &X2th, &X2chisq, X2hits_dt, resolution, &X2flag,3, X2wirefit, X2distfit,&X2wires_used, &X2doublewires, &X2multiplemin, &X2chisqminimization); 
//       raytrace(X2.dist, X2.wire, &X2pos, &X2th, &X2chisq, X2hits_dt, resolution, &X2flag,3,&X2wires_used, &X2doublewires, &X2multiplemin); 
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
//       raytrace(U2.dist, U2.wire, &U2pos, &U2th, &U2chisq, U2hits_dt, resolution, &U2flag,4,&U2wires_used, &U2doublewires, &U2multiplemin); 
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
   //thetaFP = CalcThetaFP(X1pos,X2pos);
   //t_thetaFP = thetaFP;
   thetaFP  = CalcThetaFP(U1pos,U2pos);
   t_thetaFP   = thetaFP;

   Y1=CalcYFP(X1pos,U1pos,X1th);  
   t_Y1=Y1;
   #ifdef _FULLANALYSIS
   h_Y1->Fill(Y1);
   #endif

   Y2=CalcYFP(X2pos,U2pos,thetaFP);  // I get funny double locus if I use calc theta // changed by AT to be used 
   t_Y2=Y2;
   #ifdef _FULLANALYSIS
   h_Y2->Fill(Y2);
   #endif

   t_phiFP=CalcPhiFP(X1pos,Y1,X2pos,Y2,thetaFP);

   thetaSCAT = CalcThetaScat(X1pos,thetaFP);   //NOTE: we need thetaSCAT for the calculation of corrX. Therefore 
   t_thetaSCAT = thetaSCAT;		       // we can only use X1pos in the thetaSCAT calculation.

   CalcCorrX(X1pos+x1offset, Y1, thetaSCAT, &Xcorr);
   t_X1posC=Xcorr;

   CalcCorrXTOF(X1pos+x1offset, Y1, tof, &Xcorr2);
   t_X1posCTOF=Xcorr2;


   t_phiSCAT = CalcPhiScat(Xcorr,thetaFP,Y1);
   t_theta = CalcTheta(Xcorr, thetaFP, Y1);

   //t_Ex = CalcExDirect(Xcorr);
   t_Ex = CalcEx(Xcorr2);

   extern double *masses;
   t_T3 = CalcTfromXcorr(Xcorr2, masses[2]);
   t_rigidity3 = CalcQBrho(Xcorr2);

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
   TDC_value_export = new float[TDCValueExportStore.size()];
   //printf("\n TDCValueExportStore.size(): %d \n",TDCValueExportStore.size());

   //printf("TDCChannelExportStore.size(): %d \t TDCValueExportStore.size(): %d\n",TDCChannelExportStore.size(),TDCValueExportStore.size());

   int TDCHits = 0;
   if(TDCChannelExportStore.size()==TDCValueExportStore.size())TDCHits = TDCValueExportStore.size();
   else{TDCHits = 0; printf("TDC Channel/Value mismatch - not going to process external data");}

   for(unsigned int p=0;p<TDCChannelExportStore.size();p++)TDC_channel_export[p] = TDCChannelExportStore[p];
   for(unsigned int p=0;p<TDCValueExportStore.size();p++)TDC_value_export[p] = TDCValueExportStore[p];

  
   
   //========================================================================================================================
   //Now, process ADC and TDC_export through any ancillary sorts to get silicon/NaI/HPGe data into the output ROOT TTree
   //========================================================================================================================


#ifdef _GAMMADATA
gammy = new GammaData();
#endif


#ifdef _SILICONDATA
si = new SiliconData();
#endif


   
#ifdef _RAWDATA
  if(raw)
  {
    //printf("made it in main.c to RawDataDump\n");
    raw = RawDataDump(ADC,ADCchannel,TDCHits,TDC_channel_export, TDC_value_export, QDC);
    
    for(int i=0; i<TDCValueExportStore.size(); i++)
    {
        for(int j=0; j<sizeof(ADCchannel); j++)
        {
			if(ADC[j]>0.0)
			{
				//hADCChannels_vs_TDCChannels->Fill(TDC_channel_export[i], ADCchannel[j]);
			}
				//hADCChannels_vs_TDCChannels->Fill(TDC_channel_export[i], ADCchannel[j]);
        }
    }
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

#ifdef _SCINTILLATOR
    if(gammy)
    {
      //gammy = HagarSort(ADC, TDCHits, TDC_channel_export, TDC_value_export);
      ScintillatorSort(ADC, TDCHits, TDC_channel_export, TDC_value_export, gammy);
    }
#endif

   //========================================================================================================================
   // Fill TTrees
   //========================================================================================================================

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
   si->ClearEvent(); //Clear the SiliconData gubbins at the end of the event in order to make sure that we don't fill the disk up with bollocks
   delete si;        //Delete the pointer otherwise we lose access to the memory and start to crash the machine
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
#endif
   
//#ifdef _ADC
   if(ADCModules>0) ADCClear();
//#endif
        
//#define _ADC
   delete [] TDC_channel_export;
   delete [] TDC_value_export;
   TDCChannelExportStore.clear();
   TDCValueExportStore.clear();

   return SUCCESS;
}


//================================================================================================
/*-- EOR routine -------------------------------------------------------------------------------*/
//================================================================================================
INT main_eor(INT run_number)
{
/*
#ifdef _SILICONDATA
   delete si;        //Delete the pointer otherwise we lose access to the memory and start to crash the machine
#endif
*/

   return SUCCESS;
}



