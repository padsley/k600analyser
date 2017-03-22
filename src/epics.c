/********************************************************************\

  Name:         epics.c
  Created by:   RN, from basic version by Stefan Ritt

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

/*-- Module declaration --------------------------------------------*/
INT epics_event(EVENT_HEADER *, void *);
INT epics_init(void);
INT epics_clear(INT run_number);
INT epics_eor(INT run_number);


ANA_MODULE epics_accum_module = {
   "Epics",                    /* module name           */
   "RN",                       /* author                */
   epics_event,                /* event routine         */
   epics_clear,                /* BOR routine           */
   epics_eor,                  /* EOR routine           */
   epics_init,                 /* init routine          */
   NULL,                       /* exit routine          */
   NULL,                       /* parameter structure   */
   0,                          /* structure size        */
   NULL,                       /* initial parameters    */
};

/*-- accumulated epics -------------------------------------------*/

double epics[N_SCLR], oldepics[N_SCLR];
int counter;


/*-- Histogramming Data Structures ----------------------------------------*/
static TH1F *hK600B1;
static TH1F *hK600B2;



/*-- init routine --------------------------------------------------*/
INT epics_init(void)
{
   char name[256];
   char title[256];
   int i =0;

   //create histograms that can act as countrate, assuming the
   //epics value read out is never clear, it just accumulates.

   hK600B1    = H1_BOOK("K600B1","K600B1",7200,0,7200);
   hK600B2    = H1_BOOK("K600B2","K600B2",7200,0,7200);

   counter=0;
   for(i=0;i<N_SCLR;i++){
     oldepics[i]=0;
     epics[i]=0;
   }

   return SUCCESS;
}


/*-- BOR routine ---------------------------------------------------*/
INT epics_clear(INT run_number)
{
   memset(epics, 0, sizeof(epics));
   return SUCCESS;
}


/*-- EOR routine ---------------------------------------------------*/
INT epics_eor(INT run_number)
{
   return SUCCESS;
}


/*-- event routine -------------------------------------------------*/
INT epics_event(EVENT_HEADER * pheader, void *pevent)
{
   INT n, i;
   DWORD *pepics;

   // look for midas bank 
   n = bk_locate(pevent, "Beamline", &pepics);   // n = number of data words in epics bank
   if (n == 0)  return 1;
   //printf("number of data words in epics bank = %d \n",n);

   /*   
   printf("                                    readout %d:",counter);
   */   

   /*
   for(int i=0; i<n;i++){
     //printf(" %d ",pepics[i]);
     printf("epics nr= %d  ,  value= %d \n",i,pepics[i]);
   }
   */
   //printf("%d  ,  %d \n",pepics[0],pepics[9]);


   //  This causes problems; analyzer crash  ??
   /*
   for (i = 0; i < n; i++) {  
     //epics[i]=pepics[i]-oldepics[i];  // The following is valid for epicss NOT cleared after every event    
     //oldepics[i]=pepics[i];            // The following is valid for epicss NOT cleared after every event    
     epics[i]=pepics[i];     //  This valid if the epicss are cleared after every readout.
   }
   */


   for(int i=0; i<n;i++){
     if(pepics[i]>5000) pepics[i]=5000;
   }

   /*
   for(int j = 0; j <pepics[0]; j++) {
     hTriggerU->Fill(counter);
   }
   for(int j = 0; j <pepics[9] ; j++) {
     hTriggerI->Fill(counter);
   }
   for(int j = 0; j <pepics[1]; j++) {
     hPulserU->Fill(counter);
   }
   for(int j = 0; j <pepics[10] ; j++) {
     hPulserI->Fill(counter);
   }
   for(int j = 0; j <pepics[2]; j++) {
     hCIU->Fill(counter);
   }
   for(int j = 0; j <pepics[11] ; j++) {
     hCII->Fill(counter);
   }
   for(int j = 0; j <pepics[3]; j++) {
     hClockU->Fill(counter);
   }
   for(int j = 0; j <pepics[12] ; j++) {
     hClockI->Fill(counter);
   }

   for(int j = 0; j <pepics[4] ; j++) {
     hBLM1->Fill(counter);
   }
   for(int j = 0; j <pepics[5] ; j++) {
     hBLM2->Fill(counter);
   }
   for(int j = 0; j <pepics[6] ; j++) {
     hBLM3->Fill(counter);
   }
   for(int j = 0; j <pepics[7] ; j++) {
     hBLM4->Fill(counter);
   }
   for(int j = 0; j <pepics[8] ; j++) {
     hBLM5->Fill(counter);
   }
   */

   //printf("counter = %d \n",counter);
   counter += 1;
   if(counter==7200) counter=1;  // after 7200sec start filling the spectrum from chan0



   return SUCCESS;
}
