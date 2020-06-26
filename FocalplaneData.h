#ifndef __FocalplaneData__
#define __FocalplaneData__ 1

/*
 * 
 * This is a header file defining the data structure for silicon events for K600 analysis.
 * It should be used as a basis for another analysis file, an example of which should be found in the same directory.
 * 
 * Author: Retief Neveling, June 2020    (shamelessly copied from P Adsley's earlier work)
 * E-mail: neveling@tlabs.ac.za
 * 
 * Please contact me if you find errors or have useful suggestions
 * 
 */
#include <vector>
#include <stdio.h>
#include <TCutG.h>

class FocalplaneData
{
public :
  FocalplaneData();
  virtual ~FocalplaneData();
  
  private :
  //The actual members of the class

  //Parameters specific to the use of VDCs/MWDCs
  std::vector<double> FPDetHits;
  std::vector<double> FPDetPos;
  std::vector<double> FPDetPosO;   //offset added
  std::vector<double> FPDetPosC;   //corrected position
  std::vector<double> FPDetY;
  std::vector<double> FPDetAngle;
  std::vector<double> FPDetdE;

  //Physical values which are generated
  double FPtof;
  double FPdE;
  double FPX; 
  double FPRigidity;
  double FPEnergy;
  double FPEx;
  double FPTheta;  
  double FPPhi;  
  double FPThetaSCAT;
  double FPPhiSCAT; 
 
  
public :
  //Setters - set the physical values for each physical event
  void SetFPDetHits(int hits)		{FPDetHits.push_back(hits);}
  void SetFPDetPos(double position)	{FPDetPos.push_back(position);}
  void SetFPDetPosO(double position)	{FPDetPosO.push_back(position);}
  void SetFPDetPosC(double position)	{FPDetPosC.push_back(position);}
  void SetFPDetY(double yposition)	{FPDetY.push_back(yposition);}
  void SetFPDetAngle(double thet)	{FPDetAngle.push_back(thet);}
  void SetFPDetdE(double de)		{FPDetdE.push_back(de);}

  void SetFPtof(int tof)		{FPtof = tof;}
  void SetFPdE(int de)			{FPde = de;}
  void SetFPX(int x)			{FPtof = x;}
  void SetFPRigidity(int rigidity)	{FPtof = rigidity;}
  void SetFPEnergy(int energy)		{FPtof = energy;}
  void SetFPEx(int ex)			{FPtof = ex;}
  void SetFPtheta(int theta)		{FPtof = theta;}
  void SetFPphi(int phi)		{FPtof = phi;}
  void SetFPthetaSCAT(int thetascat)	{FPtof = thetascat;}
  void SetFPphiSCAT(int phiscat)	{FPtof = phiscat;}
   
  //Getters - return the value of the private member - shouldn't have them visible to the user
  //double GetFPtof(int i)		{return FPtof.at(i);}
  //double GetFPdE(int i)			{return FPTheta.at(i);}
  //double GetFPX(int i)			{return FPPhi.at(i);}
  //double GetFPtheta(int i)		{return FPTime.at(i);}  
  
  void PrintEvent();
  bool TestEvent();
  void ClearEvent();
  
  unsigned int SizeOfEvent();
};

#endif
