#include "GammaData.h"

GammaData::GammaData()
{
  //Constructor
}

GammaData::~GammaData()
{
  //Destructor
}

//================================
void GammaData::PrintEvent()
{
  printf("Gamma hits: %d\n",SizeOfEvent());

  for(unsigned int i=0;i<SizeOfEvent();i++)
    {
      printf("Hit number: %d\n",i);
      printf("Energy: %g\n",GetEnergy(i));
    }
}


//================================
bool GammaData::TestEvent()
{
  bool result = true;

  //Need to introduce test conditions for event checking
  unsigned int hits = GammaEnergy.size();

  if(GammaTheta.size() != hits){printf("Mismatched size : Theta\n"); result = false;}
  if(GammaPhi.size() != hits){printf("Mismatched size: Phi\n"); result = false;}
  if(GammaTime.size() != hits){printf("Mismatched size: Time \n"); result = false;}
  if(GammaDetectorType.size() != hits){printf("Mismatched size: Detector Type\n");result=false;}

  return result;
}


//================================
void GammaData::ClearEvent()
{
  GammaEnergy.clear();
  GammaTheta.clear();
  GammaPhi.clear();
  GammaTime.clear();
  GammaDetectorType.clear();
  
  GammaADCChannel.clear();
  GammaTDCChannel.clear();
  GammaADCValue.clear();
  GammaTDCValue.clear();

}


//================================
unsigned int GammaData::SizeOfEvent()
{
  unsigned int result = GammaEnergy.size();
  return result;
}


