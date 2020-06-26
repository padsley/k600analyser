#include "FocalplaneData.h"

FocalplaneData::FocalplaneData()
{  
}

FocalplaneData::~FocalplaneData()
{
//   printf("Destructor called for FocalplaneData");
  ClearEvent();
}

void FocalplaneData::PrintEvent()//Print out the 'proper' information about an event
{
  //Want to print out the number of valid focalplane hits
  printf("FocalplaneData PrintEvent routine: Focalplane hits: %d\n",SizeOfEvent());
  
  for(unsigned int i=0;i<SizeOfEvent();i++)
  {
    printf("Hit number: %d\n",i);
    printf("Position: %g\n",GetPosition(i));
  }
}

bool FocalplaneData::TestEvent()
{
  bool result = true;
//  Test the output data: not sure yet how to do that...
//  for the FP detector

  unsigned int hits = FocalplanePosition.size();

// this has to be rethought: it was designed for the DSSSDs and
// systems with many channels per event. For the FP detectors
// it will be different.

  //Check to see the same number of hits c.f. the energy hits. If you get a lot of warning messages from theta, phi, time here, that's likely because you're doing the energy hits wrong/differently
  if(FocalplaneTheta.size() != hits){printf("Mismatched size: Theta\n"); result = false;}
  if(FocalplanePhi.size() != hits){printf("Mismatched size: Phi\n"); result = false;}
  if(FocalplaneTime.size() != hits){printf("Mismatched size: Time\n"); result = false;}
  
  
  return result;
}

void FocalplaneData::ClearEvent()
{
  printf("FocalplaneData: ClearEvent()\n");

  FPDetHits.clear();
  FPDetPos.clear();
  FPDetPosO.clear();   
  FPDetPosC.clear();   
  FPDetY.clear();
  FPDetAngle.clear();
  FPDetdE.clear();

  // and what about the other variables that are not vectors?
 
}

// this has to be rethought: it was designed for the DSSSDs and
// systems with many channels per event. For the FP detectors
// it will be different.
unsigned int FocalplaneData::SizeOfEvent()
{
  unsigned int result = FPDetPos.size();
  return result;
}
