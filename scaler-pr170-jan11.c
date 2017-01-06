/********************************************************************\

  Name:         scaler.c
  Created by:   Stefan Ritt

  Contents:     Example scaler analyzer module. This module looks
                for a SCLR banks and accumulates scalers into an
                ACUM bank.

  $Log: scaler.c,v $
  Revision 1.5  2004/01/08 08:40:08  midas
  Implemented standard indentation

\********************************************************************/


/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <string.h>
#include <time.h>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"

#include "TH1F.h"


/*------------Preprocessor Directives to change analysis------------*/
#define _MISALIGNTIME




/*-- variables to be used in f-plane.c as extern variables----------*/
int runtime;
int CII,CIU,triggerU,triggerI;



/*-- Module declaration --------------------------------------------*/
INT scaler_event(EVENT_HEADER *, void *);
INT scaler_init(void);
INT scaler_clear(INT run_number);
INT scaler_eor(INT run_number);


ANA_MODULE scaler_accum_module = {
   "Scalers",                   /* module name           */
   "Stefan Ritt",               /* author                */
   scaler_event,                /* event routine         */
   scaler_clear,                /* BOR routine           */
   scaler_eor,                  /* EOR routine           */
   scaler_init,                 /* init routine          */
   NULL,                        /* exit routine          */
   NULL,                        /* parameter structure   */
   0,                           /* structure size        */
   NULL,                        /* initial parameters    */
};

/*-- accumulated scalers -------------------------------------------*/

double scaler[N_SCLR], oldscaler[N_SCLR];
int counter;
//Double_t oldtrigI,oldtrigU;



/*-- Histogramming Data Structures ----------------------------------------*/
static TH1F *hClockI;
static TH1F *hClockU;
static TH1F *hPulserI;
static TH1F *hPulserU;
static TH1F *hCII;
static TH1F *hCIU;
static TH1F *hTriggerI;
static TH1F *hTriggerU;
static TH1F *hBLM1;
static TH1F *hBLM2;
static TH1F *hBLM3;
static TH1F *hBLM4;
static TH1F *hBLM5;
static TH1F *hBLM6;
static TH1F *hBLM7;
static TH1F *hBLM8;
static TH1F *hTDCBUF1;
static TH1F *hTDCBUF2;
static TH1F *hTDCBUF3;
static TH1F *hTDCBUF4;
static TH1F *hTDCBUF5;
static TH1F *hTDCBUF6;
static TH1F *hTDCBUF7;
static TH1F *hQDCBUF;


/*-- init routine --------------------------------------------------*/
INT scaler_init(void)
{
   //char name[256];
   //char title[256];
   //int i =0;

   counter=0;

   //create histograms that can act as countrate, assuming the
   //scaler value read out is never cleared; it just accumulates.

   hClockI    = H1_BOOK("Scaler_ClockI","Clock scaler (Inhibited)",7200,0,7200);
   hClockU    = H1_BOOK("Scaler_ClockU","Clock scaler (UnInhibited)",7200,0,7200);
   hPulserI   = H1_BOOK("Scaler_PulserI","Pulser scaler (Inhibited)",7200,0,7200);
   hPulserU   = H1_BOOK("Scaler_PulserU","Pulser scaler (UnInhibited)",7200,0,7200);
   hCII       = H1_BOOK("Scaler_CII","CI scaler (Inhibited)",7200,0,7200);
   hCIU       = H1_BOOK("Scaler_CIU","CI scaler (UnInhibited)",7200,0,7200);
   hTriggerI  = H1_BOOK("Scaler_TriggerI","Trigger scaler (Inhibited)",7200,0,7200);
   hTriggerU  = H1_BOOK("Scaler_TriggerU","Trigger scaler (UnInhibited)",7200,0,7200);
   hBLM1  = H1_BOOK("Scaler_BLM1","BLM scaler 1",7200,0,7200);
   hBLM2  = H1_BOOK("Scaler_BLM2","BLM scaler 2",7200,0,7200);
   hBLM3  = H1_BOOK("Scaler_BLM3","BLM scaler 3",7200,0,7200);
   hBLM4  = H1_BOOK("Scaler_BLM4","BLM scaler 4",7200,0,7200);
   hBLM5  = H1_BOOK("Scaler_BLM5","BLM scaler 5",7200,0,7200);
   hBLM6  = H1_BOOK("Scaler_BLM6","BLM scaler 6",7200,0,7200);
   hBLM7  = H1_BOOK("Scaler_BLM7","BLM scaler 7",7200,0,7200);
   hBLM8  = H1_BOOK("Scaler_BLM8","BLM scaler 8",7200,0,7200);

   hTDCBUF1 = H1_BOOK("Scaler_TDCBUF1","TDCBUF1",7200,0,7200);
   hTDCBUF2 = H1_BOOK("Scaler_TDCBUF2","TDCBUF2",7200,0,7200);
   hTDCBUF3 = H1_BOOK("Scaler_TDCBUF3","TDCBUF3",7200,0,7200);
   hTDCBUF4 = H1_BOOK("Scaler_TDCBUF4","TDCBUF4",7200,0,7200);
   hTDCBUF5 = H1_BOOK("Scaler_TDCBUF5","TDCBUF5",7200,0,7200);
   hTDCBUF6 = H1_BOOK("Scaler_TDCBUF6","TDCBUF6",7200,0,7200);
   hTDCBUF7 = H1_BOOK("Scaler_TDCBUF7","TDCBUF7",7200,0,7200);
   hQDCBUF = H1_BOOK("Scaler_QDCBUF","QDCBUF",7200,0,7200);

   return SUCCESS;
}


/*-- BOR routine ---------------------------------------------------*/
INT scaler_clear(INT run_number)
{
   memset(scaler, 0, sizeof(scaler));
   return SUCCESS;
}


/*-- EOR routine ---------------------------------------------------*/
INT scaler_eor(INT run_number)
{
   return SUCCESS;
}


/*-- event routine -------------------------------------------------*/
//  Look in the frontend code! At present only the first ? channels per scaler
//  are enabled. That is why you should have only 2*? datawords read
//  out per scaler event because there are 2 scaler modules.   <-- OLD COMMENT: RN Nov10
INT scaler_event(EVENT_HEADER * pheader, void *pevent)
{
   INT n;
   DWORD *psclr;
   extern int scaler_counter;		      // defined; declared in analyzer.c
   extern int misaligntime; 		      // declared in main.c

   #ifdef _MISALIGNTIME
   if (counter>misaligntime) {
	return SUCCESS;         // stop analysis if we have misalignment beyond time misaligntime
   }
   #endif 


 
   scaler_counter++;

   // look for SCLR bank ===========================================================================================
   n = bk_locate(pevent, "SCLR", &psclr);   // n = number of data words in scaler bank
   if (n == 0)  return 1;

   //printf("number of data words in scaler bank = %d \n",n); 
   //printf("                                    readout %d:",counter);

   for(int i=0; i<n;i++){
     //printf("scaler nr= %d  ,  value= %d  ,  DWORD= %x \n",i,psclr[i],psclr[i]);
     //if((psclr[i]&0xffffffff)>0xffff){
     if ( psclr[i]>>28&0xf ){                                     // i found that some scalers have value ffffff82f
	//printf("-------------------------------------------\n"); // and that straight printf prints it as -2001
	psclr[i]=111;					 	   // so this test is supposed to take out all with	
     }                                                             // f as bits 29-32 (28-31)
   }


  
   for(unsigned int j = 0; j <psclr[0]; j++) {   //use of unsigned int to get rid of compiler warning:
     hTriggerU->Fill(counter);			 //"warning: comparison between signed and unsigned integer expressions"
   }
   for(unsigned int j = 0; j <psclr[1]; j++) {
     hPulserU->Fill(counter);
   }; 
   for(unsigned int j = 0; j <psclr[2]; j++) {
     hCIU->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[3]; j++) {
     hClockU->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[4] ; j++) {
     hBLM1->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[5] ; j++) {
     hBLM2->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[6] ; j++) {
     hBLM3->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[7] ; j++) {
     hBLM4->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[8] ; j++) {
     hBLM5->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[9] ; j++) {
     hBLM6->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[10] ; j++) {
     hBLM7->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[11] ; j++) {
     hBLM8->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[13] ; j++) {   
     hTriggerI->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[14] ; j++) {   
     hPulserI->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[15] ; j++) {   
     hCII->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[16] ; j++) {
     hClockI->Fill(counter);
   }

   triggerU = psclr[0];
   triggerI = psclr[13];
   CIU = psclr[2];
   CII = psclr[15];


   // look for SCLD bank ===========================================================================================
   n = bk_locate(pevent, "SCLD", &psclr);   // n = number of data words in scaler bank
   if (n == 0)  return 1;

   for(unsigned int j = 0; j <psclr[7]; j++) {
     hTDCBUF1->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[8]; j++) {
     hTDCBUF2->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[9]; j++) {
     hTDCBUF3->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[10]; j++) {
     hTDCBUF4->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[11]; j++) {
     hTDCBUF5->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[12]; j++) {
     hTDCBUF6->Fill(counter);
   }
   for(unsigned int j = 0; j <psclr[13]; j++) {
     hTDCBUF7->Fill(counter);
   }
   for(int j = 0; j <psclr[14]; j++) {
     hQDCBUF->Fill(counter);
   }







   //==============================================================================================================
   counter += 1;
   runtime = counter;             // this is a value to be put in the tree structure. Hence I can
 				  // cut on time as well in the tree
   if(counter==14400) counter=1;  // afteu 14400sec=4h start filling the spectrum from chan0

   //printf("scaler event nr  = %d \n",counter); 


   return SUCCESS;
}
