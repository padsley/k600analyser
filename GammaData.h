#ifndef __GammaData__
#define __GammaData__ 1

/*
 * 
 * This is a header file defining the data structure for ganna events for K600 analysis.
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
#include <string>

class GammaData
{
 public :
  GammaData();
  virtual ~GammaData();

 private : 
  int GammaHits;

  std::vector<double> GammaEnergy;
  std::vector<double> GammaTheta;
  std::vector<double> GammaPhi;
  std::vector<double> GammaTime;
  std::vector<std::string> GammaDetectorType;//This is a particular thing in this class which isn't in the SiliconData type. It defines the type of the gamma detector. E.g. HPGeClover, HPGe, Hagar, NaI, CsI, Eyeball, BGO, Kudu, peacock.
  //It's a STRING... i.e., type what you want in there and then test for that later on

  //There should follow some diagnostic values which will be added for The Greater Good (The Greater Good).

  void SetHits(int hits){GammaHits = hits;}

  void SetEnergy(double ener){GammaEnergy.push_back(ener);}
  void SetTheta(double thet){GammaTheta.push_back(thet);}
  void SetPhi(double ph){GammaPhi.push_back(ph);}
  void SetTime(double tim){GammaTime.push_back(tim);}
  void SetDetectorType(std::string type){GammaDetectorType.push_back(type);}

  double GetEnergy(int i){return GammaEnergy.at(i);}
  double GetTheta(int i){return GammaTheta.at(i);}
  double GetPhi(int i){return GammaPhi.at(i);}
  double GetTime(int i){return GammaTime.at(i);}
  std::string GetType(int i){return GammaDetectorType.at(i);}

  void PrintEvent();
  bool TestEvent();
  void ClearEvent();

  unsigned int SizeOfEvent();
};

#endif
