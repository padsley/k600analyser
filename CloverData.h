#ifndef __CloverData__
#define __CloverData__

/*
 * 
 * This is a header file defining the data structure for Gamma clover events for K600 analysis.
 * It should be used as a basis for another analysis file, an example of which should be found in the same directory.
 * 
 * Author: Philip Adsley, June 2014
 * E-mail: padsley@gmail.com
 * 
 * Please contact me if you find errors or have useful suggestions
 * 
 */
#include <vector>
#include <stdio.h>
#include <TCutG.h>
class CloverData
{
public :
  CloverData();
  virtual ~CloverData();
  
  private :
  //The actual members of the class
  //Physical values which are generated
  std::vector<double> GammaEnergy;
  std::vector<double> GammaTheta;
  std::vector<double> GammaPhi;
  std::vector<double> GammaTime;
  
  //Diagnostic values - these are things which might be useful to finding problems in the experiment or the sort code.
  std::vector<int> DetectorHit;
  
public :
  //Setters - set the physical values for each physical event
  void SetEnergy(double ener)		{GammaEnergy.push_back(ener);}
  void SetTheta(double thet)		{GammaTheta.push_back(thet);}
  void SetPhi(double ph)		{GammaPhi.push_back(ph);}
  void SetTime(double tim)		{GammaTime.push_back(tim);}
  
  void SetDetectorHit(int det)		{DetectorHit.push_back(det);}
  
  //Getters - return the value of the private member - shouldn't have them visible to the user
  double GetEnergy(int i)		{return GammaEnergy.at(i);}
  double GetTheta(int i)		{return GammaTheta.at(i);}
  double GetPhi(int i)			{return GammaPhi.at(i);}
  double GetTime(int i)			{return GammaTime.at(i);}  
  
//   TCutG *GetFrontBackEnergyCut() 	{return mFrontBackEnergyCut;}
  
  void PrintEvent();
  bool TestEvent();
  void ClearEvent();
  
  
  
  unsigned int SizeOfEvent();
};

#endif