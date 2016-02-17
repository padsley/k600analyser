/*
 * Sort code for the W1s in conjunction with the K600 spectrometer
 * This code 'plugs in' to f-plane.c and is called from there. It populates the SiliconData class in that code and chucks the data out into the ROOT tree 'DATA'.
 *
 * Date: August 2014
 *
 */

#include "SiliconData.h"
#include "W1.h"

extern int ADCModules;
extern int ADCsize;
extern int NumberOfW1;

extern int **W1ADCChannelLimits;
extern int **W1TDCChannelLimits;

extern double *ADCOffsets;
extern double *ADCGains;

TCutG *W1FrontBackEnergyCut;

void W1SiliconSort(float *ADC_import, int ntdc,
		   int *TDC_channel_import, float *TDC_value_import, SiliconData *si)
{
    multiTDC mTDC;
    mTDC.multiTDCSort(ntdc, TDC_channel_import, TDC_value_import);

    int tdcSize = mTDC.GetSize();

    for (int k = 0; k < tdcSize; k++)
    {
        int tdcFront = mTDC.GetChannel(k);
        if (!W1TDCFrontTest(tdcFront))
            continue;
        for (int l = 0; l < tdcSize; l++)
        {
            int tdcBack = mTDC.GetChannel(l);
            if (!(W1TDCBackTest(tdcBack) && W1TDCFrontBackTest(tdcFront, tdcBack)))
                continue;
            int DetNum = W1TDCIdentifyDetector(tdcFront, tdcBack);
            if (DetNum < 0)
                continue;
            for (int i = W1ADCChannelLimits[DetNum][0]; i <= W1ADCChannelLimits[DetNum][1]; i++)
            {
                if (!(W1ADCTDCChannelTestPSide(i, tdcFront) && ADC_import[i] > 0))
                    continue;
                for (int j = W1ADCChannelLimits[DetNum][2]; j <= W1ADCChannelLimits[DetNum][3]; j++)
                {
                    if (ADC_import[j] <= 0)
                        continue;
                    double energyi = W1EnergyCalc(i,ADC_import[i]);
                    double energyj = W1EnergyCalc(j,ADC_import[j]);
                    //Test whether the hits are in the front and back of the same detector and whether the energies are good
                    if(!(W1FrontBackTest(i,j,energyi,energyj,si) &&
                                W1ADCTDCChannelTestNSide(j,tdcBack)))
                        continue;
                    //printf("through front-back test\n");
                    //si->SetEnergy(0.5*(energyi+energyj));
                    si->SetEnergy(energyi);          //Just use front energy because the back energy resolution is bloody terrible
                    si->SetTheta(W1ThetaCalc(i,j));
                    si->SetPhi(W1PhiCalc(i,j));
                    si->SetTime(mTDC.GetValue(k));

                    si->SetEnergyFront(energyi);
                    si->SetEnergyBack(energyj);
                    si->SetTimeFront(mTDC.GetValue(k));
                    si->SetTimeBack(mTDC.GetValue(l));

                    si->SetDetectorHit(W1DetHitNumber(i,j));
                    si->SetStripFront(W1StripFront(i));
                    si->SetStripBack(W1StripBack(j));

                    si->SetADCChannelFront(i);
                    si->SetADCChannelBack(j);
                    si->SetADCValueFront(ADC_import[i]);
                    si->SetADCValueBack(ADC_import[j]);

                    si->SetTDCChannelFront(tdcFront);
                    si->SetTDCChannelBack(tdcBack);
                    si->SetTDCValueFront(mTDC.GetValue(k));
                    si->SetTDCValueBack(mTDC.GetValue(l));

                    si->SetMult(mTDC.GetMult(k));
                }
            }
            if (W1TDCChannelLimits[DetNum][2] == -1)
                break;
        }
    }

    si->SetHits(si->SizeOfEvent());
    if(!si->TestEvent())si->ClearEvent();
    //si->PrintEvent();
    //   return si;
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
    //printf("Channel: %i \t ADCValue: %f\n",Channel,ADCValue);
    //printf("Offset: %f \t Gain: %f\n",ADCOffsets[Channel],ADCGains[Channel]);
    double result = ADCOffsets[Channel] + ADCGains[Channel]*ADCValue;
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

    //   gROOT->ProcessLine(".x FrontBackEnergyCut.C");

    for(int i=0;i<NumberOfW1;i++)
    {
        if(FrontChannel>=W1ADCChannelLimits[i][0] && FrontChannel<=W1ADCChannelLimits[i][1] && BackChannel>=W1ADCChannelLimits[i][2] && BackChannel<=W1ADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
        {
            //if(FrontChannel==0)printf("\n\nFrontChannel: %d \t BackChannel: %d\n\n",FrontChannel, BackChannel);
            //printf("Test157\n");
            //printf("FrontEnergy: %f\n",FrontEnergy);
            //printf("BackEnergy: %f\n",BackEnergy);
            double diff = FrontEnergy - BackEnergy;
            //printf("diff: %f\n",diff);
            //printf("Test condition: %f\n",diff/(0.5*(FrontEnergy+BackEnergy)));
            //if(diff==0)result=true;
            if(diff<0)diff*=-1;
            if(result==false && diff/(0.5*(FrontEnergy+BackEnergy))<0.05)//Check to see if the front and back energies are approximately equal
            {
                //printf("Test162\n");
                result = true;//They are -> it's a good event
            }
        }
    }
    //     printf("FrontBackTest End\n");
    //if(result)printf("True\n");
    //else if(!result)printf("False\n");

    return result;
}


int W1DetHitNumber(int FrontChannel, int BackChannel)
{
    int result = 0;
    for(int i=0;i<NumberOfW1;i++)
    {
        if(FrontChannel>=W1ADCChannelLimits[i][0] && FrontChannel<=W1ADCChannelLimits[i][1] && BackChannel>=W1ADCChannelLimits[i][2] && BackChannel<=W1ADCChannelLimits[i][3])//If the two hits are part of the front and back of the same detector, then consider that the event might be good - make sure that hit 'i' in the main loop in the sort is always the front. Then we don't get any double-counting
        {
            result = i+1;
        }
    }
    return result;
}

int W1StripFront(int FrontChannel)//The strip number runs 1->16 on each detector. i.e. there will be 4 strip 1s if there are 4 detectors. Use the detector hit quantity from above if you need to localise to one detector
{
    //This one is just reversed.
    int result = 0;
    result = FrontChannel%16;//Gives the number relative to the start of each detector. 0,16,32... return 0, 1, 17, 33 return 1 etc.
    result = 16-result;//Want to start the channel numbering at 1 for consistency with the Micron documentation
    return result;
}

int W1StripBack(int BackChannel)//Again, for the W1, this runs from 1->16.
{
    int result = 0;
    int chan = BackChannel%16;
    if(chan>=8)result = chan - 7; //(actually -8+1 because we want to shift 8->15 to 0->7 and then 1->8)
    else if(chan<=7)result = chan + 8 + 1;
    return result;
}

bool W1ADCTDCChannelTestPSide(int ADCChannel, int TDCChannel)
{
    bool result = false;
    for(int i=0;i<NumberOfW1;i++)
    {
        if(ADCChannel>=W1ADCChannelLimits[i][0] && ADCChannel<=W1ADCChannelLimits[i][1] && TDCChannel>=W1TDCChannelLimits[i][0] && TDCChannel<=W1TDCChannelLimits[i][1])//Check to see if the ADC/TDC events are in the same detector
        {
            if(ADCChannel%16==TDCChannel%16)
            {
                result = true;
            }
            //if(TDCChannel<832)printf("Pass for invalid TDC value! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
        }
        // Commenting out for now because it can cause problems if some detectors are missing - NH
        //if(W1ADCChannelLimits[i][0]==-1)result = true; //No information for some of the W1s -> Suppress this test (i.e. if there is no ADC channel set)
        //if(W1ADCChannelLimits[i][1]==-1)result = true;
        //if(W1TDCChannelLimits[i][0]==-1)result = true;
        //if(W1TDCChannelLimits[i][1]==-1)result = true;
    }

    //printf("W1ADCTDCChannelTest stop: %d %d",ADCChannel,TDCChannel);
    return result;
}

bool W1ADCTDCChannelTestNSide(int ADCChannel, int TDCChannel)
{
    bool result = false;
    for(int i=0;i<NumberOfW1;i++)
    {
        if(ADCChannel>=W1ADCChannelLimits[i][2] && ADCChannel<=W1ADCChannelLimits[i][3] && TDCChannel>=W1TDCChannelLimits[i][2] && TDCChannel<=W1TDCChannelLimits[i][3])//Check to see if the ADC/TDC events are in the same detector
        {
            if(ADCChannel%8==TDCChannel%8)
            {
                result = true;
            }
            //if(TDCChannel<832)printf("Pass for invalid TDC value! ADCChannel: %d \t TDC Channel: %d\n",ADCChannel, TDCChannel);
        }
        if(W1ADCChannelLimits[i][2]==-1)result = true; //No information for some of the W1s -> Suppress this test (i.e. if there is no ADC channel set)
        if(W1ADCChannelLimits[i][3]==-1)result = true;
        if(W1TDCChannelLimits[i][2]==-1)result = true;
        if(W1TDCChannelLimits[i][3]==-1)result = true;
    }

    //printf("W1ADCTDCChannelTestNSide\n");

    return result;
}

bool W1TDCFrontTest(int TDCChannel)
{
    bool result = false;
    for (int i = 0; i < NumberOfW1; i++)
    {
        if (TDCChannel >= W1TDCChannelLimits[i][0] && TDCChannel <= W1TDCChannelLimits[i][1])
        {
            result = true;
        }
        // Commenting out for now because it can cause problems if some detectors are missing - NH
        //if (W1TDCChannelLimits[i][0] == -1) result = true;
        //if (W1TDCChannelLimits[i][1] == -1) result = true;
    }
    return result;
}

bool W1TDCBackTest(int TDCChannel)
{
    bool result = false;
    for (int i = 0; i < NumberOfW1; i++)
    {
        if (TDCChannel >= W1TDCChannelLimits[i][2] && TDCChannel <= W1TDCChannelLimits[i][3])
        {
            result = true;
        }
        if (W1TDCChannelLimits[i][2] == -1) result = true;
        if (W1TDCChannelLimits[i][3] == -1) result = true;
    }
    return result;
}

bool W1TDCFrontBackTest(int TDCFrontChannel, int TDCBackChannel)
{
    bool result = false;
    for (int i = 0; i < NumberOfW1; i++)
    {
        if (TDCFrontChannel >= W1TDCChannelLimits[i][0] && TDCFrontChannel <= W1TDCChannelLimits[i][1]
                && TDCBackChannel >= W1TDCChannelLimits[i][2] && TDCBackChannel <= W1TDCChannelLimits[i][3])
        {
            result = true;
        }
        // Commenting out for now because it can cause problems if some detectors are missing - NH
        //if (W1TDCChannelLimits[i][0] == -1) result = true;
        //if (W1TDCChannelLimits[i][1] == -1) result = true;

        if (W1TDCChannelLimits[i][2] == -1) result = true;
        if (W1TDCChannelLimits[i][3] == -1) result = true;
    }
    return result;
}

int W1TDCIdentifyDetector(int TDCFrontChannel, int TDCBackChannel)
{
    int result = -1;
    for (int i = 0; i < NumberOfW1; i++)
    {
        if (TDCFrontChannel >= W1TDCChannelLimits[i][0] && TDCFrontChannel <= W1TDCChannelLimits[i][1])
        {
            if (W1TDCChannelLimits[i][2] == -1 || (TDCBackChannel >= W1TDCChannelLimits[i][2] && TDCFrontChannel <= W1TDCChannelLimits[i][3]))
            {
                result = i;
            }
        }
    }
    return result;
}

