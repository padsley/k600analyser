/*
 * Sort code for the W1s in conjunction with the K600 spectrometer
 * This code 'plugs in' to f-plane.c and is called from there. It populates the SiliconData class in that code and chucks the data out into the ROOT tree 'DATA'.
 * 
 * Date: August 2014
 * 
 */

#include "SiliconData.h"
#include "W1.h"

const int ADCsize = 128;
const int NumberOfDetectors = 4;

int **W1ADCChannelLimits;
int **W1TDCChannelLimits;

TCutG *W1FrontBackEnergyCut;

SiliconData *W1SiliconSort(float *ADC_import, int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
    SiliconData *si = new SiliconData();
  
    multiTDC *mTDC = new multiTDC(ntdc, TDC_channel_import, TDC_value_import);	
    
    for(int k=0;k<mTDC->GetSize();k++)
    {
      
    }
  return si;
}

void W1LoadCuts(SiliconData *si)
{
  printf("Load W1 Front-Back Energy cut\n");
  TCutG *cutg = new TCutG("FrontBackEnergyCut",8);
  cutg->SetVarX("EnergyBack");
  cutg->SetVarY("EnergyFront");
  cutg->SetTitle("Graph");
  cutg->SetFillColor(1);
  cutg->SetPoint(0,8625.54,9448.89);
  cutg->SetPoint(1,9436.42,9393.27);
  cutg->SetPoint(2,9398.71,8336.6);
  cutg->SetPoint(3,8512.39,8253.18);
  cutg->SetPoint(4,610.991,300.318);
  cutg->SetPoint(5,516.703,912.076);
  cutg->SetPoint(6,8625.54,9143.01);
  cutg->SetPoint(7,8625.54,9448.89);
  
  W1FrontBackEnergyCut = cutg;
}

void W1Init()//Initialise function which gets the information on the DAQ channels->Physical channels
{ 
  //The user needs to change the channel limits given below depending on what their DAQ setup is.
  W1ADCChannelLimits = new int*[NumberOfDetectors];
  for(int i=0;i<NumberOfDetectors;i++)
  {
    W1ADCChannelLimits[i] = new int[4];
  }
  //   
  //   //I set the channel address limits manually. I could do it with an automatic script but it's actually simpler this way. Get back to work!
  //   
  W1ADCChannelLimits[0][0] = 0;//First channel of the front of the first detector
  W1ADCChannelLimits[0][1] = 15;//Last channel of the front of the first detector
  W1ADCChannelLimits[0][2] = 80;//First channel of the back of the first detector
  W1ADCChannelLimits[0][3] = 87;//Last channel of the back of the first detector
  //   
  W1ADCChannelLimits[1][0] = 16;
  W1ADCChannelLimits[1][1] = 31;
  W1ADCChannelLimits[1][2] = 88;
  W1ADCChannelLimits[1][3] = 95;
  //   
  W1ADCChannelLimits[2][0] = 32;
  W1ADCChannelLimits[2][1] = 47;
  W1ADCChannelLimits[2][2] = 96;
  W1ADCChannelLimits[2][3] = 103;
  //   
  W1ADCChannelLimits[3][0] = 48;
  W1ADCChannelLimits[3][1] = 63;
  W1ADCChannelLimits[3][2] = 104;
  W1ADCChannelLimits[3][3] = 111;
  //   
  //   //Currently, the TDCs only exist for the fronts so we shall limit ourself to having an array size of 2 for these. If TDC fronts and backs are instrumented, this will need to change to something that looks like what is above
  //   
  W1TDCChannelLimits = new int*[NumberOfDetectors];
  for(int i=0;i<NumberOfDetectors;i++)
  {
    W1TDCChannelLimits[i] = new int[2];
  }
  
  W1TDCChannelLimits[0][0] = 128*6+48;
  W1TDCChannelLimits[0][1] = 128*6+48+15;
  
  W1TDCChannelLimits[1][0] = 128*6+48+16;
  W1TDCChannelLimits[1][1] = 128*6+48+31;
  
  W1TDCChannelLimits[2][0] = 128*6+48+32;
  W1TDCChannelLimits[2][1] = 128*6+48+47;
  
  W1TDCChannelLimits[3][0] = 128*6+48+48;
  W1TDCChannelLimits[3][1] = 128*6+48+63;
}

bool W1SuppressChannel(int Channel)//If the ADC channel is one which we wish to suppress, we do that here. Use if(Channel = 12)return true to suppress channel 12. Load of else ifs for the other suppressed channels. Then else return false.
{
  if(Channel>128)
    return true;
  else
    return false;
}

double W1EnergyCalc(int Channel, double ADCValue)
{
  double result = silicon_offset[Channel] + silicon_gain[Channel]*ADCValue;
  //   if(Channel<64 || Channel>=80 && Channel<112)printf("EnergyCalc: %g\n",result);
  return result;
}

double W1ThetaCalc(int Channel)
{
  double result = 180;
  return result;
}

double W1PhiCalc(int Channel)
{
  double result = 0;
  return result;
}

bool W1FrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si)
{
  bool result = false;
  //     printf("FrontBackTest Start\n");
  //   gROOT->ProcessLine(".x FrontBackEnergyCut.C");
  
  for(int i=0;i<NumberOfDetectors;i++)
  {
    if(FrontChannel>=W1ADCChannelLimits[i][0] && FrontChannel<=W1ADCChannelLimits[i][1] && BackChannel>=W1ADCChannelLimits[i][2] && BackChannel<=W1ADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	if(W1FrontBackEnergyCut->IsInside(BackEnergy,FrontEnergy))//Check to see if the front and back energies are approximately equal
	  {
	    // 	    printf("Tust\n");
	    result = true;//They are -> it's a good event
	  }
      }
  }
  //   printf("FrontBackTest End");
  return result;
}


int W1DetHitNumber(int FrontChannel, int BackChannel)
{
  int result = 0;
  for(int i=0;i<NumberOfDetectors;i++)
  {
    if(FrontChannel>=W1ADCChannelLimits[i][0] && FrontChannel<=W1ADCChannelLimits[i][1] && BackChannel>=W1ADCChannelLimits[i][2] && BackChannel<=W1ADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
      {
	result = i+1;
      }
  }
  return result;
}

bool W1ADCTDCChannelTest(int ADCChannel, int TDCChannel)
{
  bool result = false;
//   printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfDetectors;i++)
  {
    if(ADCChannel>=W1ADCChannelLimits[i][0] && ADCChannel<=W1ADCChannelLimits[i][1] && TDCChannel>=W1TDCChannelLimits[i][0] && TDCChannel<=W1TDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
   {
     if(TDCChannel<816)printf("Pass for invalid TDC value! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
     if(ADCChannel%16==TDCChannel%16)
     {
//        printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
       result = true;
     }
   }
  }
  return result;
}

