#ifndef __SilverBulletRaytrace__
#define __SilverBulletRaytrace__ 1

#include <vector>
#include <iostream>

#include "TMath.h"

//#include "Parameters.h"

class SilverBulletRaytrace {
    
    std::vector<double> xPositions;
    std::vector<double> thetaRaytraces;
    std::vector<double> rSquareds;
    std::vector<int>    nUsedWires;
    std::vector<int>    nRemovedWires;
    std::vector<int>    nTotalWires;
    
    std::vector<std::vector<double>> trackVisDriftTimes;
    std::vector<std::vector<double>> trackVisDriftTimesRaw;
    std::vector<int> trackVisWireNumbers;

public :
  SilverBulletRaytrace();
  virtual ~SilverBulletRaytrace();
    
    void AddEntry(double xPosition, double thetaRaytrace, double rSquared, int nWiresUsed, int nWiresRemoved, int nWiresTotal);
    void ArrangeDescendingRSquared();
    void ClearEvent();

    void AddTrackVisDriftTimes(std::vector<double> driftDistances, std::vector<int> wireAccount);
    void AddTrackVisDriftTimesRaw(std::vector<double> driftDistances, std::vector<int> wireAccount);
    void DetermineTrackVisWireNumbers(std::vector<double> driftDistances, std::vector<int> wireAccount);
    
    void OptomiseDriftDistanceInversion(std::vector<double> driftDistances, std::vector<double>& driftDistances_corrected, std::vector<int> wires, std::vector<int> wireAccount);
    void Raytrace(double dd[],int wire[],int wire_num);
    void LeastSquaresFit(std::vector<double> driftDistances, std::vector<int> wires, std::vector<int> wireAccount, double &xPosition, double &thetaRaytrace, double &rSquare);
};

#endif
