/********************************************************************\

  Name:         tdc.c
  Created by:  Retief Neveling

\********************************************************************/


/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <time.h>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include <math.h>

/* root includes */
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
//#include <TMath.h>

/* home-made includes */
#include "Parameters.h"
#include "FocalPlane.h"

//---------------------Preprocessor directives: general-------------*/
//#define _PRINTTOSCREEN
#define _POLARIZATION
/*---------------------Preprocessor directives for K600 focal plane detectors----*/
//#define _MOVIE


// Module declaration 
INT tdc_event(EVENT_HEADER *, void *);
INT tdc_init(void);
INT tdc_bor(INT run_number);
INT tdc_eor(INT run_number);

//ADC_PARAM_STR(tdc_param_str);

ANA_MODULE tdc_module = {
   "TDC",                       /* module name           */
   "Retief Neveling",           /* author                */
   tdc_event,                   /* event routine         */
   tdc_bor,                     /* BOR routine           */
   tdc_eor,                     /* EOR routine           */
   tdc_init,                    /* init routine          */
   NULL,                        /* exit routine          */
   //&tdc_param,                  /* parameter structure   */
   //sizeof(tdc_param),           /* structure size        */
   //tdc_param_str,               /* initial parameters    */
};

#define H1I_BOOK(n,t,b,min,max) (h1_book<TH1I>(n,t,b,min,max))
#define H2I_BOOK(n,t,xb,xmin,xmax,yb,ymin,ymax) (h2_book<TH2I>(n,t,xb,xmin,xmax,yb,ymin,ymax))

// defined in Parameters.c
extern float *TDC;
extern int   *TDCchannel;
extern int TDCModules;
extern int TDCsize;
extern float *TDC_value_export;
extern int   *TDC_channel_export;
extern int TDCHits;
extern double   *TDCOffsets;

// defined in analyzer.c
extern EXP_PARAM exp_param;
extern RUNINFO runinfo;


/*-------------------- global variables --------------------*/

//-- variables to be used in main.c as extern variables 
int tof=0,toftdc1=0,toftdc2=0,toftdc3=0,toftdc4=0,toftdc5=0,toftdc6=0,toftdc7=0;
int pad1hipt, pad1lowpt, pad2hipt, pad2lowpt;
int pulser,cloverpulser; 	
#ifdef _POLARIZATION  
Int_t polu=0, pold=0;   // a pattern register equivalent, used in main.c
#endif



// Histogramming Data Structures 
TH2F **hTDC2DModule;
static TH1F *hTDCPerEvent;
static TH1F *hTDCPerEventRaw;
static TH1F *hHitPatternRawTDC;
static TH2F *hChanVsTimeRef, *hChanVsTimeOffset;
static TH1F *hTDCOff, *hTDCOfftmp;


/*-- init routine --------------------------------------------------------------*/
INT tdc_init(void)
{
   char name[256];
   char title[256];
   int i;

  //====== tdc init routine: definition of histograms ================================================

   hTDCOff = new TH1F("hTDCOff","TDC offsets",TDCsize,0,TDCsize);	// visual aid only
   hTDCOfftmp = new TH1F("hTDCOfftmp","TDC offset aid: -1000 for all channels)",TDCsize,0,TDCsize);

   hTDC2DModule = new TH2F*[TDCModules];   
   for(int counter=0;counter<TDCModules;counter++){
	  sprintf(name,"hTDC2DModule%d",counter);
	  sprintf(title,"hTDC2DModule %d ",counter);
          hTDC2DModule[counter]=new TH2F(name,title, 4000, -8000, 14999,128,0,128);
          //hTDC2DModule[counter]=new TH2F(name,title,4096,0,4096,32,0,32);
   }

   hTDCPerEventRaw = new TH1F("hTDCPerEventRaw","TDC channels/event (All data)",600,0,600);
   hTDCPerEvent    = new TH1F("hTDCPerEvent","TDC channels/event (PID selected)",MAX_WIRES_PER_EVENT,0,MAX_WIRES_PER_EVENT);
   hHitPatternRawTDC   = new TH1F("hHitPatternRawTDC","Hits per raw TDC chan",1000,0,1000);
   hChanVsTimeRef       = new TH2F("hChanVsRefTime","TDC channel vs time (ref times incl)", 3000, 0, 15000, 896, 0, 896);
   hChanVsTimeOffset    = new TH2F("hChanVsOffsetTime","TDC channel vs time (cablelenghts offsets incl)", 1500, 0, 15000, 896, 0, 896);


  //====== tdc init routine: filling some histograms ================================================

   for(int j = 0; j < TDCsize; j++) {
     //printf("TDC Offset: %f  ",TDCOffsets[j]);
     hTDCOff->SetBinContent(j,TDCOffsets[j]+1000); 
     hTDCOfftmp->SetBinContent(j,1000); 
   }
   hTDCOff->Add(hTDCOfftmp,-1);   //because I do not know how to fill a neg histogram


  //====== tdc init routine: definition of TTree variables ================================================


   return SUCCESS;
}


/*-- BOR routine ---------------------------------------------------------------*/
INT tdc_bor(INT run_number)
{
   return SUCCESS;
}


/*-- eor routine ---------------------------------------------------------------*/
INT tdc_eor(INT run_number)
{
   return SUCCESS;
}


/*-- event routine -------------------------------------------------------------*/
INT tdc_event(EVENT_HEADER * pheader, void *pevent)
{
   DWORD *ptdc;
   Int_t ntdc = 0;
   Int_t tdc1190datacode, tdc1190error, tdc1190trailerstatus;
   Int_t tdctrigtimetag;
   Int_t channel, channelnew, time;
   Int_t tdcmodule, wire;
   Int_t ref_time, offset_time;
   Int_t reftimes[10]; 
   Double_t resolution[10];                 // a array of numbers used in res plots
   Int_t tdcevtcount = 0;

   extern int empty_tdc_counter;		      // defined; declared in analyzer.c	
   extern int tdc_counter;		 	      // defined; declared in analyzer.c	
   extern int toolarge_tdc_event_counter;             // defined; declared in analyzer.c	
   extern int empty_tdc_counter;		      // defined; declared in analyzer.c	
   extern int trailer_TDCERR_counter;                 // defined; declared in analyzer.c	
   extern int trailer_triglost_counter;               // defined; declared in analyzer.c	
   extern int trailer_bufoverflow_counter;            // defined; declared in analyzer.c	

   extern int qdc_counter1;

   tof=0;toftdc1=0;toftdc2=0;toftdc3=0;toftdc4=0;toftdc5=0;toftdc6=0;toftdc7=0;  //zero at start of each event
   polu=0;pold=0;
   pad1hipt=0; pad1lowpt=0; pad2hipt=0; pad2lowpt=0;
   pulser=0;cloverpulser=0; 	
   // rather define it in Parameters.c
   //int *TDC_channel_export;
   //float *TDC_value_export;	//Defined here. Storage structure for TDC information to be exported to be used for ancillary detectors. Filled below.

   std::vector<int> TDCChannelExportStore;
   std::vector<float> TDCValueExportStore;
   

   //----------------------------------------------------------------------
   // look for TDC0 bank 

   ntdc = bk_locate(pevent, "TDC0", &ptdc);  // TDC bank in analyzer.c defined as TDC0. ntdc is nr of values in the bank

   tdc_counter++;
   if(qdc_counter1 != tdc_counter) printf("qdc and tdc event counters out of sync: qdccounter=%i     tdccounter=%i\n",qdc_counter1,tdc_counter);

   hTDCPerEventRaw->Fill(ntdc);  // a diagnostic: how many TDC channels per event, including zeros and too many
   //t_tdcsperevent=ntdc;

   tdcevtcount=(ptdc[1]>>5)&0xfffff;   // bad name: is nr of tdc hits 

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
        //hEventID->Fill(ev_id_noTDC);          
	empty_tdc_counter++;
	return 1;
   }

   getRefTimes(reftimes,ntdc,ptdc);       // reftimes[i] is copy of trigger in TDC i. Each TDC should only have 1 value/event

   hTDCPerEvent->Fill(ntdc);          // a diagnostic: to see how many TDC channels per event    

   // ===loop through all the TDC datawords===============================================
   for(int i = 0; i < ntdc; i++) {
      if((((ptdc[i])>>27)&0x1f)==0x1f){      // to determine TDC module nr. the frontend creates a dataword that
        tdcmodule=(ptdc[i])&0xf;             // has bits 27-31 =1. Then ch nr goes into bits 1-5.
        continue;                            // go back to beginning of for loop as this word only gives tdcmodule nr
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
  
      if(tdc1190datacode!=0) continue;    //ignore rest of for loop if we only have header/trailer word, BUT for valid measurent word, continue

      channel = (0x7F&((ptdc[i])>>19));      // channel is per TDC; i.e. 0-127
      time = 0x7FFFF&(ptdc[i]);

      ref_time = reftimes[tdcmodule] - time;     // to get accurate time info that does not suffer from trigger jitter
      //printf("--->  tdc.c  tdcmodule= %i,  channel= %i, time= %i   \n",tdcmodule,channel,ref_time); 
      
      if(tdcmodule<TDCModules){          // fill raw TDC 2D spectra for diagnostics
	hTDC2DModule[tdcmodule]->Fill(ref_time,channel); 		
      }

      channel = channel+tdcmodule*128;                     // convert channel nr to nr in range: 0-(nr_of_tdcs)*128
      offset_time = ref_time - int(TDCOffsets[channel]);  // in TDCoffset.dat: line nr = y bin nr in hChanVsOffsetTime

      TDCChannelExportStore.push_back(channel);
      TDCValueExportStore.push_back(offset_time);

      // fill some diagnostic histos 
      hHitPatternRawTDC->Fill(channel);            
      hChanVsTimeRef->Fill(ref_time,channel);        // 2D: chan vs reference times
      hChanVsTimeOffset->Fill(offset_time,channel);  // 2D: chan vs offset corrected times
      //printf("ntdc: %d \t tdc_counter: %d \t channel: %d \t value: %d \n",ntdc,tdc_counter,channel,offset_time);


      switch(channel){
		case (TDC_CHAN_PULSER): pulser=1; break;            // see Parameters.h. The chan = 2
		case 9:  pad1hipt=ref_time; break;
		case 10: pad1lowpt=ref_time;break;
		case 11: pad2hipt=ref_time; break;
		case 12: pad2lowpt=ref_time; break;
		case 14: cloverpulser=1;break;
		case TOF_TDC_CHAN: if(tof==0) {toftdc1=ref_time; 
						 //rntof=ref_time+TOFoffset; 
						 tof=ref_time; 
					      } break;  // this ensures only the 1st signal, not last of multiple hits, gets digitized
		case (TOF_TDC_CHAN+1*128): if(toftdc2==0) toftdc2=ref_time;break;
		case (TOF_TDC_CHAN+2*128): if(toftdc3==0) toftdc3=ref_time; break;
		case (TOF_TDC_CHAN+3*128): if(toftdc4==0) toftdc4=ref_time; break;
		case (TOF_TDC_CHAN+4*128): if(toftdc5==0) toftdc5=ref_time; break;
		case (TOF_TDC_CHAN+5*128): if(toftdc6==0) toftdc6=ref_time; break;
		case (TOF_TDC_CHAN+6*128): if(toftdc7==0) toftdc7=ref_time; break;
		#ifdef _POLARIZATION  		
		case (TDC_CHAN_POLU): polu=1; break;  // for polarized beam
		case (TDC_CHAN_POLD): pold=1; break;  // for polarized beam
		#endif
      }
      //continue;  // If you analyze the pulser-only data uncomment this line otherwise you have a segmentation fault

   }


   TDC_channel_export = new int[TDCChannelExportStore.size()];
   TDC_value_export = new float[TDCValueExportStore.size()];
   //printf("\n TDCValueExportStore.size(): %d \n",TDCValueExportStore.size());
   //printf("TDCChannelExportStore.size(): %d \t TDCValueExportStore.size(): %d\n",TDCChannelExportStore.size(),TDCValueExportStore.size());

   TDCHits = 0;
   if(TDCChannelExportStore.size()==TDCValueExportStore.size())	 {	
	TDCHits = TDCValueExportStore.size();
   }
   else {
	TDCHits = 0; printf("TDC Channel/Value mismatch - not going to process external data");
   }

   for(unsigned int p=0;p<TDCChannelExportStore.size();p++) TDC_channel_export[p] = TDCChannelExportStore[p];
   for(unsigned int p=0;p<TDCValueExportStore.size();p++) TDC_value_export[p] = TDCValueExportStore[p];

   TDCChannelExportStore.clear();
   TDCValueExportStore.clear();

   //printf("Got to SUCCESS in tdc.c\n");
   return SUCCESS;
}





