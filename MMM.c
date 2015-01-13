/* Sort code for MMM-type silicon detectors
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for silicon sort codes for K600 experiments.
 * 
 * The header file MMM.h defines the ADC and TDC channel mapping for the particular experiment in question. 
 * The user should modify these accordingly. How these channel mappings are defined as given in ???
 * 
 */

#include "SiliconData.h"
#include "MMM.h"
#include "K600SimOutput_std.h"
#include "ADCPedestal_PR226.h"
#include <math.h>
#include <stdlib.h>     /* abs */

extern double pi;
extern int ADCModules;
int ADCsize = 32*ADCModules;
extern int NumberOfMMM;

extern int MMMADCChannelLimits[4][4];    // see parameters.c
extern int MMMTDCChannelLimits[4][4];

TCutG *MMMFrontBackEnergyCut;


//=======================================
SiliconData *MMMSiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  // PLEASE NOTE: ntdc is the number of TDC channels with data; not the nr of data words in the TDC bank
  SiliconData *si = new SiliconData();
  //MMMInit();
  double theta1,theta2,phi1,phi2,thetarel;
  //int flaghit;

  //flaghit=0;

  //Loop over ADC and relevant TDC data and do the following:
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - worry about multihits ??
  //Only use ADC info above ADC pedestal
  //Do energy calibration for each channel 
  //Check whether there are front-back coincidences for a detector, and coincidences is inside energy cut
  //Calculate the theta and phi for the valid events

  multiTDC *mTDC = new multiTDC(ntdc, TDC_channel_import, TDC_value_import);

/*
  if(mTDC->SizeOfEvent()>0){
	for(int i=0;i<mTDC->GetSize();i++){
		if(mTDC->GetMult(i)>1){
			printf("Size of mTDC event: %i \n",mTDC->GetSize());
      			printf("channel= %i   value= %f  multiplicity= %i \n\n",mTDC->GetChannel(i), mTDC->GetValue(i),mTDC->GetMult(i) );

			for(int k=0;k<ntdc;k++){
     				if(TDC_channel_import[k]>815){
      					printf("   from non-mTDC data: channel= %i   value= %f \n",TDC_channel_import[k], TDC_value_import[k]);
     				}
  			} 
		}
     	}
  }	
*/
/*
  if(mTDC->SizeOfEvent()>0){
	printf("Size of mTDC event: %i \n",mTDC->SizeOfEvent());
	for(int i=0;i<ntdc;i++){
     		if(TDC_channel_import[i]>815){//This limits the code to only deal with the silicon timing values
      			//printf("TDC > chan 815:  channel= %i   value= %f \n",TDC_channel_import[i], TDC_value_import[i]);
     		}
  	}		 
	for(int k=0;k<mTDC->GetSize();k++){	
		if(mTDC->GetChannel(k)>815){
  			si->SetMult(mTDC->GetMult(k));
		  	//printf("double hit: TDC event size= %i : TDC event size= %i    \n",mTDC->SizeOfEvent(),mTDC->GetSize() );
		  	printf("double hit: TDC event size= %i : channel= %i  value= %f   multiplicity= %i   \n",mTDC->SizeOfEvent(),mTDC->GetChannel(k),mTDC->GetValue(k),mTDC->GetMult(k) );
		}
         }
  }
*/


/*
  printf("ntdc value where MMMSiliconSort is coded %i \n",ntdc);
  for(int i=0;i<ntdc;i++)
  {
     printf("%i TDCchan= %i TDCvalue= %f \n",i,TDC_channel_import[i],TDC_value_import[i]);  
     if(TDC_channel_import[i]>815)//This limits the code to only deal with the silicon timing values
     {
      	printf("TDC > chan815:  channel= %i   value= %f \n",TDC_channel_import[i], TDC_value_import[i]);
     }
  } 
  printf("\nRaw ADC data for the event (above 220):\n");
  for(int i=0;i<ADCsize;i++)
  {
     if(ADC_import[i]>220)   // a rough pedestal test for rings
     {
	printf("chan = %i value = %f \n",i,ADC_import[i]);
     }
  }
*/

  //printf("Loop over TDCs channels > 815 starts:\n");

  for(int k=0;k<mTDC->SizeOfEvent();k++){
//  for(int k=0;k<ntdc;k++)//Loop over all TDC values - should only be a small number relative to the ADC values
//  {
    if(TDC_channel_import[k]>815)//This limits the code to only deal with the silicon timing values
    {
      //printf("  TDC selection for TDC channel %d \n", TDC_channel_import[k]);
      //printf("  TDC selection for TDC channel %d \n", mTDC->GetChannel(k));
      for(int i=0;i<ADCsize;i++)
      {
	if(ADC_import[i]>220)   // a rough pedestal test for rings
	{
	  //printf("      ADC chan %i  ADC value %f : now on to ADCTDC test\n",i,ADC_import[i]);
	  //MMMADCTDCChannelTest(i,TDC_channel_import[k]);
        
	  // Check to see that we have TDC data for MMM rings
	  // Here we effectively pin down 'i' as an MMM ring as only these have TDC values
//	  if( MMMADCTDCChannelTest(i,TDC_channel_import[k]))    
	  if( MMMADCTDCChannelTest(i,mTDC->GetChannel(k)))    
	  {
	    for(int j=0;j<ADCsize;j++)
	    {
	      if(ADC_import[j]>220)	//temporary pedestal for sectors: improve for channel dependence ...
	      {
	        double energyi = MMMEnergyCalc(i,ADC_import[i]);
	        double energyj = MMMEnergyCalc(j,ADC_import[j]);
	      
	        //Test whether the hits are in the front/ring/i and back/sector/j of same detector and if energies are good
	        if(MMMFrontBackTest(i,j,energyi,energyj,si))  // here i is locked in as 'front' and j as 'back'.
	        {
		  //flaghit++; 
		  //si->SetEnergy(0.5*(energyi+energyj));
		  si->SetEnergy(energyi);
		  si->SetTheta(MMMThetaCalc(i,j));    //PR226: effectively i is ring, j is sector
		  si->SetPhi(MMMPhiCalc(i,j));        //
//		  si->SetTime(TDC_value_import[k]);	
si->SetTime(mTDC->GetValue(k));
		  si->SetSA(MMMSACalc(i,j));        //
                  //printf("theta = %d : phi = %d,  time = %d \n",MMMThetaCalc(i,j),MMMPhiCalc(i,j),mTDC->GetValue(k));

		  si->SetDetector(MMMDetHitNumber(i,j));    //which detector was hit
		  si->SetADCChannelFront(i);
		  si->SetADCChannelBack(j); 
// 		  si->SetTDCChannelFront(TDC_channel_import[k]); 
 si->SetTDCChannelFront(mTDC->GetChannel(k));
	 	  si->SetTDCChannelBack(-1);               	 // no TDCs connected to sectors during PR226
// 		  si->SetTDCValueFront(TDC_value_import[k]);     
 si->SetTDCValueFront(mTDC->GetValue(k));
		  si->SetTDCValueBack(-1);

		  si->SetADCValueFront(ADC_import[i]);
		  si->SetADCValueBack(ADC_import[j]);
		  si->SetEnergyFront(energyi);
		  si->SetEnergyBack(energyj);
         	  si->SetRing(MMMRingCalc(i));
		  si->SetSector(MMMSectorCalc(j));

  		  si->SetMult(mTDC->GetMult(k));      // at present this multiplicity value only helps to gauge size of problem with double TDC hits

	        }
	      }
	    }
	  }
	}
      }
    }
  }
  

  si->SetHits(si->SizeOfEvent());              // here the number of hits per event are set! See SiliconData.h  


/*
  //if(flaghit>1){
  if(si->SizeOfEvent()>1){
	//printf("doublhit!!!! ");
	printf("size of Si event = %i  \n",si->SizeOfEvent());
	printf("adc front(0) back(0) %i %i\n",si->GetADCChannelFront(0),si->GetADCChannelBack(0));
	printf("adc front(1) back(1) %i %i\n",si->GetADCChannelFront(1),si->GetADCChannelBack(1));
	//printf("Detectors hit: %i and  %i\n",si->GetDetector(0),si->GetDetector(1));
	printf("Hit 1: Detector %i   Ring %i   Sector %i   TDCfront %i %i\n",si->GetDetector(0),si->GetRing(0),si->GetSector(0),si->GetTDCChannelFront(0),si->GetTDCValueFront(0));
	printf("Hit 2: Detector %i   Ring %i   Sector %i   TDCfront %i %i\n",si->GetDetector(1),si->GetRing(1),si->GetSector(1),si->GetTDCChannelFront(1),si->GetTDCValueFront(1));
	//printf("Hit 1: Ring %i and sector %i\n",si->GetRing(0),si->GetSector(0));
	//printf("Hit 2: Ring %i and sector %i\n",si->GetRing(1),si->GetSector(1));

  }
*/

  // if you have more than 1 hit calculate here the relative angle inbetween the 2 hits
  if(si->SizeOfEvent()==2)
  {
     	theta1=si->GetTheta(0);
     	theta2=si->GetTheta(1);
     	phi1=si->GetPhi(0);
     	phi2=si->GetPhi(1);
	thetarel=acos(  cos(theta1)*cos(theta2) + sin(theta1)*sin(theta2)*cos(abs(phi1-phi2))  );
	si->SetRelativeTheta(180.*thetarel/pi);
  }


  if(!si->TestEvent()) si->ClearEvent(); 
  //If the event fails for some reason, we void it and clear it here. 
  //The number of these should be logged and, ideally, should be zero. 
  //A VOIDED EVENT IS ONE IN WHICH ALL SILICON DATA ARE THROWN AWAY BECAUSE THE RESULT IS **WRONG**. 
  //There are more energy hits than theta hits, for example. IT THEY ARE HAPPENING, THEN YOU'VE DONE IT WRONG.

  mTDC->ClearEvent();
  
  delete mTDC;
  
  return si;
}



//=======================================
void MMMLoadCuts(SiliconData *si)
{
  //  gROOT->ProcessLine(".x FrontBackEnergyCut.C");
  //  si->SetFrontBackEnergyCut((TCutG*)gROOT->FindObjectAny("FrontBackEnergyCut"));
  printf("Load MMM Front-Back Energy cut\n");
  TCutG *cutg = new TCutG("FrontBackEnergyCut",8);
  cutg->SetVarX("EnergyBack");
  cutg->SetVarY("EnergyFront");
  cutg->SetTitle("Graph");
  cutg->SetFillColor(1);
  cutg->SetPoint(0,8625.54,9448.89);
  cutg->SetPoint(1,9636.42,9693.27);   //cutg->SetPoint(1,9436.42,9393.27);
  cutg->SetPoint(2,9398.71,8336.6);
  cutg->SetPoint(3,8674.39,8253.18);   //cutg->SetPoint(3,8512.39,8253.18);
  cutg->SetPoint(4,593.991,200.318);   //cutg->SetPoint(4,610.991,300.318);
  cutg->SetPoint(5,317.703,811.076);   //cutg->SetPoint(5,516.703,912.076);
  cutg->SetPoint(6,8625.54,9143.01);
  cutg->SetPoint(7,8625.54,9448.89);
  
  MMMFrontBackEnergyCut = cutg;
}



//=======================================
void MMMInit()//Initialise function which gets the information on the DAQ channels->Physical channels
{ 
  //   printf("Start Init\n");

     initialize_GA();
     initialize_pedestal();
//   MMMADCChannelLimits = new int*[NumberOfDetectors];
//   for(int i=0;i<NumberOfDetectors;i++)
//   {
//     MMMADCChannelLimits[i] = new int[4];
//   }
//   //   
//   //   //I set the channel address limits manually. I could do it with an automatic script but it's actually simpler this way. Get back to work!
//   //   
//   MMMADCChannelLimits[0][0] = 0;//First channel of the front of the first detector
//   MMMADCChannelLimits[0][1] = 15;//Last channel of the front of the first detector
//   MMMADCChannelLimits[0][2] = 80;//First channel of the back of the first detector
//   MMMADCChannelLimits[0][3] = 87;//Last channel of the back of the first detector
//   //   
//   MMMADCChannelLimits[1][0] = 16;
//   MMMADCChannelLimits[1][1] = 31;
//   MMMADCChannelLimits[1][2] = 88;
//   MMMADCChannelLimits[1][3] = 95;
//   //   
//   MMMADCChannelLimits[2][0] = 32;
//   MMMADCChannelLimits[2][1] = 47;
//   MMMADCChannelLimits[2][2] = 96;
//   MMMADCChannelLimits[2][3] = 103;
//   //   
//   MMMADCChannelLimits[3][0] = 48;
//   MMMADCChannelLimits[3][1] = 63;
//   MMMADCChannelLimits[3][2] = 104;
//   MMMADCChannelLimits[3][3] = 111;
//   //   
//   //   //Currently, the TDCs only exist for the fronts so we shall limit ourself to having an array size of 2 for these. If TDC fronts and backs are instrumented, this will need to change to something that looks like what is above
//   //   
//   MMMTDCChannelLimits = new int*[NumberOfDetectors];
//   for(int i=0;i<NumberOfDetectors;i++)
//   {
//     MMMTDCChannelLimits[i] = new int[2];
//   }
//   
//   MMMTDCChannelLimits[0][0] = 128*6+48;
//   MMMTDCChannelLimits[0][1] = 128*6+48+15;
//   
//   MMMTDCChannelLimits[1][0] = 128*6+48+16;
//   MMMTDCChannelLimits[1][1] = 128*6+48+31;
//   
//   MMMTDCChannelLimits[2][0] = 128*6+48+32;
//   MMMTDCChannelLimits[2][1] = 128*6+48+47;
//   
//   MMMTDCChannelLimits[3][0] = 128*6+48+48;
//   MMMTDCChannelLimits[3][1] = 128*6+48+63;
}



//=======================================
bool MMMSuppressChannel(int Channel)//If the ADC channel is one which we wish to suppress, we do that here. Use if(Channel = 12)return true to suppress channel 12. Load of else ifs for the other suppressed channels. Then else return false.
{
  if(Channel>128)
    return true;
  else
    return false;
}



//=======================================
double MMMEnergyCalc(int Channel, double ADCValue)
{
  //define the silicon calibration parameters
  extern double silicon_offset[128];
  extern double silicon_gain[128];
  double result = silicon_offset[Channel] + silicon_gain[Channel]*ADCValue;
  //   if(Channel<64 || Channel>=80 && Channel<112)printf("EnergyCalc: %g\n",result);
  return result;
}


//=======================================
double MMMThetaCalc(int FrontChannel, int BackChannel)
{
  //double result = 180;
  //return result;

  int ring,sector;
  int detectori,detectorj;

/*
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(FrontChannel>=MMMADCChannelLimits[i][0] && FrontChannel<=MMMADCChannelLimits[i][1] && BackChannel>=MMMADCChannelLimits[i][2] && BackChannel<=MMMADCChannelLimits[i][3])
      {
	detectori = i;
      }
  }
*/

  if(FrontChannel<16) {  
	detectori=0; 
	ring=FrontChannel;
  }
  else if (FrontChannel<32){  
	detectori=1;
	ring=FrontChannel-16;
  }
  else if (FrontChannel<48) { 
	detectori=2;
	ring=FrontChannel-32;
  }
  else if  (FrontChannel<64) { 
	detectori=3;
	ring=FrontChannel-48;
  }
  	   
  if(BackChannel>79 && BackChannel<88) {  
	detectorj=0; 
	sector=BackChannel-80;
  }
  else if (BackChannel>87 && BackChannel<96){  
	detectorj=1;
	sector=BackChannel-88;
  }
  else if (BackChannel>95 && BackChannel<104) { 
	detectorj=2;
	sector=BackChannel-96;
  }
  else if  (BackChannel>103 && BackChannel<112) { 
	detectorj=3;
	sector=BackChannel-104;
  }

  if(detectori!=detectorj)  printf("something wrong: ring and sectors does not agree on detector nr. Bad frontback test");

  //printf("detectori = %i : ring = %i,  sector = %i  theta = %f \n",detectori, ring, sector,GA_TIARA[detectori][ring][sector][0]);
  return GA_TIARA[detectori][ring][sector][0]; 
}


//=======================================
double MMMPhiCalc(int FrontChannel, int BackChannel)
{
  //double result = 0;
  //return result;
  int ring,sector;
  int detectori,detectorj;

  if(FrontChannel<16) {  
	detectori=0; 
	ring=FrontChannel;
  }
  else if (FrontChannel<32){  
	detectori=1;
	ring=FrontChannel-16;
  }
  else if (FrontChannel<48) { 
	detectori=2;
	ring=FrontChannel-32;
  }
  else if  (FrontChannel<64) { 
	detectori=3;
	ring=FrontChannel-48;
  }
  	   
  if(BackChannel>79 && BackChannel<88) {  
	detectorj=0; 
	sector=BackChannel-80;
  }
  else if (BackChannel>87 && BackChannel<96){  
	detectorj=1;
	sector=BackChannel-88;
  }
  else if (BackChannel>95 && BackChannel<104) { 
	detectorj=2;
	sector=BackChannel-96;
  }
  else if  (BackChannel>103 && BackChannel<112) { 
	detectorj=3;
	sector=BackChannel-104;
  }

  if(detectori!=detectorj)  printf("something wrong: ring and sectors does not agree on detector nr. Bad frontback test");

  //printf("detectori = %i : ring = %i,  sector = %i \n",detectori, ring, sector);
  return GA_TIARA[detectori][ring][sector][1]; 
}


//=======================================
double MMMSACalc(int FrontChannel, int BackChannel)
{
  //double result = 0;
  //return result;
  int ring,sector;
  int detectori,detectorj;

  if(FrontChannel<16) {  
	detectori=0; 
	ring=FrontChannel;
  }
  else if (FrontChannel<32){  
	detectori=1;
	ring=FrontChannel-16;
  }
  else if (FrontChannel<48) { 
	detectori=2;
	ring=FrontChannel-32;
  }
  else if  (FrontChannel<64) { 
	detectori=3;
	ring=FrontChannel-48;
  }
  	   
  if(BackChannel>79 && BackChannel<88) {  
	detectorj=0; 
	sector=BackChannel-80;
  }
  else if (BackChannel>87 && BackChannel<96){  
	detectorj=1;
	sector=BackChannel-88;
  }
  else if (BackChannel>95 && BackChannel<104) { 
	detectorj=2;
	sector=BackChannel-96;
  }
  else if  (BackChannel>103 && BackChannel<112) { 
	detectorj=3;
	sector=BackChannel-104;
  }

  if(detectori!=detectorj)  printf("something wrong: ring and sectors does not agree on detector nr. Bad frontback test");

  //printf("detectori = %i : ring = %i,  sector = %i \n",detectori, ring, sector);
  return GA_TIARA[detectori][ring][sector][2]; 
}



//=======================================
bool MMMFrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si)
{
  bool result = false;
  //     printf("FrontBackTest Start\n");
  //   gROOT->ProcessLine(".x FrontBackEnergyCut.C");
  
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(FrontChannel>=MMMADCChannelLimits[i][0] && FrontChannel<=MMMADCChannelLimits[i][1] && BackChannel>=MMMADCChannelLimits[i][2] && BackChannel<=MMMADCChannelLimits[i][3])
    //If the two hits are part of the front and back of the same detector, then consider that the event might be good 
    // -> make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	if(MMMFrontBackEnergyCut->IsInside(BackEnergy,FrontEnergy))//Check to see if the front and back energies are approximately equal
	  {
	    // 	    printf("Tust\n");
	    result = true;   //They are -> it's a good event
	  }
      }
  }
  //   printf("FrontBackTest End");
  return result;
}



//=======================================
int MMMDetHitNumber(int FrontChannel, int BackChannel) 
// Determine which detector was hit
{
  int result = 0;
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(FrontChannel>=MMMADCChannelLimits[i][0] && FrontChannel<=MMMADCChannelLimits[i][1] && BackChannel>=MMMADCChannelLimits[i][2] && BackChannel<=MMMADCChannelLimits[i][3])
    //If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure 
    //that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	result = i+1;
      }
  }
  return result;
}



//=======================================
bool MMMADCTDCChannelTest(int ADCChannel, int TDCChannel)    
//Do we have TDC data for every case of ADC data?
//NOTE: assume TDC data recorded for only one side of the Si detector (this is PR226 specific)
{ 
  bool result = false;
  //printf("              Inside MMMADCTDCChannelTest:  ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(ADCChannel>=MMMADCChannelLimits[i][0] && ADCChannel<=MMMADCChannelLimits[i][1] && TDCChannel>=MMMTDCChannelLimits[i][0] && TDCChannel<=MMMTDCChannelLimits[i][1])
    //Check to see if the ADC/TDC events are in the same detector
    {
      if(TDCChannel<816) printf("Pass for invalid TDC value! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
      if(ADCChannel%16==(TDCChannel-816)%16)    // VERY SPECIFIC TO PR226 SETUP!!!
      {
        //printf("               ----- Correlation! ADCChannel: %d \t TDC Channel: %d  -----\n",ADCChannel, TDCChannel);
        result = true;
      }
    }
  }
  //if(result==false) printf("No TDC data for ADC data \n");
  return result;
}




//=======================================
int MMMRingCalc(int FrontChannel)
{
  int ring,sector;
  int detectori,detectorj;

  if(FrontChannel<16) {  
	detectori=0; 
	ring=FrontChannel;
  }
  else if (FrontChannel<32){  
	detectori=1;
	ring=FrontChannel-16;
  }
  else if (FrontChannel<48) { 
	detectori=2;
	ring=FrontChannel-32;
  }
  else if  (FrontChannel<64) { 
	detectori=3;
	ring=FrontChannel-48;
  }

  //printf("detectori = %i : ring = %i,  sector = %i  theta = %f \n",detectori, ring, sector,GA_TIARA[detectori][ring][sector][0]);
  return ring; 
}
 

//=======================================
int MMMSectorCalc(int BackChannel)
{
  int ring,sector;
  int detectori,detectorj;
	   
  if(BackChannel>79 && BackChannel<88) {  
	detectorj=0; 
	sector=BackChannel-80;
  }
  else if (BackChannel>87 && BackChannel<96){  
	detectorj=1;
	sector=BackChannel-88;
  }
  else if (BackChannel>95 && BackChannel<104) { 
	detectorj=2;
	sector=BackChannel-96;
  }
  else if  (BackChannel>103 && BackChannel<112) { 
	detectorj=3;
	sector=BackChannel-104;
  }

  //printf("detectori = %i : ring = %i,  sector = %i  theta = %f \n",detectori, ring, sector,GA_TIARA[detectori][ring][sector][0]);
  return sector; 
}



