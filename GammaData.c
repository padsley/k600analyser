#include "GammaData.h"

GammaData::GammaData()
{
  //Constructor
}

GammaData::~GammaData()
{
  //Destructor
}

void GammaData::PrintEvent()
{
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

  //Need to introduce test conditions for event checking

  return result;
}

void GammaData::ClearEvent()
{
  
}

unsigned int GammaData::SizeOfEvent()
{
  unsigned int result = GammaEnergy.size();
  return result;
}
