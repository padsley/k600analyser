#include "GammaData.h"

GammaData::GammaData()
{
  //Constructor
}

GammaData::~GammaData()
{
//   printf("Destructor called for GammaData");
  ClearEvent();
}

void GammaData::PrintEvent()//Print out the 'proper' information about an event
{
  //Want to print out the number of valid Gamma hits
  printf("Gamma hits: %d\n",SizeOfEvent());

  for(unsigned int i=0;i<SizeOfEvent();i++)
    {
      printf("Hit number: %d\n",i);
      printf("Energy: %g\n",GetEnergy(i));
    }
}

bool GammaData::TestEvent()
{
  bool result = true;

 //  Test the output data: do we have the same number of hits in each thing (energy, time etc.) and are the events good and shizzle like that 
 unsigned int hits = GammaEnergy.size();

  //Check to see the same number of hits c.f. the energy hits. If you get a lot of warning messages from theta, phi, time here, that's likely because you're doing the energy hits wrong/differently
//  if(GammaTheta.size() != hits){printf("Mismatched size: Theta\n"); result = false;}
 // if(GammaPhi.size() != hits){printf("Mismatched size: Phi\n"); result = false;}
  if(GammaTime.size() != hits){printf("Mismatched size: Time\n"); result = false;}

/*
  for(unsigned int i=0; i<hits; i++)
  {
  //Check to see that the computed angle is reasonable and that nothing weird is going on - you need to add you own checks here as detector positions will change
    if(GammaTheta.at(i)<90){printf("GammaTheta suggests a Gamma detector at forward angles - problem in angle computation\n"); result = false;}
  }

*/
// diagnostic variable
 /// if(DetectorHit.size() != hits){printf("Mismatched size: DetectorHit"); result = false;}
/*  if(ADCChannelFront.size() != hits){printf("Mismatched size: ADCChannelFront\n"); result = false;}
  if(ADCChannelBack.size() != hits){printf("Mismatched size: ADCChannelBack\n"); result = false;}
  if(TDCChannelFront.size() != hits){printf("Mismatched size: TDCChannelFront\n"); result = false;}
  if(TDCChannelBack.size() != hits){printf("Mismatched size: TDCChannelBack\n"); result = false;}
  if(ADCValueFront.size() != hits){printf("Mismatched size: ADCValueFront\n"); result = false;}
  if(ADCValueBack.size() != hits){printf("Mismatched size: ADCValueBack\n"); result = false;}
  if(TDCValueFront.size() != hits){printf("Mismatched size: TDCValueFront\n"); result = false;}
  if(TDCValueBack.size() != hits){printf("Mismatched size: TDCValueBack\n"); result = false;}
  if(StripFront.size() != hits){printf("Mismatched size: StripFront\n"); result = false;}
   if(StripBack.size() != hits){printf("Mismatched size: StripBack\n"); result = false;}
  if(EnergyFront.size() != hits){printf("Mismatched size: EnergyFront\n"); result = false;}
  if(EnergyBack.size() != hits){printf("Mismatched size: EnergyBack\n"); result = false;}
  
*/

  return result;
}



void GammaData::ClearEvent()
{
  
//   printf("GammaData: ClearEvent()\n");
  GammaEnergy.clear();
//  GammaTheta.clear();
//  GammaPhi.clear();
  GammaTime.clear();
  GammaDetectorType.clear();
  GammaDetectorLabel.clear();
  GammaDetectorSegm.clear();
  GammaRawADC.clear();
  GammaADCChannel.clear();
 //diagnostic variable
 // DetectorHit.clear();
//  ADCChannelClover.clear();
//  TDCChannelClover.clear();
//  ADCValueClover.clear();
 // TDCValueClover.clear();
  //StripFront.clear();
  //StripBack.clear();
  //EnergyFront.clear();
  //EnergyBack.clear();


}

unsigned int GammaData::SizeOfEvent()
{
  unsigned int result = GammaEnergy.size();
  return result;
}
