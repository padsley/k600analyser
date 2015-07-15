#include "SiliconData.h"

SiliconData::SiliconData()
{  
}

SiliconData::~SiliconData()
{
//   printf("Destructor called for SiliconData");
  ClearEvent();
}

void SiliconData::PrintEvent()//Print out the 'proper' information about an event
{
  //Want to print out the number of valid silicon hits
  printf("Silicon hits: %d\n",SizeOfEvent());
  
  for(unsigned int i=0;i<SizeOfEvent();i++)
  {
    printf("Hit number: %d\n",i);
    printf("Energy: %g\n",GetEnergy(i));
  }
}

bool SiliconData::TestEvent()
{
  bool result = true;
//  Test the output data: do we have the same number of hits in each thing (energy, time etc.) and are the events good and shizzle like that 
 unsigned int hits = SiliconEnergy.size();

  //Check to see the same number of hits c.f. the energy hits. If you get a lot of warning messages from theta, phi, time here, that's likely because you're doing the energy hits wrong/differently
  if(SiliconTheta.size() != hits){printf("Mismatched size: Theta\n"); result = false;}
  if(SiliconPhi.size() != hits){printf("Mismatched size: Phi\n"); result = false;}
  if(SiliconTime.size() != hits){printf("Mismatched size: Time\n"); result = false;}
  if(SiliconTimeFront.size() != hits){printf("Mismatched size: Front Time\n"); result = false;}
  if(SiliconTimeBack.size() != hits){printf("Mismatched size: Back Time\n"); result = false;}
  //Check to see that the calculated energy value lies within the right region - i.e. between the front and back energy stored for that channel
  for(unsigned int i=0; i<hits; i++)
  {
//     if(!((SiliconEnergy.at(i)>EnergyFront.at(i) && SiliconEnergy.at(i)<EnergyBack.at(i)) || (SiliconEnergy.at(i)<EnergyFront.at(i) && SiliconEnergy.at(i)>EnergyBack.at(i))))printf("SiliconEnergy doesn't fall between the front and back energies");
  //Check to see that the computed angle is reasonable and that nothing weird is going on - you need to add you own checks here as detector positions will change
    if(SiliconTheta.at(i)<90){printf("SiliconTheta suggests a silicon detector at forward angles - problem in angle computation\n"); result = false;}
  }
  
  if(DetectorHit.size() != hits){printf("Mismatched size: DetectorHit"); result = false;}
  if(ADCChannelFront.size() != hits){printf("Mismatched size: ADCChannelFront\n"); result = false;}
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
  
  return result;
}

void SiliconData::ClearEvent()
{
//   printf("SiliconData: ClearEvent()\n");
  SiliconEnergy.clear();
  SiliconTheta.clear();
  SiliconPhi.clear();
  SiliconTime.clear();
  
  DetectorHit.clear();
  ADCChannelFront.clear();
  ADCChannelBack.clear();
  TDCChannelFront.clear();
  TDCChannelBack.clear();
  ADCValueFront.clear();
  ADCValueBack.clear();
  TDCValueFront.clear();
  TDCValueBack.clear();
  StripFront.clear();
  StripBack.clear();
  EnergyFront.clear();
  EnergyBack.clear();
}

unsigned int SiliconData::SizeOfEvent()
{
  unsigned int result = SiliconEnergy.size();
  return result;
}
