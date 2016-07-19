/********************************************************************\

  Name:         beamline.c
  Created by:   R. Neveling

  Contents:     This module looks for MSRD banks which hold info on beamline elements

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


/*-- variables to be used in f-plane.c as extern variables----------*/
//int runtime;
//int CII,CIU,triggerU,triggerI;


/*-- Module declaration --------------------------------------------*/
INT beam_event(EVENT_HEADER *, void *);
INT beam_init(void);
INT beam_clear(INT run_number);
INT beam_eor(INT run_number);

ANA_MODULE beamline_module = {
   "beam",                   /* module name           */
   "R Neveling",                 /* author                */
   beam_event,               /* event routine         */
   beam_clear,               /* BOR routine           */
   beam_eor,                 /* EOR routine           */
   beam_init,                /* init routine          */
   NULL,                         /* exit routine          */
   NULL,                         /* parameter structure   */
   0,                            /* structure size        */
   NULL,                         /* initial parameters    */
};


double beamline[N_MSRD];
//int counter;

/*-- Histogramming Data Structures ----------------------------------------*/
static TH1F *hK600D1;
static TH1F *hK600D2;
static TH1F *hK600Q;
static TH1F *hK600H;
static TH1F *hK600K;
static TH1F *hSLIT9Xxgap;
static TH1F *hSLIT12Xxgap;

/*-- init routine --------------------------------------------------*/
INT beam_init(void)
{
   //char name[256];
   //char title[256];
   //int i =0;

   //counter=0;

   //create histograms that can act as countrate, assuming the
   //beamline value read out is never cleared; it just accumulates.

   hK600D1       = H1_BOOK("hK600D1","K600 D1 real",121,0,120);
   hK600D2       = H1_BOOK("hK600D2","K600 D2 real",121,0,120);
   hK600Q        = H1_BOOK("hK600Q","K600 Q real",121,0,120);
   hK600K        = H1_BOOK("hK600K","K600 K real",121,0,120);
   hK600H        = H1_BOOK("hK600H","K600 H real",121,0,120);
   hSLIT9Xxgap   = H1_BOOK("hslit9Xxgap","Slit 9X x-gap real",121,0,120);
   hSLIT12Xxgap  = H1_BOOK("hslit12Xxgap","Slit 12X x-gap",121,0,120);

   return SUCCESS;
}

/*-- BOR routine ---------------------------------------------------*/
INT beam_clear(INT run_number)
{
   memset(beamline, 0, sizeof(beamline));
   return SUCCESS;
}

/*-- EOR routine ---------------------------------------------------*/
INT beam_eor(INT run_number)
{
   return SUCCESS;
}

/*-- event routine -------------------------------------------------*/
INT beam_event(EVENT_HEADER * pheader, void *pevent)
{
   INT n;
   //FLOAT *pmsrd;
   float *pmsrd;
   extern int beamline_counter;		      // defined; declared in analyzer.c
 
   beamline_counter++;

   // look for MSRD bank ===========================================================================================
   n = bk_locate(pevent, "MSRD", &pmsrd);   // n = number of data words in beamline bank
   if (n == 0)  return 1;

/*
   printf("Beamline event # %i;  number of data words in beamline bank = %d \n",beamline_counter,n); 	
   for(int i=0; i<n;i++){
     printf("beamline nr= %d  ,  value= %f  \n",i,pmsrd[i]);
   }
*/
   hK600D1->SetBinContent(beamline_counter,pmsrd[30]);
 

/* 
   triggerU = pmsrd[0];
   triggerI = pmsrd[32];
   CIU = pmsrd[2];
   CII = pmsrd[34];
 */

   //==============================================================================================================

 /*
   counter += 1;
   runtime = counter;             // this is a value to be put in the tree structure. Hence I can
 				  // cut on time as well in the tree
   if(counter==14400) counter=1;  // afteu 14400sec=4h start filling the spectrum from chan0

 */

   return SUCCESS;
}
