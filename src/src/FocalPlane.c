/********************************************************************\
  Name:         FocalPlane.c
  Created by:   Retief Neveling
  Contents:     Focal-plane detector stuff

  This is the first step in a major reorganization of the analyzer code
  where all the focalplane code will be reorganized in a similar 
  fashion to the code for ancilliary as created by Phil Adsley.
  This main aim now is to get the vast majority of the vdc code
  out of what used to be f-plane.c, and which is not called main.c

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

/* home-made includes */
#include "Parameters.h"


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
void setupchannel2wireXoldXold(unsigned int chan2wire[])
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

  for(input=0;input<896;input++) chan2wire[input]=-1;

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
		  chan2wire[24]=0;
		  chan2wire[25]=1;
		  chan2wire[26]=2;
		  chan2wire[27]=3;
		  chan2wire[28]=4;
		  chan2wire[29]=5;
		  chan2wire[30]=6;
		  chan2wire[31]=7;			
		  for(int i=24;i<32;i++){
		    tdcchan=i;
                    //printf("chan2wire[%d]  = %d  \n",tdcchan,chan2wire[tdcchan]);
                    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, chan2wire[tdcchan]);
                  }
		}
		else{
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
		    chan2wire[tdcchan]=i;
                    //printf("chan2wire[%d] = %d   \n",tdcchan, chan2wire[tdcchan]);
		  }
		}
	   }
	   else if(preampcount>21 && preampcount <35){ // wireplane X2  =================================================
	       	basecount=508;
	       	preampbase=23;
		channelstart=basecount + (preampcount-preampbase)*16;
		if(preampcount==22){
		  chan2wire[424]=500;
		  chan2wire[425]=501;
		  chan2wire[426]=502;
		  chan2wire[427]=503;
		  chan2wire[428]=504;
		  chan2wire[429]=505;
		  chan2wire[430]=506;
		  chan2wire[431]=507;			
		  for(int i=424;i<432;i++){
		    tdcchan=i;
//                     printf("chan2wire[%d]  = %d  \n",tdcchan,chan2wire[tdcchan]);
                    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, chan2wire[tdcchan]);
                  }
		}
		else{
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
		    chan2wire[tdcchan]=i;
//                     printf("chan2wire[%d] = %d   \n",tdcchan, chan2wire[tdcchan]);
		  }
		}
	   }
	   preampcount++;	
      }
  }
}

/* ---------------------------------------------------------------------------------------*/
void setupchannel2wireXoldXU(unsigned int chan2wire[])
// Mapping of wires to channels when using 1 old VDC and 1 new VDC (placed with first wireplane being the X-wires)
// See camac-vme-cabling.xls
{
  printf("setupchannel2wireXoldXU()\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  int tdcchan;

  for(input=0;input<896;input++) chan2wire[input]=-1;

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
		  chan2wire[24]=0;
		  chan2wire[25]=1;
		  chan2wire[26]=2;
		  chan2wire[27]=3;
		  chan2wire[28]=4;
		  chan2wire[29]=5;
		  chan2wire[30]=6;
		  chan2wire[31]=7;			
		  for(int i=24;i<32;i++){
		    tdcchan=i;
                    //printf("chan2wire[%d]  = %d  \n",tdcchan,chan2wire[tdcchan]);
                    //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, chan2wire[tdcchan]);
                  }
		}
		else{
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
		    //tdcchan=(tdcmodulecounter*128) +  ((preampcount+1)-7*tdcmodulecounter)*16 +(i-channelstart);
		    chan2wire[tdcchan]=i;
                    //printf("channelstart %d;  tdcmodule %d ;  preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, tdcmodulecounter, preampcount, tdcchan, chan2wire[tdcchan]);
		  }
		}
	   }
	   else if(preampcount>12 && preampcount <22){ // wireplane U1  =================================================
	       	basecount=300;
	       	preampbase=13;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  chan2wire[tdcchan]=i;
		  //printf("chan2wire[%d] = %d  \n",tdcchan, chan2wire[tdcchan]);
		}
	   }
	   else if(preampcount>21 && preampcount <35){ // wireplane X2  =================================================
	       	basecount=500;
	       	preampbase=22;
		channelstart=basecount+(preampcount-preampbase)*16;

		if(preampcount==(35-1)){		    
		  chan2wire[624]=697;
		  chan2wire[625]=696;
		  chan2wire[626]=695;
		  chan2wire[627]=694;
		  chan2wire[628]=693;
		  chan2wire[629]=692;
		  //for(int i=624;i<640;i++){
		  //    printf("chan2wire %d   tdcchan= %d  \n",chan2wire[i],i);
		  //}
		}
		else {
		  int counter=1;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    chan2wire[tdcchan]=(channelstart+16) - counter;
		    counter++;
  		    //printf("chan2wire[%d] = %d  \n",tdcchan, chan2wire[tdcchan]);
		  }
		} 
	   }
	   else if(preampcount>34 && preampcount <44){// wireplane U2  =================================================
	       	basecount=800;
	       	preampbase=35;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  chan2wire[tdcchan]=i;
		  //printf("chan2wire[%d] = %d  \n",tdcchan, chan2wire[tdcchan]);
		}
	   }
	   preampcount++;	
      }
  }

  //for(int i=0;i<300;i++){
  //     printf("--------------------- chan2wire[%d] = %d   \n",i, chan2wire[i]);
  //}



}

/* ---------------------------------------------------------------------------------------*/
void setupchannel2wireXUXU(unsigned int chan2wire[])
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

  for(input=0;input<896;input++) chan2wire[input]=-1;

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
		  chan2wire[224]=197;
		  chan2wire[225]=196;
		  chan2wire[226]=195;
		  chan2wire[227]=194;
		  chan2wire[228]=193;
		  chan2wire[229]=192;
		  for(int i=230;i<240;i++){
		    chan2wire[i]=0;
		  }
		  //for(int i=224;i<240;i++){
		  //    printf("chan2wire %d   tdcchan= %d  \n",chan2wire[i],i);
		  //}
		}
		else {
		  int counter=1;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    chan2wire[tdcchan]=(channelstart+16) - counter;
		    counter++;
		    //printf("chan2wire %d   tdcchan= %d  \n",chan2wire[tdcchan],tdcchan);
		  }
		} 
	   }
	   else if(preampcount<22){ // wireplane U1  =================================================
	       	basecount=300;
	       	preampbase=13;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  chan2wire[tdcchan]=i;
		  //printf("chan2wire %d   tdcchan= %d  \n",chan2wire[tdcchan],tdcchan);
		}
	   }
	   else if(preampcount<35){ // wireplane X2  =================================================
	       	basecount=500;
	       	preampbase=22;
		channelstart=basecount+(preampcount-preampbase)*16;

		if(preampcount==(35-1)){		    
		  chan2wire[624]=697;
		  chan2wire[625]=696;
		  chan2wire[626]=695;
		  chan2wire[627]=694;
		  chan2wire[628]=693;
		  chan2wire[629]=692;
		  for(int i=230;i<240;i++){
		    chan2wire[i]=0;
		  }
		  //for(int i=624;i<640;i++){
		  //    printf("chan2wire %d   tdcchan= %d  \n",chan2wire[i],i);
		  //}
		}
		else {
		  int counter=1;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    chan2wire[tdcchan]=(channelstart+16) - counter;
		    counter++;
		    //printf("chan2wire %d   tdcchan= %d  \n",chan2wire[tdcchan],tdcchan);
		  }
		} 
	   }
	   else if(preampcount<44){// wireplane U2  =================================================
	       	basecount=800;
	       	preampbase=35;
		channelstart=basecount+(preampcount-preampbase)*16;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  chan2wire[tdcchan]=i;
		  //printf("chan2wire %d   tdcchan= %d  \n",chan2wire[tdcchan],tdcchan);
		}
	   }
	   preampcount++;	
      }
  }
}

/* ---------------------------------------------------------------------------------------*/
void setupchannel2wireXUXold(unsigned int chan2wire[])
// hack the mapping of wires to channels with XU and then old X chamber
//
{
  printf("setupchannel2wireXUXold()\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  //int channel=0;
  int tdcchan;
  
  for(input=0;input<896;input++) chan2wire[input]=-1;
  
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
	    chan2wire[224]=197;
	    chan2wire[225]=196;
	    chan2wire[226]=195;
	    chan2wire[227]=194;
	    chan2wire[228]=193;
	    chan2wire[229]=192;
	    for(int i=230;i<240;i++){
	      chan2wire[i]=0;
	    }
	    //for(int i=224;i<240;i++){
	      //    printf("chan2wire %d   tdcchan= %d  \n",chan2wire[i],i);
	      //}
	  }
	  else {
	    int counter=1;
	    for(int i=channelstart;i<channelstart+16;i++){
	      tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
	      chan2wire[tdcchan]=(channelstart+16) - counter;
	      counter++;
	      //printf("chan2wire %d   tdcchan= %d  \n",chan2wire[tdcchan],tdcchan);
	    }
	  } 
      }
      else if(preampcount<22){ // wireplane U1  =================================================
	  basecount=300;
	  preampbase=13;
	  channelstart=basecount+(preampcount-preampbase)*16;
	  for(int i=channelstart;i<channelstart+16;i++){
	    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
	    chan2wire[tdcchan]=i;
	    //printf("chan2wire %d   tdcchan= %d  \n",chan2wire[tdcchan],tdcchan);
	  }
      }
      else if(preampcount>21 && preampcount <35){ // wireplane X2  =================================================
	  basecount=508;
	  preampbase=23;
	  channelstart=basecount + (preampcount-preampbase)*16;
	  if(preampcount==22){
	    chan2wire[424]=500;
	    chan2wire[425]=501;
	    chan2wire[426]=502;
	    chan2wire[427]=503;
	    chan2wire[428]=504;
	    chan2wire[429]=505;
	    chan2wire[430]=506;
	    chan2wire[431]=507;            
	    //for(int i=424;i<432;i++){
	      //tdcchan=i;
	      //printf("chan2wire[%d]  = %d  \n",tdcchan,chan2wire[tdcchan]);
	      //printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, chan2wire[tdcchan]);
	    //}
	  }
	  else{
	    for(int i=channelstart;i<channelstart+16;i++){
	      tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
	      chan2wire[tdcchan]=i;
	      //printf("chan2wire[%d] = %d   \n",tdcchan, chan2wire[tdcchan]);
	    }
	  }
    }
    preampcount++;   
  }
}
}


/* ---------------------------------------------------------------------------------------*/
void setupchannel2wireUXUX(unsigned int chan2wire[])
// hack the mapping of wires to channels for UXUX setup
// See camac-vme-cabling.xls
// This is fairly complicated due to design mistakes on the PCB's
//
// chan 0-207   = X wires VDC1
// chan 300-443 = U wires VDC1
// chan 500-707 = X wires VDC2
// chan 800-943 = U wires VDC2

{
   printf("setupchannel2wireUXUX()\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  //int channel=0;
  int tdcchan;

  for(input=0;input<896;input++) chan2wire[input]=-1;

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
		  chan2wire[16]=10;
		  chan2wire[17]=11;
		  chan2wire[18]=12;
		  chan2wire[19]=13;
		  chan2wire[20]=14;
		  chan2wire[21]=15;
		  for(int i=22;i<31;i++){
		    chan2wire[i]=0;
		  }
		  channelstart=basecount+(preampcount-preampbase)*16;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  }
		}
		else {
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    chan2wire[tdcchan]=i;
 //printf("tdcchan %d   i %d  \n",chan2wire[tdcchan],i);
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
		  chan2wire[tdcchan]=(channelstart+16) - counter;
		  counter++;
		}
	   }
	   else if(preampcount<35){ // wireplane X2  =================================================
	       	basecount=500;
	       	preampbase=22;
		channelstart=basecount+(preampcount-preampbase)*16;
		if(preampcount==22){		    
		  chan2wire[416]=510;
		  chan2wire[417]=511;
		  chan2wire[418]=512;
		  chan2wire[419]=513;
		  chan2wire[420]=514;
		  chan2wire[421]=515;
		  for(int i=422;i<431;i++){
		    chan2wire[i]=0;
		  }
		  channelstart=basecount+(preampcount-preampbase)*16;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    //printf("tdc %d input %d channel %d wire %d\n",tdcmodulecounter,input,tdcchan,chan2wire[tdcchan]);
		  }
		}
		else {
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    chan2wire[tdcchan]=i;
		    //printf("tdc %d input %d channel %d wire %d\n",tdcmodulecounter,input,tdcchan,chan2wire[tdcchan]);
		  }
		}	   }
	   else if(preampcount<44){// wireplane U2  =================================================
	       	basecount=800;
	       	preampbase=35;
		channelstart=basecount+(preampcount-preampbase)*16;
		int counter=1;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  chan2wire[tdcchan]=(channelstart+16) - counter;
		  counter++;
		}
	   }
	   preampcount++;	
      }
  }
}


/* ---------------------------------------------------------------------------------------*/
void setupchannel2wirePR170(unsigned int chan2wire[])
// Specific for the Oct 2011 beamtime of PR170:
// The detectors were set up with an old VDC and then new UX/
// Unfortunately the cabling was such that they look like 
// VDC1 a new UX, then VDC2 the old VDC.
// To work around this I will first take the data as given to the TDCs
// i.e. I assume VDC1 ix UX VDC is old X. Then afterwars I will swap things around.

// chan 0-207   = X wires VDC1
// chan 300-443 = U wires VDC1
// chan 500-707 = X wires VDC2
// chan 800-943 = U wires VDC2

{
  printf("setupchannel2wirePR170()\n");
  int input,tdcmodulecounter,preampnum,channelstart,basecount;
  int preampcount=0;
  int preampbase=0;
  //int channel=0;
  int tdcchan;

  for(input=0;input<896;input++) chan2wire[input]=-1;

  for(tdcmodulecounter=0;tdcmodulecounter<8;tdcmodulecounter++){
      for(input=1;input<8;input++){
	   channelstart=0;
	   preampnum=(tdcmodulecounter*7)+input;
	   //printf("tdc %d   input %d   preamp %d  \n",tdcmodulecounter,input,preampnum);
	   if(preampcount<13) { // wireplane X1  =================================================
	      	basecount=500;
	       	preampbase=0;
		channelstart=basecount+(preampcount-preampbase)*16;
		if(preampcount==0){		    
		  chan2wire[16]=510;
		  chan2wire[17]=511;
		  chan2wire[18]=512;
		  chan2wire[19]=513;
		  chan2wire[20]=514;
		  chan2wire[21]=515;
		  for(int i=22;i<31;i++){
		    chan2wire[i]=0;
		  }
		  channelstart=basecount+(preampcount-preampbase)*16;
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  }
		}
		else {
		  for(int i=channelstart;i<channelstart+16;i++){
		    tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		    chan2wire[tdcchan]=i;
		  }
		}
	   }
	   else if(preampcount<22){ // wireplane U1  =================================================
	       	basecount=800;
	       	preampbase=13;
		channelstart=basecount+(preampcount-preampbase)*16;
		int counter=1;
		for(int i=channelstart;i<channelstart+16;i++){
		  tdcchan=(tdcmodulecounter*128) + (input*16) + i-channelstart;
		  chan2wire[tdcchan]=(channelstart+16) - counter;
		  counter++;
		}
	   }

           else if(preampcount>21 && preampcount <35){ // wireplane X2  =================================================
	  	basecount=8;
	  	preampbase=23;
	  	channelstart=basecount + (preampcount-preampbase)*16;
	  	if(preampcount==22){
	    		chan2wire[424]=0;
		        chan2wire[425]=1;
	    		chan2wire[426]=2;
	    		chan2wire[427]=3;
	    		chan2wire[428]=4;
	    		chan2wire[429]=5;
	    		chan2wire[430]=6;
	    		chan2wire[431]=7;            
	    		//for(int i=424;i<432;i++){
	      			//tdcchan=i;
	      			//printf("chan2wire[%d]  = %d  \n",tdcchan,chan2wire[tdcchan]);
	      			//printf("channelstart %d;   preampcount %d ;   chan2wire[%d] = %d   \n",channelstart, preampcount, tdcchan, chan2wire[tdcchan]);
	    		//}
	  	}
	  	else{
	    		for(int i=channelstart;i<channelstart+16;i++){
	      			tdcchan=(tdcmodulecounter*128) +  (input*16) +(i-channelstart);
	      			chan2wire[tdcchan]=i;
	      			//printf("chan2wire[%d] = %d   \n",tdcchan, chan2wire[tdcchan]);
	    		}
	  	}
    	   }
    	   preampcount++;   
  	}
  }


  //for(int i=0;i<900;i++){
  //	printf(" raw chan %d    chan2wire %d  \n",i,chan2wire[i]);
  //}


}





//---------------------------------------------------------------------------------------------//
void getRefTimes(int time[], int ntdc, DWORD ptdc[])
//void getRefTimes(int time[], int ntdc, DWORD ptdc[],int doubletdc[],int doublecnt[])
// 1st chan into each TDC is a copy of the trigger. Find this for each module for each event.
// Without this you cannot get accurate time determination.
{
   memset(time,0,8*sizeof(int));   
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

      // Determine tentative ray: straight line through first and last wires
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
   Double_t averagedd=0;
   Double_t sstot=0,ssres=0;
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
 
   averagedd=sum_z/sum_0;

   // from this loop we obtain really sum of squares of residuals and total sum of squares
   for(i=0;i<wire_num;++i){
     if(dd[i]!=dd[wireID_min]){
       ssres += pow(dd[i]-(a1*X_WIRESPACE*(wire[i])+b1),2); 
       ++wirechisq;
       sstot += pow(dd[i]-averagedd,2);
     }
   }

   //chisq_v1 /= wirechisq;
   chisq_v1 = 1- ssres/sstot;

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
void CalcCorrX(Double_t X, Double_t Y, Double_t ThetaSCAT, Double_t *Xcorr)
//lineshape correction when you have a well defined thetaSCAT
{
  double result = 0;
  extern int NXThetaCorr;
  extern double *XThetaCorr;

  extern int NXThetaXCorr;
  extern double *XThetaXCorr;

  extern int NXThetaXLoffCorr;
  extern double *XThetaXLoffCorr;
  extern double X_LSOffset;

  extern int NXY1Corr;
  extern double *XY1Corr;
  
  //printf("XLineshapeOffset = %f\n",X_LSOffset);
  //printf("X to start with: %f\n",X);

  //printf("X to start with: %f\n",X);

  extern int NXTOFCorr;
  extern double *XTOFCorr;
  extern double TOF_LSOffset;
 
  //printf("XLineshapeOffset = %f\n",X_LSOffset);
  //printf("X to start with: %f\n",X);

  //printf("X to start with: %f\n",X);

  *Xcorr = 0;
  for(int i=0;i<NXThetaCorr;i++){
    if(i==0)result = X;
    if(i>0)result += XThetaCorr[i] * pow(ThetaSCAT,i);//Correct the rigidity based on the ThetaSCAT value
  }
  //printf("Xcorr from ThetaCorr: %f\n",result);

  for(int i=0;i<NXThetaXCorr;i++){
    if(i==0)result = result;
    if(i>0)result += XThetaXCorr[i] * pow(ThetaSCAT,i) * pow(X,i);
  }
  //printf("Xcorr from ThetaXCorr: %f\n",result);


  for(int i=0;i<NXThetaXLoffCorr;i++){
    if(i==0)result = result;
    if(i>0)result += XThetaXLoffCorr[i] * pow(ThetaSCAT,i) * (X-X_LSOffset);
  }
  //printf("Xcorr from ThetaXLoffCorr: %f\n",result);

  //At this point, result is X1posC after the ThSCAT correction
  for(int i=0;i<NXY1Corr;i++){
      if(i==0)result = result;
      if(i>0)result += XY1Corr[i] * pow(Y,i);
  }
  //printf("Xcorr from YCorr: %f\n",result);
  //printf("------------------------------------------\n");


  *Xcorr = result;
}


//--------------------------------------------------------------------------------------
void CalcCorrXTOF(Double_t X, Double_t Y, Double_t TOF, Double_t *Xcorr)
//lineshape correction when using only 1 detector
{

  double result = 0;

  extern int NXY1Corr;
  extern double *XY1Corr;

  extern int NXTOFCorr;
  extern double *XTOFCorr;
  extern double TOF_LSOffset;
 
  //printf("XLineshapeOffset = %f\n",X_LSOffset);

  //At this point, result is X1posC after the ThSCAT correction
  for(int i=0;i<NXY1Corr;i++){
      if(i==0)result = X;
      if(i>0)result += XY1Corr[i] * pow(Y,i);
  }
  //printf("Xcorr from YCorr: %f\n",result);
  //printf("------------------------------------------\n");

  for(int i=0;i<NXTOFCorr;i++){
    if(i==0)result = result;
    if(i>0)result += XTOFCorr[i] * pow(TOF-TOF_LSOffset,i);
  }
  //printf("Xcorr from ThetaXLoffCorr: %f\n",result);

  *Xcorr = result;
}





//--------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------
double CalcTfromXcorr(double Xcorr, double mass)
{
  double T = 0;

  double rig = CalcQBrho(Xcorr);

  double p = rig * TMath::C()/1e9; //to obtain the momentum in MeV/c if rigidity calculated with SPANC
  //std::cout << "p3: " << p3 << std::endl;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
  //std::cout << "T3: " << T << std::endl;
  return T;
}

//--------------------------------------------------------------------------------------
double CalcTfromRigidity(double rig, double mass)
{
  double T = 0;

  double p = rig * TMath::C()/1e9; //to obtain the momentum in MeV/c if rigidity calculated with SPANC
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
}

//--------------------------------------------------------------------------------------
double CalcTfromP(double p, double mass)
{
  double T = 0;
  T = sqrt(pow(p,2.) + pow(mass,2.)) - mass;
}

//--------------------------------------------------------------------------------------
double CalcExDirect(double Xcorr)
{
  double result = 0;
  result = 31.3029 - 0.0299994*Xcorr - 1.54103e-6*pow(Xcorr,2.);
  return result;
}

//--------------------------------------------------------------------------------------
double CalcEx(double Xcorr)
{
  double exE = 0;
  extern double T1;
  double  T2 = 0, T3 = 0, T4 = 0;//Energies in MeV

  double p1, p2, p3, p4; //Momenta for each particle

  extern double *masses;//This is a pointer array containing the information on the particle masses involved in the reaction

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
  p3 = CalcQBrho(Xcorr) * TMath::C()/1e9;
  //std::cout << "p3: " << p3 << std::endl;
  T3 = CalcTfromXcorr(Xcorr,masses[2]);
  //std::cout << "T3: " << T3 << std::endl;
  double Q = masses[0] + masses[1] - masses[2] -masses[3];

  if(theta3 == 0)
    {
      theta4 = 0;
      
      p4 = p1 - p3;
      T4 = sqrt(p4*p4 + masses[3]*masses[3]) - masses[3];
      exE = T1 - T3 - T4 + Q;
    }
  else
    {
      theta4 = 180./TMath::Pi()*atan(sin(theta3*TMath::Pi()/180.)/(p1/p3 - cos(theta3*TMath::Pi()/180.)));
      //std::cout << "theta4: " << theta4 << std::endl;
      p4 = p3 * sin(theta3*TMath::Pi()/180.)/sin(theta4*TMath::Pi()/180.);
      T4 = CalcTfromP(p4,masses[3]);
      //std::cout << "T4: " << T4 << std::endl;
      exE = T1 - T3 - T4 + Q;
    }
  //std::cout << "exE: " << exE << std::endl;
  if(exE<0)exE=0;
  if(Xcorr>800)exE=0;
  return exE;
}

//--------------------------------------------------------------------------------------
double CorrectEx(double mEx)
{
  extern double ExCorrection;

  double result = 0;
  result = mEx - ExCorrection;
  return result;
}

//--------------------------------------------------------------------------------------
double CalcYFPforUX(double x, double u, double thFP)
{
  double sinu=0.766044443; //sin(U_WIRE_ANGLE/57.2957); 
  double tanu=1.191753593; //tan(U_WIRE_ANGLE/57.2957);
  double tmp1,tmp2;
  double tanfp=tan(thFP/57.2957);
  double y=0;

// for UX configuration. See RN K600 book6 p20-23
  tmp1=(u*tanfp+sinu*16);    
  tmp2=sinu*tanfp;
  y=-1*((tmp1/tmp2-x)*tanu+76.27);  // the 76.27 is the offset due to first u and x wires not sharing the same origin
  //y=1*((tmp1/tmp2-x)*tanu+76.27 -50.);  // the 76.27 is the offset due to first u and x wires not sharing the same origin
				      // the -50 is to put it around zero
 
  //printf("x=%f  u=%f  ThFP=%f  sinu=%f  tanu=%f tanThFP=%f  y=%f \n",x,u,thFP,sinu,tanu,tanfp,(x-tmp1/tmp2)*tanu);
  //printf("%f %f %f %f %f %f %f \n",x,u,thFP,sinu,tanu,tanfp,(x-tmp1/tmp2)*tanu);
  return y;
}

//--------------------------------------------------------------------------------------
double CalcYFP(double x, double u, double thFP)
{
  double sinu=0.766044443; //sin(U_WIRE_ANGLE/57.2957); 
  double tanu=1.191753593; //tan(U_WIRE_ANGLE/57.2957);
  double tmp1,tmp2;
  double tanfp=tan(thFP/57.2957);
  double y=0;

// for UX configuration. See RN K600 book6 p20-23
//  tmp1=(u*tanfp+sinu*16);    
//  tmp2=sinu*tanfp;
//  *y=(tmp1/tmp2-x)*tanu+76.27 -50;  // the 76.27 is the offset due to first u and x wires not sharing the same origin
				      // the -50 is to put it around zero
// for XU configuration
  tmp1=(u*tanfp-sinu*16);    
  tmp2=sinu*tanfp;
  y=-1*((tmp1/tmp2-x)*tanu+29.21);        // added 3mm to previous offset of 26.21mm
 
  //printf("x=%f  u=%f  ThFP=%f  sinu=%f  tanu=%f tanThFP=%f  y=%f \n",x,u,thFP,sinu,tanu,tanfp,(x-tmp1/tmp2)*tanu);
  //printf("%f %f %f %f %f %f %f \n",x,u,thFP,sinu,tanu,tanfp,(x-tmp1/tmp2)*tanu);
  return y;
}


//--------------------------------------------------------------------------------------
double CalcThetaScat(double X1, double ThFP)
{

/*  
  extern int NThFPtoThSCAT;
  extern double *ThFPtoThSCAT;

  double result =  ThFP*(ThFPtoThSCAT[0] + ThFPtoThSCAT[1]*X1 +  ThFPtoThSCAT[2]*X1*X1) 
		      + (ThFPtoThSCAT[3] + ThFPtoThSCAT[4]*X1 +  ThFPtoThSCAT[5]*X1*X1);
  return result;
*/

  double result = 0;
  extern int NThFPSCATOffset;
  extern double *ThFPSCATOffset;
  extern int NThFPSCATSlope;
  extern double *ThFPSCATSlope;

  //printf("anglefp = %f\n",ThFP);  

  for(int i=0;i<NThFPSCATOffset;i++)
  {
    if(i==0) result = 0;
    if(i>0)  result += ThFPSCATOffset[i] * pow(X1,i-1); 
    //printf("NTHFPSCATOffset  %i  and parameter %f  and result %f at X1= %f \n",i, ThFPSCATOffset[i],result,X1);
  }

  for(int i=0;i<NThFPSCATSlope;i++)
  {
    if(i==0) result = result;
    if(i>0)  result += ThFP * ThFPSCATSlope[i] * pow(X1,i-1);
    //printf("NTHFPSCATSlope  %i  and parameter %f  and result %f at X1 = %f \n",i, ThFPSCATSlope[i],result,X1);
  }

  //printf("\n\n");
  return result;


}

//--------------------------------------------------------------------------------------
double CalcPhiScat(double X1, double ThFP, double Y1)

{
  double result = 0;
  result = Y1 * (0.108+0.00086*ThFP) - (0.6097+3.99e-4*X1);
  return result;
}

//--------------------------------------------------------------------------------------
double CalcTheta(double X1, double ThFP, double Y1)
{
  extern double theta3;//K600 scattering angle

  double ThetaSCAT = CalcThetaScat(X1,ThFP);

  double PhiSCAT = CalcPhiScat(X1, ThFP, Y1);
  
  double result = -1;

  result = sqrt(pow(ThetaSCAT + theta3,2.) + pow(PhiSCAT,2.));
}

//--------------------------------------------------------------------------------------
double CalcThetaFP(double X1, double X2)
// Calculate the horizontal component of the particle track throught the focal plane
{
   double x;
   double result =-1;
   extern double z_x1x2, x_x1x2;

   x=(X2 + x_x1x2) - X1 ;               
   result=57.29578*atan(z_x1x2/x);
   //printf("z_x1x2=%f x_x1x2 = %f  \n",z_x1x2,x_x1x2);
  
   return result;
}

//--------------------------------------------------------------------------------------
double CalcPhiFP(double X1, double Y1, double X2, double Y2,  double thFP)
// Calculate the vertical component of the particle track throught the focal plane
{
   double y;
   double result=-1;
   extern double z_x1x2, x_x1x2;

   y= Y2 - Y1 ;               
   result=57.29578*atan(y*sin(thFP/57.29578)/z_x1x2);
   //printf("y1=%f y2=%f z_x1x2=%f ThFP=%f phi=%f\n",Y1,Y2,z_x1x2,thFP,57.29578*atan(y/(z_x1x2/sin(thFP/57.29578))));
   return result;

}

