#ifndef __SiliconData__
#define __SiliconData__ 1

/*
 * 
 * This is a header file defining the data structure for silicon events for K600 analysis.
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

class SiliconData
{
public :
  SiliconData();
  virtual ~SiliconData();
  
  private :
  //The actual members of the class
  int SiliconHits;
  
  //Physical values which are generated
  std::vector<double> SiliconEnergy;
  std::vector<double> SiliconTheta;
  std::vector<double> SiliconPhi;
  std::vector<double> SiliconTime;
  std::vector<double> SiliconSA;
  
  //Diagnostic values - these are things which might be useful to finding problems in the experiment or the sort code.
  std::vector<int> Detector;         // nr of detector that was hit
  std::vector<int> ADCChannelFront;
  std::vector<int> ADCChannelBack;
  std::vector<int> TDCChannelFront;
  std::vector<int> TDCChannelBack;
  std::vector<int> ADCValueFront;
  std::vector<int> ADCValueBack;
  std::vector<int> TDCValueFront;
  std::vector<int> TDCValueBack;
  std::vector<double> EnergyFront;
  std::vector<double> EnergyBack;  
  std::vector<double> RelativeTheta;
  std::vector<int> Ring;
  std::vector<int> Sector;


  std::vector<int> TDCChannelMultiplicity; //This gives the multiplicity of the TDC channel which the silicon event is taken from which is included for if the user is concerned about whether the multiTDC code is doing what it's supposed to. Not currently populated.
  
public :
  //Setters - set the physical values for each physical event
  void SetHits(int hits)		{SiliconHits = hits;}
  
  void SetEnergy(double ener)		{SiliconEnergy.push_back(ener);}
  void SetTheta(double thet)		{SiliconTheta.push_back(thet);}
  void SetPhi(double ph)		{SiliconPhi.push_back(ph);}
  void SetTime(double tim)		{SiliconTime.push_back(tim);}
  void SetSA(double sa)			{SiliconSA.push_back(sa);}
  
  void SetDetector(int det)		{Detector.push_back(det);}
  void SetADCChannelFront(int chan)	{ADCChannelFront.push_back(chan);}
  void SetADCChannelBack(int chan)	{ADCChannelBack.push_back(chan);}
  void SetTDCChannelFront(int chan)	{TDCChannelFront.push_back(chan);}
  void SetTDCChannelBack(int chan)	{TDCChannelBack.push_back(chan);}
  void SetADCValueFront(int chan)	{ADCValueFront.push_back(chan);}
  void SetADCValueBack(int chan)	{ADCValueBack.push_back(chan);}
  void SetTDCValueFront(int chan)	{TDCValueFront.push_back(chan);}
  void SetTDCValueBack(int chan)	{TDCValueBack.push_back(chan);}
  void SetEnergyFront(double ener)	{EnergyFront.push_back(ener);}
  void SetEnergyBack(double ener)	{EnergyBack.push_back(ener);}
  void SetMult(int mult)		{TDCChannelMultiplicity.push_back(mult);}
  void SetRelativeTheta(double theta)	{RelativeTheta.push_back(theta);}
  void SetRing(int ringnr)		{Ring.push_back(ringnr);}
  void SetSector(int sectornr)		{Sector.push_back(sectornr);}
 


  //Getters - return the value of the private member - shouldn't have them visible to the user
  double GetEnergy(int i)		{return SiliconEnergy.at(i);}
  double GetTheta(int i)		{return SiliconTheta.at(i);}
  double GetPhi(int i)			{return SiliconPhi.at(i);}
  double GetTime(int i)			{return SiliconTime.at(i);}  
  double GetSA(int i)			{return SiliconSA.at(i);}  
  int GetADCChannelFront(int i)		{return ADCChannelFront.at(i);}  
  int GetADCChannelBack(int i)		{return ADCChannelBack.at(i);}  
  int GetTDCChannelFront(int i)		{return TDCChannelFront.at(i);}  
  int GetTDCValueFront(int i)		{return TDCValueFront.at(i);}  
  int GetDetector(int i)		{return Detector.at(i);}  
  int GetRing(int i)			{return Ring.at(i);}  
  int GetSector(int i)			{return Sector.at(i);}  
  
  //Should add Getters for the diagnostic values in case people want to use them to test events themselves under certain conditions
  
  void PrintEvent();
  bool TestEvent();
  void ClearEvent();
  
  unsigned int SizeOfEvent();
};

#endif
