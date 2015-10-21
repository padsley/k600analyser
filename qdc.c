/********************************************************************\

  Name:         qdc.c
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
float pad1=0,pad2=0,pad1hip=0,pad1lowp=0,pad2hip=0,pad2lowp=0;
int qdcevtcount;
int qdcevtcount2;


/*-- For ODB: from /Analyzer/Parameters/----------------------------*/
QDC_PARAM qdc_param;


/*-- Module declaration --------------------------------------------*/
INT qdc_event(EVENT_HEADER *, void *);
INT qdc_init(void);
INT qdc_bor(INT run_number);
INT qdc_eor(INT run_number);

QDC_PARAM_STR(qdc_param_str);

ANA_MODULE qdc_module = {
   "QDC",                       /* module name           */
   "Retief Neveling",           /* author                */
   qdc_event,                   /* event routine         */
   qdc_bor,                     /* BOR routine           */
   qdc_eor,                     /* EOR routine           */
   qdc_init,                    /* init routine          */
   NULL,                        /* exit routine          */
   &qdc_param,                  /* parameter structure   */
   sizeof(qdc_param),           /* structure size        */
   qdc_param_str,               /* initial parameters    */
};


/*--------------------------------------------------------------------------*/
#define H1I_BOOK(n,t,b,min,max) (h1_book<TH1I>(n,t,b,min,max))
#define H2I_BOOK(n,t,xb,xmin,xmax,yb,ymin,ymax) (h2_book<TH2I>(n,t,xb,xmin,xmax,yb,ymin,ymax))


/*------------Declarations of predefined Constants ----------------*/
const int QDC_N_BINS = 4096;
const int QDC_X_LOW  = 0;
const int QDC_X_HIGH = 4095;
extern EXP_PARAM exp_param;
extern RUNINFO runinfo;


/*-- Histogramming Data Structures ----------------------------------------*/
//static TH1I *hAdcHists[N_QDC];
//static TH1I *hPaddle1;
//static TH1I *hPaddle2;
static TH2F *hPaddlePID;


/*-- init routine --------------------------------------------------*/
INT qdc_init(void)
{

   // Paddle and TOF spectra
   //hAdcHists[0] = H1I_BOOK("hPad1HiP","Pad1HiP (hi-P side), QDC0" , QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   //hAdcHists[1] = H1I_BOOK("hPad1LowP","Pad1LowP (low-P side), QDC1" , QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   //hAdcHists[2] = H1I_BOOK("hPad2HiP","Pad2HiP (hi-P side), QDC2", QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   //hAdcHists[3] = H1I_BOOK("hPad2LowP","Pad2LowP (low-P side), QDC3", QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   //hAdcHists[4] = H1I_BOOK("hPad3HiP","Pad3HiP (hi-P side), QDC4", QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   //hAdcHists[5] = H1I_BOOK("hPad3LowP","Pad3LowP (low-P side), QDC5", QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   //hPaddle1 = H1I_BOOK("hPad1Ave", "Paddle 1 (sqrt(L*R))", QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   //hPaddle2 = H1I_BOOK("hPad2Ave", "Paddle 2 (sqrt(L*R))", QDC_N_BINS, QDC_X_LOW, QDC_X_HIGH);
   hPaddlePID   = H2_BOOK("hPad1Pad2","Paddle1(y-axis) vs Paddle2(x-axis)",  1024, 0, 4096, 1024,0 , 4096);

   return SUCCESS;
}



/*-- BOR routine ---------------------------------------------------*/
INT qdc_bor(INT run_number)
{
   return SUCCESS;
}


/*-- eor routine ---------------------------------------------------*/
INT qdc_eor(INT run_number)
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
      unsigned qdc:12; // bit0 here
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


/* ---------------------------------------------------------------------------------------*/
void v792N_printEntry(const v792N_Data* v) {
// keep for debugging purposes
switch (v->data.type) {
    case v792N_typeMeasurement:
      printf("Data=0x%08lx Measurement ch=%3d v=%6d over=%1d under=%1d\n",
	     v->raw,v->data.channel,v->data.qdc,v->data.ov,v->data.un);
      break;
    case v792N_typeHeader:
      printf("Data=0x%08lx Header geo=%2x crate=%2x cnt=%2d\n",
      	   v->raw,v->header.geo,v->header.crate,v->header.cnt);
      break;
    case v792N_typeFooter:
      printf("Data=0x%08lx Footer geo=%2x evtCnt=%7d\n",
    	   v->raw,v->footer.geo,v->footer.evtCnt);
      break;
    case v792N_typeFiller:
      printf("Data=0x%08lx Filler\n",v->raw);
      break;
    default:
      printf("Data=0x%08lx Unknown %04x\n",v->raw,v->data.type);
      break;
    }
}


/*-- event routine -------------------------------------------------*/
INT qdc_event(EVENT_HEADER * pheader, void *pevent)
{
   INT i, nwords;
   DWORD *pqdc;
   extern float *QDC;      // size has to be 32 if you use the 32chan QDC and all channel are in banks
   //   float pad1,pad2;
   int qdcchan;
   extern int qdc_counter1, qdc_counter2;		      // defined; declared in analyzer.c
	
	for(int p=0;p<32;p++)QDC[p] = 0;
	
   /* look for QDC0 bank, return if not present */
   /* In PR137 and PR138 it is called QDC0 bank */
   nwords=bk_locate(pevent, "QDC0", &pqdc);
   qdc_counter1++;
   if (nwords==0){
      qdc_counter2++;
      return 1;
   }
   for (i = 0; i < nwords; i++){
	if(((pqdc[i]>>24)&0x7) ==0){// continue; // not data but header words.
 	  //qdcchan=((pqdc[i])>>17)&0x1f;  // for 16 chan NIM QDC
  	  qdcchan=((pqdc[i])>>16)&0x1f;  // for 32 chan ECL QDC
          //printf("qdc data %d %d from 0x%08x number of words is : %d\n",qdcchan,(pqdc[i]&0x0fff),pqdc[i],nwords); 
      	  QDC[qdcchan] =(float)(pqdc[i]&0x0fff);
	}
	//v792N_printEntry((v792N_Data*)&(pqdc[i]));
   }
   qdcevtcount=pqdc[24]&0xfffff;  // take event counter in the trailer, the 34th word, to 
		  		  //f-plane to compare to TDC counter 
   //printf("qdc event nr from trailer:  %d\n",pqdc[25]&0xfffff); 
   //printf("qdc event nr from trailer:  %d\n",qdcevtcount); 
   
   qdcevtcount2=0;
   if(nwords>25)qdcevtcount2=pqdc[49]&0xfffff; 

   //printf("----- end of read of qdc --------\n");
   /* apply software gain calibration; not used now, but keep for future */
   for (i = 0; i < N_QDC; i++){
      QDC[i] *= 1.0;
   }

   /* fill QDC histos */
   //for (i = 0; i < 6; i++){
   //  if (qdc[i] > 0.0)                   // if ( qdc[i] > (float) qdc_param.histogram_threshold )
   //      hAdcHists[i]->Fill(qdc[i], 1);
   //}  
   pad1hip=QDC[0];  // pad1HiP
   pad1lowp=QDC[1];  // pad1lop
   pad2hip=QDC[2];  // pad2HiP
   pad2lowp=QDC[3];  // pad2lop

   //printf("ODB test in qdc.c : %d  \n",qdc_param.histogram_threshold);

   pad1=sqrt(QDC[0]*QDC[1]);
   pad2=sqrt(QDC[2]*QDC[3]);

   //hPaddle1->Fill(pad1);
   //hPaddle2->Fill(pad2);
   hPaddlePID->Fill(pad2,pad1);

   return SUCCESS;
}

