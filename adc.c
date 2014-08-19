/********************************************************************\

  Name:         adc.c
  Created by:   Stephen Jones
  Modified by:  Retief Neveling

\********************************************************************/


/*-- Include files -------------------------------------------------*/

/* standard includes */
#include <stdio.h>
#include <time.h>

/* midas includes */
#include "midas.h"
#include "experim.h"
#include "analyzer.h"
#include <math.h>

/* root includes */
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
//#include <TMath.h>


/*-- variables to be used in f-plane.c as extern variables----------*/
float ADC[128];
int adcevtcount;
float NaI[8];//, Plastic[8];
float Sectors[40],Rings[80];
//float RingsTDC[80];
float Ring1[16],Ring2[16],Ring3[16],Ring4[16],Ring5[16];
float Sector1[8],Sector2[8],Sector3[8],Sector4[8],Sector5[8];


/*-- For ODB: from /Analyzer/Parameters/----------------------------*/
ADC_PARAM adc_param;


/*-- Module declaration --------------------------------------------*/
INT adc_event(EVENT_HEADER *, void *);
INT adc_init(void);
INT adc_bor(INT run_number);
INT adc_eor(INT run_number);

ADC_PARAM_STR(adc_param_str);

ANA_MODULE adc_module = {
   "ADC",                       /* module name           */
   "Retief Neveling",           /* author                */
   adc_event,                   /* event routine         */
   adc_bor,                     /* BOR routine           */
   adc_eor,                     /* EOR routine           */
   adc_init,                    /* init routine          */
   NULL,                        /* exit routine          */
   &adc_param,                  /* parameter structure   */
   sizeof(adc_param),           /* structure size        */
   adc_param_str,               /* initial parameters    */
};


/*--------------------------------------------------------------------------*/
#define H1I_BOOK(n,t,b,min,max) (h1_book<TH1I>(n,t,b,min,max))
#define H2I_BOOK(n,t,xb,xmin,xmax,yb,ymin,ymax) (h2_book<TH2I>(n,t,xb,xmin,xmax,yb,ymin,ymax))

/*------------Declarations of predefined Constants ----------------*/
const int ADC_N_BINS = 4096;
const int ADC_X_LOW  = 0;
const int ADC_X_HIGH = 4095;
extern EXP_PARAM exp_param;
extern RUNINFO runinfo;

/*-- Histogramming Data Structures ----------------------------------------*/
static TH2F *hADC2DModule[5];
static TH2F *hSiFBHitPattern;

/*-- init routine --------------------------------------------------*/
INT adc_init(void)
{
   char name[256];
   char title[256];
   int i;

   for(int counter=0;counter<5;counter++){
	  sprintf(name,"hADC2DModule%d",counter);
	  sprintf(title,"hADC2DModule %d ",counter);
          hADC2DModule[counter]=H2_BOOK(name,title,4100,0,4100,32,0,32);
   }

   hSiFBHitPattern = H2_BOOK("hSiFBHitPattern","", 48,0,48,24,0.,24.);

     return SUCCESS;
}


/*-- BOR routine ---------------------------------------------------*/

INT adc_bor(INT run_number)
{
   return SUCCESS;
}



/*-- eor routine ---------------------------------------------------*/
INT adc_eor(INT run_number)
{
   return SUCCESS;
}

  enum v792N_DataType {
    v792N_typeMeasurement=0,
    v792N_typeHeader     =2,
    v792N_typeFooter     =4,
    v792N_typeFiller     =6
  };


  typedef union {
    DWORD raw;
    struct EntryN {
      unsigned adc:12; // bit0 here
      unsigned ov:1;
      unsigned un:1;
      unsigned _pad_1:2;
      unsigned channel:5;
      unsigned _pad_2:3;
      unsigned type:3;
      unsigned geo:5;
    } data ;
    struct HeaderN {
      unsigned _pad_1:8; // bit0 here
      unsigned cnt:6;
      unsigned _pad_2:2;
      unsigned crate:8;
      unsigned type:3;
      unsigned geo:5;
    } header;
    struct FooterN {
      unsigned evtCnt:24; // bit0 here
      unsigned type:3;
      unsigned geo:5;
    } footer;
  } v792N_Data; 


/*-- event routine -------------------------------------------------*/
INT adc_event(EVENT_HEADER * pheader, void *pevent)
{
   INT i, nwords;
   DWORD *padc;
   float adc[160];  
   int adcchan,adcnr;
   extern int adc_counter1, adc_counter2;   // defined; declared in analyzer.c
 

   for(int i = 0; i < 16 ; i++) { 
     Ring1[i]=0;
     Ring2[i]=0;
     Ring3[i]=0;
     Ring4[i]=0;
     Ring5[i]=0;
   }
   for(int i = 0; i < 8 ; i++) { 
     Sector1[i]=0;
     Sector2[i]=0;
     Sector3[i]=0;
     Sector4[i]=0;
     Sector5[i]=0;
     NaI[i]=0;
   }

  	
   /* look for ADC0 bank, return if not present */
   nwords=bk_locate(pevent, "ADC0", &padc);
   adc_counter1++;
   if (nwords==0){
      adc_counter2++;
      return 1;
   }
        
   for (i = 0; i < nwords; i++){
        //printf("-------raw data 0x%08x  Nr of words %d \n",padc[i],nwords); 
        if(((padc[i]>>24)&0xff) ==0xfd) {
           if((padc[i]&0xf) ==0) adcnr=0;
           if((padc[i]&0xf) ==1) adcnr=1;
           if((padc[i]&0xf) ==2) adcnr=2;  //printf(" adc nr 2 \n"); 
           if((padc[i]&0xf) ==3) adcnr=3;  //printf(" adc nr 3 \n");
           if((padc[i]&0xf) ==4) adcnr=4;  //printf(" adc nr 4 \n");
	   //printf("-----raw data 0x%08x ->  data %d adcnr %i \n",padc[i],(padc[i]&0x0fff),adcnr); 
	}
	if(((padc[i]>>24)&0x7) ==0){     // if not then they are not data but header words.
	    adcchan=((padc[i])>>16)&0x1f;
            adcchan=adcchan + adcnr*32;
      	    adc[adcchan] =(float)(padc[i]&0x0fff);
            //printf("raw data 0x%08x -> chan %d data %d adcnr %i words %d \n",padc[i],adcchan,(padc[i]&0x0fff),adcnr,nwords);

        /* fill ADC histos */

           if(adcchan<32) 
	     {
		hADC2DModule[0]->Fill(adc[adcchan],adcchan);
		Rings[adcchan]=adc[adcchan];
		if(adcchan<16) Ring1[adcchan]=adc[adcchan];	
		else  Ring2[adcchan-16]=adc[adcchan];	
	     }  
           else if(adcchan<64) 
	     {
		hADC2DModule[1]->Fill(adc[adcchan],adcchan-32);
		Rings[adcchan]=adc[adcchan];
		if(adcchan<48) Ring3[adcchan-32]=adc[adcchan];	
		else  Ring4[adcchan-48]=adc[adcchan];	
	     }
           else if(adcchan<96) 
	     {
		hADC2DModule[2]->Fill(adc[adcchan],adcchan-64);
		if(adcchan<80) {
			Ring5[adcchan-64]=adc[adcchan];	
			Rings[adcchan]=adc[adcchan];
		}
		else if (adcchan<88) {
			Sector1[adcchan-80]=adc[adcchan];	
			Sectors[adcchan-80]=adc[adcchan];	
			}
		else if (adcchan<96) {
			Sector2[adcchan-88]=adc[adcchan];	
			Sectors[adcchan-80]=adc[adcchan];	
			}
	     }
           else if(adcchan<128) 
	     {
		hADC2DModule[3]->Fill(adc[adcchan],adcchan-96);
		if(adcchan<104) {
			Sector3[adcchan-96]=adc[adcchan];	
			Sectors[adcchan-80]=adc[adcchan];	
			}
		else if (adcchan<112) {
			Sector4[adcchan-104]=adc[adcchan];	
			Sectors[adcchan-80]=adc[adcchan];	
			}
		else if (adcchan<120) {
			Sector5[adcchan-112]=adc[adcchan];	
			Sectors[adcchan-80]=adc[adcchan];	
			}
		else if (adcchan<128) NaI[adcchan-120]=adc[adcchan];	

	     }
           else if(adcchan<160) hADC2DModule[4]->Fill(adc[adcchan],adcchan-128);   


	}

   }
   //adcevtcount=padc[33]&0xfffff;  // take event counter in the trailer, the 34th word, to 
  				    // f-plane to compare to TDC counter 

   /* fill variables for tree */
   //for (i = 0; i < 32; i++){
     for(i=0; i<128;i++){
     if (adc[i] > 0.0)                   // if ( adc[i] > (float) adc_param.histogram_threshold )
         ADC[i]=adc[i];     
   }  


	for(int i=0; i<80; i++)//Loop over rings
	{
		//if(i<48)GetK600vsSiPA()->Fill(X1pos,Rings[i]);
		for(int j=0;j<40;j++)//Loop over sectors
		{
		  if(Rings[i]>200 && Sectors[j]>250) hSiFBHitPattern->Fill(i,j);
		  //if(i<48 && j<24)GetSiFBADC()->Fill(Rings[i],Sectors[j]);
		}
		/*
		for(int j=0;j<80;j++)//Loop over rings
		{
		  if(Rings[i]>200)GetSiADCTDCHitPattern()->Fill(i,j);
		  if(i<48 && Rings[i]>200)GetSiADCTDCValues()->Fill(Rings[i],RingsTDC[j]);

		  if(i>=48 && i<64 && j>=64 && Rings[i]>200 && Rings[j]>200)GetSidE_EADC()->Fill(Rings[i],Rings[j]);
		}
		*/
	}



 
   return SUCCESS;
}







/*
   for (i = 0; i < 34; i++){
        //printf("-------raw data 0x%08x  Nr of words %d \n",padc[i],nwords); 
	if(((padc[i]>>24)&0x7) ==0){     // if not then they are not data but header words.
	    adcchan=((padc[i])>>16)&0x1f; 
      	    adc[adcchan+0*32] =(float)(padc[i]&0x0fff);
            //printf("raw data 0x%08x -> chan %d data %d adcnr 1  words %d \n",padc[i],adcchan,(padc[i]&0x0fff),nwords);
	}
   }
   for (i = 34; i < 70; i++){
        //printf("-------raw data 0x%08x  Nr of words %d \n",padc[i],nwords); 
	if(((padc[i]>>24)&0x7) ==0){     // if not then they are not data but header words.
	    adcchan=((padc[i])>>16)&0x1f; 
      	    adc[adcchan+1*32] =(float)(padc[i]&0x0fff);
            //printf("raw data 0x%08x -> chan %d data %d adcnr 2  words %d \n",padc[i],adcchan,(padc[i]&0x0fff),nwords);
	}
   }
*/

