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
  //The actual members of the class
  int GammaHits;

 //Physical values which are generated
  std::vector<double> GammaEnergy;
 // std::vector<double> GammaTheta;
//  std::vector<double> GammaPhi;
  std::vector<double> GammaTime;
  std::vector<std::string> GammaDetectorType;//This is a particular thing in this class which isn't in the SiliconData type. It defines the type of the gamma detector. E.g. HPGeClover, HPGe, Hagar, NaI, CsI, Eyeball, BGO, Kudu, peacock.
  //It's a STRING... i.e., type what you want in there and then test for that later on
  std::vector<int> GammaDetectorLabel;
 //std::vector<std::string> GammaDetectorSegm;
  std::vector<int> GammaDetectorSegm;

 //There should follow some diagnostic values which will be added for The Greater Good (The Greater Good).

  //Diagnostic values - these are things which might be useful to finding problems in the experiment or the sort code.
  std::vector<int> GammaRawADC;
  std::vector<int> GammaADCChannel;
  std::vector<int> GammaTDCChannel;
  std::vector<int> GammaTDCMultiplicity;


public :
//Setters - set the physical values for each physical event
  void SetHits(int hits){GammaHits = hits;}	//number of gamma events

  void SetEnergy(double ener){GammaEnergy.push_back(ener);}
//  void SetTheta(double thet){GammaTheta.push_back(thet);}
 // void SetPhi(double ph){GammaPhi.push_back(ph);}
  void SetTime(double tim){GammaTime.push_back(tim);}
  void SetDetectorType(std::string type){GammaDetectorType.push_back(type);}
  void SetDetectorLabel(int label){GammaDetectorLabel.push_back(label);}//"clover1" "hagar" "LaBr1" DATA->Draw("GammaEnergy:Ex","GammaDetectorLabel==\"clover1\"","")
  //void SetDetectorSegm(std::string label){GammaDetectorSegm.push_back(label);}
   void SetDetectorSegm(int segm){GammaDetectorSegm.push_back(segm);}
   void SetGammaRawADC(int val){GammaRawADC.push_back(val);}
   void SetGammaADCChannel(int chan){GammaADCChannel.push_back(chan);}
   void SetGammaTDCChannel(int chan){GammaTDCChannel.push_back(chan);}
   void SetGammaTDCMultiplicity(int mult){GammaTDCMultiplicity.push_back(mult);}

//Getters - return the value of the private member - shouldn't have them visible to the user
  double GetEnergy(int i){return GammaEnergy.at(i);}
//  double GetTheta(int i){return GammaTheta.at(i);}
//  double GetPhi(int i){return GammaPhi.at(i);}
  double GetTime(int i){return GammaTime.at(i);}
  std::string GetType(int i){return GammaDetectorType.at(i);}
  double GetLabel(int i){return GammaDetectorLabel.at(i);}
 // double GetDetectorHit(int i){return DetectorHit.at(i);}

//Should add Getters for the diagnostic values in case people want to use them to test events themselves under certain conditions
  void PrintEvent();
  bool TestEvent();
  void ClearEvent();

  unsigned int SizeOfEvent();
};

#endif
