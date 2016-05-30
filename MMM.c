/* Sort code for MMM-type silicon detectors
 * 
 * Author: Philip Adsley
 * 
 * This code also functions as a template for silicon sort codes for K600 experiments.
 * 
 * The header file MMM.h defines the ADC and TDC channel mapping for the particular experiment in question. The user should modify these accordingly. How these channel mappings are defined as given in ???
 * 
 */

#include "SiliconData.h"
#include "MMM.h"
#include "MMManglecalibration.h"     // a file from Kevin, based on his GEANT simlulations. Can be exp specific.
#include "ADCPedestal.h"

extern int ADCModules;
extern int ADCsize;
extern int NumberOfMMM;

extern int **MMMADCChannelLimits;
extern int **MMMTDCChannelLimits;

extern double *ADCOffsets;
extern double *ADCGains;

extern double *TDCOffsets;

TCutG *MMMFrontBackEnergyCut;

const double sigma = 100;//keV - silicon energy resolution - used for the front-back energy cut condition

/*double TDCOffsets[64] = {2180.06,
2177.38,
2138.44,
2116.34,
2143.47,
2134.65,
2154.8,
2145.37,
2143.91,
2106.59,
2107.9,
2115.07,
2153.49,
2096.72,
2111.04,
2089.38,
2304.18,
2295.5,
2313.7,
2258.54,
2249.64,
2237.04,
2247.58,
2275.4,
2251.4,
2261.99,
2244.29,
2201.36,
2235.62,
2256.41,
2228.89,
2179.46,
1804,
2202.34,
2243.57,
2238.94,
2224.37,
2179.97,
2212.43,
2211.06,
2201.18,
2209.8,
2202.67,
2183.57,
2198.41,
2146.87,
2163.14,
2208.32,
2238.36,
2164.76,
2197.97,
2176.16,
2174.87,
2147.11,
2144.08,
2142.6,
2162.39,
2116.09,
2129.79,
2107.31,
2162.93,
2157.17,
2097.37,
2106.41};*/

//---------------------------------------------------------------------
void MMMSiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import, SiliconData *si)
{
  //SiliconData *si = new SiliconData();
  //Loop over ADC and TDC events and do the following:
  //Check whether there are front-back coincidences for a detector and test the energies
  //Check to see whether there's a TDC event for the same channel as the front hit ADC - TDCs are apparently in single-hit mode. Don't need to worry about multihits
  //Calculate the theta and phi for the valid events
  multiTDC mTDC;// = new multiTDC();
  mTDC.multiTDCSort(ntdc, TDC_channel_import, TDC_value_import);

  for(int k=0;k<mTDC.GetSize();k++)//Loop over all of the TDC values - there should only be a small number of these relative to the ADC values
  {
    if(MMMTDCFrontTest(mTDC.GetChannel(k)))
      {
	for(int l=0;l<mTDC.GetSize();l++)//Loop over all of the TDC values *again* but in this case, looking for the N sides
	  {
	    if(MMMTDCBackTest(mTDC.GetChannel(l)) && MMMTDCFrontBackTest(mTDC.GetChannel(k),mTDC.GetChannel(l)))
	      { 
		//printf("MMMSiliconSort L116 test\n");
		int DetNum = MMMTDCIdentifyDetector(mTDC.GetChannel(k),mTDC.GetChannel(l));
		if(DetNum>0)
		  {	
		    for(int i=MMMADCChannelLimits[DetNum-1][0];i<=MMMADCChannelLimits[DetNum-1][1];i++)
		      {
		        //printf("MMMSiliconSort L122 test\n");
			//Don't want to run for events w
			if(MMMADCTDCChannelTestPSide(i,mTDC.GetChannel(k)) && ADC_import[i]>0)
			  {
			    //printf("MMMSiliconSort L126 test\n");
			    for(int j=MMMADCChannelLimits[DetNum-1][2];j<=MMMADCChannelLimits[DetNum-1][3];j++)
			      {
			        //printf("MMMSiliconSort L129 test\n");
				if(ADC_import[j]>0)
				  {//printf("test\n");
				    double energyi = MMMEnergyCalc(i,ADC_import[i]);
				    double energyj = MMMEnergyCalc(j,ADC_import[j]);
				    
				    //Test whether the hits are in the front and back of the same detector and whether the energies are good
				    if(MMMFrontBackTest(i,j,energyi,energyj,si) && MMMADCTDCChannelTestNSide(j,mTDC.GetChannel(l)) && 0.5*(energyi+energyj)>400)
				      {				
	                                //printf("MMMSiliconSort L134 test\n");
					si->SetEnergy(energyi);

					si->SetTheta(MMMThetaCalc(i,j)); // RN 7 March 16: I propose a different way
					si->SetPhi(MMMPhiCalc(i,j));     // RN 7 March 16: I proposa a different way
					
					si->SetTime(mTDC.GetValue(k));
					si->SetTimeFront(mTDC.GetValue(k));
					si->SetTimeBack(mTDC.GetValue(l));

					si->SetOffsetTime(mTDC.GetValue(k) - TDCOffsets[mTDC.GetChannel(k)]);
					
					si->SetDetectorHit(MMMDetHitNumber(i,j));
					si->SetADCChannelFront(i);
					si->SetADCChannelBack(j);
					si->SetStripFront(MMMStripFront(i));
					si->SetStripBack(MMMStripBack(j));
					  
					si->SetTDCChannelFront(mTDC.GetChannel(k));

					si->SetTDCChannelBack(mTDC.GetChannel(l));

					si->SetADCValueFront(ADC_import[i]);
					si->SetADCValueBack(ADC_import[j]);
					  
					si->SetTDCValueFront(mTDC.GetValue(k));
					si->SetTDCValueBack(-1);
					si->SetEnergyFront(energyi);
					si->SetEnergyBack(energyj);
					  
					si->SetMult(mTDC.GetMult(k));
				      }
				  }
			      }
			  }
		      }
		  }
	      }
	  }
      }
  }
  
  si->SetHits(si->SizeOfEvent());
  //printf("SiliconData PrintEvent routine: Silicon hits: %d\n",si->SizeOfEvent());
  //si->PrintEvent();
  //printf("MMM.c L180\n");
  if(!si->TestEvent()) si->ClearEvent(); //If the event fails for some reason, we void it and clear it here. The number of these should be logged and, ideally, should be zero. A VOIDED EVENT IS ONE IN WHICH ALL SILICON DATA ARE THROWN AWAY BECAUSE THE RESULT IS **WRONG**. There are more energy hits than theta hits, for example. IT THEY ARE HAPPENING, THEN YOU'VE DONE IT WRONG.
  //printf("MMM.c L182\n");
  //MMMGhostBuster(SiliconData *si);

  mTDC.ClearEvent();
  //return si;

}

//---------------------------------------------------------------------
void MMMLoadCuts(SiliconData *si)
{
  printf("Load MMM Front-Back Energy cut\n");
  TCutG *cutg = new TCutG("FrontBackEnergyCut",8);
  cutg->SetVarX("EnergyBack");
  cutg->SetVarY("EnergyFront");
  cutg->SetTitle("Graph");
  cutg->SetFillColor(1);
/*
  cutg->SetPoint(0,8625.54,9448.89);
  cutg->SetPoint(1,9436.42,9393.27);
  cutg->SetPoint(2,9398.71,8336.6);
  cutg->SetPoint(3,8512.39,8253.18);
  cutg->SetPoint(4,610.991,300.318);
  cutg->SetPoint(5,516.703,912.076);
  cutg->SetPoint(6,8625.54,9143.01);
  cutg->SetPoint(7,8625.54,9448.89);
*/
  cutg->SetPoint(0,500.,0.);   
  cutg->SetPoint(1,0.,500.);   
  cutg->SetPoint(2,14500.,15000.);   
  cutg->SetPoint(3,15000.,14500.);   
  cutg->SetPoint(4,500.,0.);   

  
  MMMFrontBackEnergyCut = cutg;
}

//---------------------------------------------------------------------
void MMMInit()//Initialise function which gets the information on the DAQ channels->Physical channels
{ 
     initialize_Angles();
     initialize_pedestal();
//This bit is currently doing nowt but may be called at the beginning of each 'event' to set up useful things for other bits of the code.
}

//---------------------------------------------------------------------
bool MMMSuppressChannel(int Channel)//If the ADC channel is one which we wish to suppress, we do that here. Use if(Channel = 12)return true to suppress channel 12. Load of else ifs for the other suppressed channels. Then else return false.
{
  if(Channel>128)
    return true;
  else
    return false;
}

//---------------------------------------------------------------------
double MMMEnergyCalc(int Channel, double ADCValue)
{
  //define the silicon calibration parameters
  double result = ADCOffsets[Channel] + ADCGains[Channel]*ADCValue;
//   printf("Energy = ADCOffsets[%d] + ADCGains[%d] * ADCValue\t %f = %f + %f * %f\n",Channel,Channel,result,ADCOffsets[Channel],ADCGains[Channel],ADCValue);
  //   if(Channel<64 || Channel>=80 && Channel<112)printf("EnergyCalc: %g\n",result);
  return result;
}



//---------------------------------------------------------------------
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



//---------------------------------------------------------------------
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



//---------------------------------------------------------------------
bool MMMFrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si)
{
  bool result = false;
  //     printf("FrontBackTest Start\n");
  //   gROOT->ProcessLine(".x FrontBackEnergyCut.C");
  
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(FrontChannel>=MMMADCChannelLimits[i][0] && FrontChannel<=MMMADCChannelLimits[i][1] && BackChannel>=MMMADCChannelLimits[i][2] && BackChannel<=MMMADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	  double diff = FrontEnergy - BackEnergy;
	  //if(diff<0)diff*=-1;
	  //if(diff/(0.5*(FrontEnergy+BackEnergy))<0.05)//Check to see if the front and back energies are approximately equal <5%
	  if(std::abs(diff)<3*sigma)
	  {
	    result = true;//They are -> it's a good event
	  }
      }
  }
  //   printf("FrontBackTest End");
  return result;
}


//---------------------------------------------------------------------
int MMMDetHitNumber(int FrontChannel, int BackChannel)
{
  int result = 0;
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(FrontChannel>=MMMADCChannelLimits[i][0] && FrontChannel<=MMMADCChannelLimits[i][1] && BackChannel>=MMMADCChannelLimits[i][2] && BackChannel<=MMMADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	result = i+1;
      }
  }
  return result;
}

//---------------------------------------------------------------------
bool MMMADCTDCChannelTestPSide(int ADCChannel, int TDCChannel)
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(ADCChannel>=MMMADCChannelLimits[i][0] && ADCChannel<=MMMADCChannelLimits[i][1] && TDCChannel>=MMMTDCChannelLimits[i][0] && TDCChannel<=MMMTDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
   {
     if(ADCChannel%16==TDCChannel%16)
     {
       //printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
     if(MMMADCChannelLimits[i][0]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(MMMADCChannelLimits[i][1]==-1)result = true;
     if(MMMTDCChannelLimits[i][0]==-1)result = true;
     if(MMMTDCChannelLimits[i][1]==-1)result = true;
     }
  return result;
}

//---------------------------------------------------------------------
bool MMMADCTDCChannelTestNSide(int ADCChannel, int TDCChannel)
{ 
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfMMM;i++)
  {
    if(ADCChannel>=MMMADCChannelLimits[i][2] && ADCChannel<=MMMADCChannelLimits[i][3] && TDCChannel>=MMMTDCChannelLimits[i][2] && TDCChannel<=MMMTDCChannelLimits[i][3])//Check to see if the ADC/TDC events are in the same detector
   {
     if(ADCChannel%8==TDCChannel%8)
     {
//        printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
     if(MMMADCChannelLimits[i][2]==-1)result = true;//If there is no information for some of the MMMs, suppress this test
     if(MMMADCChannelLimits[i][3]==-1)result = true;
     if(MMMTDCChannelLimits[i][2]==-1)result = true;
     if(MMMTDCChannelLimits[i][3]==-1)result = true;
     }
  return result;
}

//---------------------------------------------------------------------
int MMMStripFront(int FrontChannel)
{
  int result = 0;
  result = FrontChannel%16;
  return result;
}

//---------------------------------------------------------------------
int MMMStripBack(int BackChannel)
{
  int result = 0;
  result = BackChannel%8;
  return result;
}

//---------------------------------------------------------------------
bool MMMTDCFrontTest(int TDCChannel)
{
  bool result = false;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCChannel>=MMMTDCChannelLimits[i][0] && TDCChannel<=MMMTDCChannelLimits[i][1])
	{
	  result = true;
	}
      if(MMMTDCChannelLimits[i][0]==-1)result = true;
      if(MMMTDCChannelLimits[i][1]==-1)result = true;
    }
 
  return result;
}

//---------------------------------------------------------------------
bool MMMTDCBackTest(int TDCChannel)
{
  bool result = false;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCChannel>=MMMTDCChannelLimits[i][2] && TDCChannel<=MMMTDCChannelLimits[i][3])
	{
	  result = true;
	}
      if(MMMTDCChannelLimits[i][2]==-1)result = true;
      if(MMMTDCChannelLimits[i][3]==-1)result = true;
    }
  return result;
}

//---------------------------------------------------------------------
bool MMMTDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel)
{
  bool result = false;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCFrontChannel>=MMMTDCChannelLimits[i][0] && TDCFrontChannel<=MMMTDCChannelLimits[i][1] && TDCBackChannel>=MMMTDCChannelLimits[i][2] && TDCBackChannel<=MMMTDCChannelLimits[i][3])
	{
	  result = true;
	  //printf("TDC F-B true\n");
	}
      //else printf("TDC F-B false\n");
    }
  return result;
}
  
//---------------------------------------------------------------------
int MMMTDCIdentifyDetector(int TDCFrontChannel, int TDCBackChannel)
{
  int result = -1;
  for(int i=0;i<NumberOfMMM;i++)
    {
      if(TDCFrontChannel>=MMMTDCChannelLimits[i][0] && TDCFrontChannel<=MMMTDCChannelLimits[i][1] && TDCBackChannel>=MMMTDCChannelLimits[i][2] && TDCBackChannel<=MMMTDCChannelLimits[i][3])
	{
	  result = i+1;
	}
    }
  //printf("result = %d\n",result);
  return result;
}




