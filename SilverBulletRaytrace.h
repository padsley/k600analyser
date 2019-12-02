#ifndef __SilverBulletRaytrace__
#define __SilverBulletRaytrace__ 1

#include <vector>
#include <iostream>

#include "TMath.h"

//#include "Parameters.h"

static std::vector<std::vector<std::vector<std::vector<int>>>> permutationLibrary;

void GeneratePermutationLibrary(int maxN);
std::vector<std::vector<int>> GeneratePermuations(int N, int r);
void NextPermutation(std::vector<int> &permutation);

class SilverBulletRaytrace {
    
    bool    goodSbrEvent;
    int     goodSbrEventNr;
    double  sbrXPosition;
    double  sbrXPosition_TLC;
    double  sbrXPosition_TLC_mapped;
    double  sbrExcitationEnergy;
    double  sbrThetaRaytrace;
    double  sbrThetaRaytraceCal;
    double  sbrRsquared;
    double  sbrAdjustedRsquared;
    double  sbrNUsedWires;
    double  sbrNRemovedWires;
    
    int     nAlternativeEvents;
    int     nTotalWires;
    
    std::vector<double> xPositions;
    std::vector<double> xPositions_TLC;
    std::vector<double> xPositions_TLC_mapped;
    std::vector<double> excitationEnergies;
    std::vector<double> thetaRaytraces;
    std::vector<double> thetaRaytraceCals;
    std::vector<double> rSquareds;
    std::vector<double> adjustedRSquareds;
    std::vector<int>    nUsedWires;
    std::vector<int>    nRemovedWires;
    
    std::vector<std::vector<double>>    trackVisDriftTimes;
    std::vector<std::vector<double>>    trackVisDriftTimesRaw;
    std::vector<int>                    trackVisWireNumbers;

public :
  SilverBulletRaytrace();
  virtual ~SilverBulletRaytrace();
    
    int     GetNAlternativeEvents();
    double  GetXPosition(int eventN);
    double  GetXPosition_TLC(int eventN);
    void    SetXPosition_TLC(int eventN, double a);
    double  GetXPosition_TLC_mapped(int eventN);
    void    SetXPosition_TLC_mapped(int eventN, double a);
    void    SetExcitationEnergy(int eventN, double a);
    
    double  GetThetaRaytrace(int eventN);
    double  GetThetaRaytraceCal(int eventN);    
    void    SetThetaRaytraceCal(int eventN, double a);
    
    void AddEntry(double xPosition, double thetaRaytrace, double rSquared, double adjustedRSquared, int nWiresUsed, int nWiresRemoved, int nWiresTotal);
    void ArrangeDescendingRSquared();
    void ArrangeDescendingAdjustedRSquared();
    void ClearEvent();
    void DetermineGoodSbrEvent();
    
    void AddTrackVisDriftTimes(std::vector<double> driftDistances, std::vector<int> wireAccount);
    void AddTrackVisDriftTimesRaw(std::vector<double> driftDistances, std::vector<int> wireAccount);
    void DetermineTrackVisWireNumbers(std::vector<double> driftDistances, std::vector<int> wires, std::vector<int> wireAccount);
    
    void OptomiseDriftDistanceInversion(std::vector<double> driftDistances, std::vector<double>& driftDistances_corrected, std::vector<int> wires, std::vector<int> wireAccount);
    void Raytrace(double dd[],int wire[],int wire_num);
    void LeastSquaresFit(std::vector<double> driftDistances, std::vector<int> wires, std::vector<int> wireAccount, double &xPosition, double &thetaRaytrace, double &rSquared, double &adjustedRSquared);
};

#endif
