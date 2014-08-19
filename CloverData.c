#include "CloverData.h"


CloverData::CloverData()
{  
}

CloverData::~CloverData()
{
//   printf("CloverData destructor called");
}

void CloverData::PrintEvent()//Print out the 'proper' information about an event
{
  //Want to print out the number of valid Clover hits
  printf("Clover hits: %d\n",SizeOfEvent());
  
  for(unsigned int i=0;i<SizeOfEvent();i++)
  {
    printf("Hit number: %d\n",i);
    printf("Energy: %g\n",GetEnergy(i));
  }
}

bool CloverData::TestEvent()
{
  bool result = true;
//  Test the output data: do we have the same number of hits in each thing (energy, time etc.) and are the events good and shizzle like that 
 unsigned int hits = GammaEnergy.size();
//  printf("Number of Clover hits: %d\n",hits);
  //Check to see the same number of hits c.f. the energy hits. If you get a lot of warning messages from theta, phi, time here, that's likely because you're doing the energy hits wrong/differently
//   if(GammaTheta.size() != hits){printf("Mismatched size: Theta\n"); result = false;}
//   if(GammaPhi.size() != hits){printf("Mismatched size: Phi\n"); result = false;}
  if(GammaTime.size() != hits){printf("Mismatched size: Time\n"); result = false;}
  //Check to see that the calculated energy value lies within the right region - i.e. between the front and back energy stored for that channel
  for(unsigned int i=0; i<hits; i++)
  {
//     if(!((GammaEnergy.at(i)>EnergyFront.at(i) && GammaEnergy.at(i)<EnergyBack.at(i)) || (GammaEnergy.at(i)<EnergyFront.at(i) && GammaEnergy.at(i)>EnergyBack.at(i))))printf("GammaEnergy doesn't fall between the front and back energies");
  //Check to see that the computed angle is reasonable and that nothing weird is going on - you need to add you own checks here as detector positions will change
    if(GammaTheta.at(i)<90){printf("CloverTheta suggests a Clover detector at forward angles - problem in angle computation\n"); result = false;}
  }
  
  if(DetectorHit.size() != hits){printf("Mismatched size: DetectorHit"); result = false;}

  return result;
}

void CloverData::ClearEvent()
{
  GammaEnergy.clear();
  GammaTheta.clear();
  GammaPhi.clear();
  GammaTime.clear();
  
  DetectorHit.clear();
}

unsigned int CloverData::SizeOfEvent()
{
  unsigned int result = GammaEnergy.size();
  return result;
}