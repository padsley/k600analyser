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
  W1Init();
  //printf("22\n");
  SiliconData *si = new SiliconData();
  //printf("23\n");
  multiTDC *mTDC = new multiTDC(ntdc, TDC_channel_import, TDC_value_import);	
  //printf("25\n");
  for(int k=0;k<mTDC->GetSize();k++)
    {
      for(int i=0;i<64;i++)
	{
	  //Don't want to run for events which are below pedestal. Set this to be 250 generally for the moment. In future, might want to increase it a bit
	  //printf("31\n");
	  if(W1ADCTDCChannelTest(i,mTDC->GetChannel(k)) && ADC_import[i]>300)
	    {
	      //if(i==0)
	      for(int j=64;j<ADCsize;j++)
		{
		  //printf("37\n");
		  if(ADC_import[j]>300)
		    {
		      //printf("40\n");
		      double energyi = W1EnergyCalc(i,ADC_import[i]);
		      double energyj = W1EnergyCalc(j,ADC_import[j]);
		      //printf("45\n");
		      //Test whether the hits are in the front and back of the same detector and whether the energies are good
		      if(W1FrontBackTest(i,j,energyi,energyj,si))
			{
			  //si->SetEnergy(0.5*(energyi+energyj));
			  si->SetEnergy(energyi);//Just use front energy because the back energy resolution is bloody terrible
			  si->SetTheta(W1ThetaCalc(i,j));
			  si->SetPhi(W1PhiCalc(i,j));
			  si->SetTime(mTDC->GetValue(k));
		
			  si->SetDetectorHit(W1DetHitNumber(i,j));
			  si->SetADCChannelFront(i);
			  si->SetADCChannelBack(j);
			  si->SetTDCChannelFront(mTDC->GetChannel(k));

			  si->SetTDCChannelBack(-1);
			  si->SetADCValueFront(ADC_import[i]);
			  si->SetADCValueBack(ADC_import[j]);
		
			  si->SetTDCValueFront(mTDC->GetValue(k));

			  si->SetTDCValueBack(-1);
			  si->SetEnergyFront(energyi);
			  si->SetEnergyBack(energyj);

			  si->SetMult(mTDC->GetMult(k));
			}
		    }
		}
	    }
	}
    }
  //printf("75\n");
  si->SetHits(si->SizeOfEvent());
  if(!si->TestEvent())si->ClearEvent();
  //si->PrintEvent();
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
  W1ADCChannelLimits[0][2] = 64;//First channel of the back of the first detector
  W1ADCChannelLimits[0][3] = 79;//Last channel of the back of the first detector
  //   
  W1ADCChannelLimits[1][0] = 16;
  W1ADCChannelLimits[1][1] = 31;
  W1ADCChannelLimits[1][2] = 80;
  W1ADCChannelLimits[1][3] = 95;
  //   
  W1ADCChannelLimits[2][0] = 32;
  W1ADCChannelLimits[2][1] = 47;
  W1ADCChannelLimits[2][2] = 96;
  W1ADCChannelLimits[2][3] = 111;
  //   
  W1ADCChannelLimits[3][0] = 48;
  W1ADCChannelLimits[3][1] = 63;
  W1ADCChannelLimits[3][2] = 112;
  W1ADCChannelLimits[3][3] = 127;
  //   
  //   //Currently, the TDCs only exist for the fronts so we shall limit ourself to having an array size of 2 for these. If TDC fronts and backs are instrumented, this will need to change to something that looks like what is above for the ADCChannelLimits
  //   
  W1TDCChannelLimits = new int*[NumberOfDetectors];
  for(int i=0;i<NumberOfDetectors;i++)
    {
      W1TDCChannelLimits[i] = new int[2];
    }
  
  W1TDCChannelLimits[1][0] = 128*6+64;
  W1TDCChannelLimits[1][1] = 128*6+64+15;
  
  W1TDCChannelLimits[0][0] = 128*6+64+16;
  W1TDCChannelLimits[0][1] = 128*6+64+31;
  
  W1TDCChannelLimits[3][0] = 128*6+64+32;
  W1TDCChannelLimits[3][1] = 128*6+64+47;
  
  W1TDCChannelLimits[2][0] = 128*6+64+48;
  W1TDCChannelLimits[2][1] = 128*6+64+63;
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
  //define the silicon calibration parameters
  extern double silicon_offset[128];
  extern double silicon_gain[128];
  double result = silicon_offset[Channel] + silicon_gain[Channel]*ADCValue;
  //   if(Channel<64 || Channel>=80 && Channel<112)printf("EnergyCalc: %g\n",result);
  return result;
}

double W1ThetaCalc(int FrontChannel, int BackChannel)
{
  double result = 180;
  return result;
}

double W1PhiCalc(int FrontChannel, int BackChannel)
{
  double result = 0;
  return result;
}

bool W1FrontBackTest(int FrontChannel, int BackChannel, double FrontEnergy, double BackEnergy, SiliconData *si)
{
  bool result = false;
  //printf("FrontBackTest Start\n");
  //   gROOT->ProcessLine(".x FrontBackEnergyCut.C");
  
  for(int i=0;i<NumberOfDetectors;i++)
    {
      if(FrontChannel>=W1ADCChannelLimits[i][0] && FrontChannel<=W1ADCChannelLimits[i][1] && BackChannel>=W1ADCChannelLimits[i][2] && BackChannel<=W1ADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
	{
	  //if(FrontChannel==0)printf("\n\nFrontChannel: %d \t BackChannel: %d\n\n",FrontChannel, BackChannel);
	  double diff = FrontEnergy - BackEnergy;
	  if(diff<0)diff*=-1;
	  if(diff/(0.5*(FrontEnergy+BackEnergy))<0.05)//Check to see if the front and back energies are approximately equal
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
  //printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
  for(int i=0;i<NumberOfDetectors;i++)
    {
      //printf("Test\n");
      //printf("W1ADCChannelLimits[%d][0]: %d\n",i,W1ADCChannelLimits[i][0]);
      //printf("W1ADCChannelLimits[%d][1]: %d\n",i,W1ADCChannelLimits[i][1]);
      //printf("W1TDCChannelLimits[%d][0]: %d\n",i,W1TDCChannelLimits[i][0]);
      //printf("W1TDCChannelLimits[%d][1]: %d\n",i,W1TDCChannelLimits[i][1]);
      //printf("EndTest\n");
      if(ADCChannel>=W1ADCChannelLimits[i][0] && ADCChannel<=W1ADCChannelLimits[i][1] && TDCChannel>=W1TDCChannelLimits[i][0] && TDCChannel<=W1TDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
	{
	  //printf("ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
	  //printf("Test2\n");
	  if(TDCChannel<816)printf("Pass for invalid TDC value! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
	  if(ADCChannel%16==TDCChannel%16)
	    {
	      //        printf("Correlation! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
	      result = true;
	    }
	}
    }
  //printf("W1ADCTDCChannelTest return\n");
  //printf("space\n");
  return result;
}

