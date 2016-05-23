#include "GammaData.h"

GammaData::GammaData()
{
  //Constructor
}

GammaData::~GammaData()
{
  //Destructor
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
 if(GammaEnergy.size() != GammaHits){printf("Mismatch between GammaEnergy and GammaHits\n");result=false;}

  //Check to see the same number of hits c.f. the energy hits. If you get a lot of warning messages from theta, phi, time here, that's likely because you're doing the energy hits wrong/differently
//  if(GammaTheta.size() != hits){printf("Mismatched size: Theta\n"); result = false;}
 // if(GammaPhi.size() != hits){printf("Mismatched size: Phi\n"); result = false;}
  if(GammaTime.size() != hits){printf("Mismatched size: Time\n"); result = false;}
  if(GammaDetectorType.size() != hits){printf("Mismatched size: GammaDetectorType\n"); result=false;}
  if(GammaDetectorLabel.size() != hits){printf("Mismatched size: GammaDetectorLabel\n"); result=false;}
  if(GammaDetectorSegm.size() != hits){printf("Mismatched size: GammaDetectorSegm\n"); result=false;}
  if(GammaRawADC.size() != hits){printf("Mismatched size: GammaRawADC\n"); result=false;}
  if(GammaADCChannel.size() != hits){printf("Mismatched size: GammaADCChannel\n"); result = false;}
  if(GammaTDCChannel.size() != hits){printf("Mismatched size: GammaTDCChannel\n"); result = false;}
  if(GammaTDCMultiplicity.size() != hits){printf("Mismatched size: GammaTDCMultiplicity\n"); result = false;}

  return result;
}

void GammaData::ClearEvent()
{
  GammaEnergy.clear();
//  GammaTheta.clear();
//  GammaPhi.clear();
  GammaTime.clear();
  GammaDetectorType.clear();
  GammaDetectorLabel.clear();
  GammaDetectorSegm.clear();
  GammaRawADC.clear();
  GammaADCChannel.clear();
  GammaTDCChannel.clear();
  GammaTDCMultiplicity.clear();
}

unsigned int GammaData::SizeOfEvent()
{
  unsigned int result = GammaEnergy.size();
  return result;
}
