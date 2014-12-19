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
//float ADC[128];
extern float *ADC;
extern int ADCModules;
extern int ADCsize;
extern double *ADCOffsets, *ADCGains;
int adcevtcount;
float NaI[8];//, Plastic[8];
float Nside[80],Pside[80];
//float PsideTDC[80];
float Pside1[16],Pside2[16],Pside3[16],Pside4[16];
float Nside1[16],Nside2[16],Nside3[16],Nside4[16];


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

//    hADC2DModule = new TH2F*[ADCModules];
   
   for(int counter=0;counter<5;counter++){
	  sprintf(name,"hADC2DModule%d",counter);
	  sprintf(title,"hADC2DModule %d ",counter);
          hADC2DModule[counter]=H2_BOOK(name,title,4096,0,16384,32,0,32);
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
   float *adc = new float[32*ADCModules];  
//    printf("adc initialisation: %d\n",32*ADCModules);
   int adcchan,adcnr;
   extern int adc_counter1, adc_counter2;   // defined; declared in analyzer.c
 

//    for(int i = 0; i < 16 ; i++) { 
//      Pside1[i]=0;
//      Pside2[i]=0;
//      Pside3[i]=0;
//      Pside4[i]=0;
//    }
//    for(int i = 0; i < 16 ; i++) { 
//      Nside1[i]=0;
//      Nside2[i]=0;
//      Nside3[i]=0;
//      Nside4[i]=0;
//      NaI[i]=0;
//    }

  	
   /* look for ADC0 bank, return if not present */
   nwords=bk_locate(pevent, "ADC0", &padc);
   adc_counter1++;
   if (nwords==0){
      adc_counter2++;
      return 1;
   }
//     printf("adc.c: L185\n");     
   for (i = 0; i < nwords; i++){
        //printf("-------raw data 0x%08x  Nr of words %d \n",padc[i],nwords); 
        if(((padc[i]>>24)&0xff) ==0xfd) {
           if((padc[i]&0xf) ==0) adcnr=0;  //printf(" adc nr 0 \n");}
           if((padc[i]&0xf) ==1) adcnr=1; // printf(" adc nr 1 \n");}
           if((padc[i]&0xf) ==2) adcnr=2;  //printf(" adc nr 2 \n");} 
           if((padc[i]&0xf) ==3) adcnr=3; // printf(" adc nr 3 \n");}
           if((padc[i]&0xf) ==4) adcnr=4;  //printf(" adc nr 4 \n");}
	   //printf("-----raw data 0x%08x ->  data %d adcnr %i \n",padc[i],(padc[i]&0x0fff),adcnr); 
	}
	if(((padc[i]>>24)&0x7) ==0){     // if not then they are not data but header words.
	    adcchan=((padc[i])>>16)&0x1f;
            adcchan=adcchan + adcnr*32;
      	    adc[adcchan] =(float)(padc[i]&0x0fff);
            //printf("raw data 0x%08x -> chan %d data %d adcnr %i words %d \n",padc[i],adcchan,(padc[i]&0x0fff),adcnr,nwords);

        /* fill ADC histos */
//       printf("adc.c: L204\n");
           if(adcchan<32) 
	     {
		hADC2DModule[0]->Fill(ADCOffsets[adcchan]+adc[adcchan]*ADCGains[adcchan],adcchan);
// 		Pside[adcchan]=adc[adcchan];
// 		if(adcchan<16) Pside1[adcchan]=adc[adcchan];	
// 		else  Pside2[adcchan-16]=adc[adcchan];	
	     }  
           else if(adcchan<64) 
	     {
		hADC2DModule[1]->Fill(ADCOffsets[adcchan]+adc[adcchan]*ADCGains[adcchan],adcchan-32);
// 		Pside[adcchan]=adc[adcchan];
// 		if(adcchan<48) Pside3[adcchan-32]=adc[adcchan];	
// 		else  Pside4[adcchan-48]=adc[adcchan];	
	     }
           else if(adcchan<96) 
	     {
// 		hADC2DModule[2]->Fill(adc[adcchan],adcchan-64);
// 		Nside[adcchan-64]=adc[adcchan];
// 		if(adcchan<80) Nside1[adcchan-64]=adc[adcchan];	
// 		else  Nside2[adcchan-80]=adc[adcchan];	
	     }
           else if(adcchan<128) 
	     {
// 		hADC2DModule[3]->Fill(adc[adcchan],adcchan-96);
// 		Nside[adcchan-96]=adc[adcchan];
// 		if(adcchan<112) Nside3[adcchan-96]=adc[adcchan];	
// 		else  Nside4[adcchan-112]=adc[adcchan];	
	     }
//            else if(adcchan<160) hADC2DModule[4]->Fill(adc[adcchan],adcchan-128);   

	}

   }
   //adcevtcount=padc[33]&0xfffff;  // take event counter in the trailer, the 34th word, to 
  				    // f-plane to compare to TDC counter 
// printf("adc.c: L239\n");
   /* fill variables for tree */
   //for (i = 0; i < 32; i++){
     for(i=0; i<ADCsize;i++){
       ADC[i] = 0;//Clear out the ADC values - should always be resetting the values because we have pedestals but it's worth being proper about it.
     if (adc[i] > 0.0)                   // if ( adc[i] > (float) adc_param.histogram_threshold )
         ADC[i]=adc[i];     
   }  
// printf("L246\n");

// 	for(int i=0; i<80; i++)//Loop over pside
// 	{
		//if(i<48)GetK600vsSiPA()->Fill(X1pos,Pside[i]);
// 		for(int j=0;j<40;j++)//Loop over sectors
// 		{
// 		  if(Pside[i]>200 && Nside[j]>250) hSiFBHitPattern->Fill(i,j);
		  //if(i<48 && j<24)GetSiFBADC()->Fill(Pside[i],Nside[j]);
// 		}
		/*
		for(int j=0;j<80;j++)//Loop over psides
		{
		  if(Pside[i]>200)GetSiADCTDCHitPattern()->Fill(i,j);
		  if(i<48 && Pside[i]>200)GetSiADCTDCValues()->Fill(Psides[i],PsidesTDC[j]);

		  if(i>=48 && i<64 && j>=64 && Pside[i]>200 && Psides[j]>200)GetSidE_EADC()->Fill(Psides[i],Psides[j]);
		}
		*/
// 	}



//   printf("Got to SUCCESS in adc.c\n");
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

