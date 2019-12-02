#include "SilverBulletRaytrace.h"
//      An adaption (by K.C.W. Li) of the pre-existing raytracing algorithm in Focalplane.c written by R. Neveling

//--------------------------------------------------------------------------------------
SilverBulletRaytrace::SilverBulletRaytrace()
{
}

SilverBulletRaytrace::~SilverBulletRaytrace()
{
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::AddEntry(double xPosition, double thetaRaytrace, double rSquared, int nWiresUsed, int nWiresRemoved, int nWiresTotal)
{
    xPositions.push_back(xPosition);
    thetaRaytraces.push_back(thetaRaytrace);
    rSquareds.push_back(rSquared);
    nUsedWires.push_back(nWiresUsed);
    nRemovedWires.push_back(nWiresRemoved);
    nTotalWires.push_back(nWiresTotal);
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::ArrangeDescendingRSquared()
{
    static int counter = 0;

    if(counter==0)
    {
        for(int i=0; i<static_cast<int>(rSquareds.size()); i++)
        {
            std::cout << "xPositions[i]: " << xPositions[i] << std::endl;
            std::cout << "thetaRaytraces[i]: " << thetaRaytraces[i] << std::endl;
            std::cout << "rSquareds[i]: " << rSquareds[i] << std::endl;
            std::cout << "nUsedWires[i]: " << nUsedWires[i] << std::endl;
            std::cout << "nRemovedWires[i]: " << nRemovedWires[i] << std::endl;
            std::cout << "nTotalWires[i]: " << nTotalWires[i] << std::endl;
            std::cout << std::endl;
        }
    }

    for(int i=0; i<static_cast<int>(rSquareds.size()); i++)
    {
        for(int j=i+1; j<static_cast<int>(rSquareds.size()); j++)
        {
            if(rSquareds[j]>rSquareds[i])
            {
                double temp_xPosition = xPositions[i];
                double temp_thetaRaytrace = thetaRaytraces[i];
                double temp_rSquared = rSquareds[i];
                double temp_nUsedWires = nUsedWires[i];
                double temp_nRemovedWires = nRemovedWires[i];
                double temp_nTotalWires = nTotalWires[i];

                //------------------------------------------------
                xPositions[i] = xPositions[j];
                thetaRaytraces[i] = thetaRaytraces[j];
                rSquareds[i] = rSquareds[j];
                nUsedWires[i] = nUsedWires[j];
                nRemovedWires[i] = nRemovedWires[j];
                nTotalWires[i] = nTotalWires[j];

                //------------------------------------------------
                xPositions[j] = temp_xPosition;
                thetaRaytraces[j] = temp_thetaRaytrace;
                rSquareds[j] = temp_rSquared;
                nUsedWires[j] = temp_nUsedWires;
                nRemovedWires[j] = temp_nRemovedWires;
                nTotalWires[j] = temp_nTotalWires;
            }
        }
    }
    
    if(counter==0)
    {
        std::cout << "AFTER" << std::endl;

        for(int i=0; i<static_cast<int>(rSquareds.size()); i++)
        {
            std::cout << "xPositions[i]: " << xPositions[i] << std::endl;
            std::cout << "thetaRaytraces[i]: " << thetaRaytraces[i] << std::endl;
            std::cout << "rSquareds[i]: " << rSquareds[i] << std::endl;
            std::cout << "nUsedWires[i]: " << nUsedWires[i] << std::endl;
            std::cout << "nRemovedWires[i]: " << nRemovedWires[i] << std::endl;
            std::cout << "nTotalWires[i]: " << nTotalWires[i] << std::endl;
            std::cout << std::endl;
        }
    }
    
    counter++;
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::ClearEvent()
{
    xPositions.clear();
    thetaRaytraces.clear();
    rSquareds.clear();
    nUsedWires.clear();
    nRemovedWires.clear();
    nTotalWires.clear();
    
    trackVisDriftTimes.clear();
    trackVisDriftTimesRaw.clear();
    trackVisWireNumbers.clear();
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::AddTrackVisDriftTimes(std::vector<double> driftDistances, std::vector<int> wireAccount)
{
    std::vector<double> temp_trackVisDriftTimes;
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        if(wireAccount[i])
        {
            temp_trackVisDriftTimes.push_back(driftDistances[i]);
        }
        else
        {
            temp_trackVisDriftTimes.push_back(-10.0);
        }
    }
    
    trackVisDriftTimes.push_back(temp_trackVisDriftTimes);
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::AddTrackVisDriftTimesRaw(std::vector<double> driftDistances, std::vector<int> wireAccount)
{
    std::vector<double> temp_trackVisDriftTimesRaw;
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        if(wireAccount[i])
        {
            temp_trackVisDriftTimesRaw.push_back(driftDistances[i]);
        }
        else
        {
            temp_trackVisDriftTimesRaw.push_back(-10.0);
        }
    }
    
    trackVisDriftTimesRaw.push_back(temp_trackVisDriftTimesRaw);
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::DetermineTrackVisWireNumbers(std::vector<double> driftDistances, std::vector<int> wireAccount)
{
    int     smallestDriftDistanceIndex = 0;
    double  smallestDriftDistance = 0.0;
    bool    foundFirstUsedWire = false;
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        if(wireAccount[i]==1 && (!foundFirstUsedWire || driftDistances[i]<smallestDriftDistance))
        {
            foundFirstUsedWire = true;
            smallestDriftDistanceIndex = i;
            smallestDriftDistance = driftDistances[i];
        }
    }
    
    trackVisWireNumbers.clear();
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        trackVisWireNumbers.push_back(i-smallestDriftDistanceIndex);
    }
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::OptomiseDriftDistanceInversion(std::vector<double> driftDistances, std::vector<double>& driftDistances_corrected, std::vector<int> wires, std::vector<int> wireAccount)
{
    //------------------------------------------------------------------------------------
    //      First determine the optimal wire of inversion: the optimal wire for which
    //      all proceeding wires have inverted drift distances
    int     optimalWireIndexOfInversion = 0;
    double  optimalRSquaredOfInversion = 0.0;
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        double  xPosition, thetaRaytrace, rSquared;
        
        //------------------------------------------------------------------------------------
        // This accounts for whether a wire should be used for the raytracing. Wire "i" is to be used if and only if wireAccount[i]==1.
        std::vector<double> driftDistances_inversionTest;
        
        for(int j=0; j<static_cast<int>(driftDistances.size()); j++)
        {
            driftDistances_inversionTest.push_back(driftDistances[j]);
            
            if(j>=i)
            {
                driftDistances_inversionTest[j] *= -1.0;
            }
        }
        
        //------------------------------------------------------------------------------------
        LeastSquaresFit(driftDistances_inversionTest, wires, wireAccount, xPosition, thetaRaytrace, rSquared);
        
        if(i==0 || (rSquared>optimalRSquaredOfInversion))
        {
            optimalWireIndexOfInversion = i;
            optimalRSquaredOfInversion = rSquared;
        }
    }
    
    //------------------------------------------------
    driftDistances_corrected.clear();
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        driftDistances_corrected.push_back(driftDistances[i]);
        
        if(i>=optimalWireIndexOfInversion)
        {
            driftDistances_corrected[i] *= -1.0;
        }
    }
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::Raytrace(double dd[],int wire[],int wire_num)
{
    //------------------------------------------------
    std::vector<double> driftDistances;
    std::vector<double> driftDistances_corrected;
    std::vector<int> wires;
    
    for(int i=0; i<wire_num; i++)
    {
        driftDistances.push_back(dd[i]);
        wires.push_back(wire[i]);
    }
    
    driftDistances_corrected = driftDistances;
    
    /*
    //------------------------------------------------------------------------------------
    //      First determine the optimal wire of inversion: the optimal wire for which
    //      all proceeding wires have inverted drift distances
    int     optimalWireIndexOfInversion = 0;
    double  optimalRSquaredOfInversion = 0.0;

    for(int i=0; i<wire_num; i++)
    {
        double rSquared;

        //------------------------------------------------------------------------------------
        // This accounts for whether a wire should be used for the raytracing. Wire "i" is to be used if and only if wireAccount[i]==1.
        std::vector<double> driftDistances_inversionTest;
        auto wireAccount_inversionTest = std::vector<int>(wire_num, 1);
        
        for(int j=0; j<wire_num; j++)
        {
            driftDistances_inversionTest.push_back(driftDistances[j]);
            
            if(j>=i)
            {
                driftDistances_inversionTest[j] *= -1.0;
            }
        }
        
        //------------------------------------------------------------------------------------
        LeastSquaresFit(driftDistances_inversionTest, wires, wireAccount_inversionTest, xPosition, thetaRaytrace, rSquared);
        
        if(i==0 || (rSquared>optimalRSquaredOfInversion))
        {
            optimalWireIndexOfInversion = i;
            optimalRSquaredOfInversion = rSquared;
        }
    }
    
    //------------------------------------------------
    std::vector<double> driftDistances_corrected;
    
    for(int i=0; i<wire_num; i++)
    {
        driftDistances_corrected.push_back(driftDistances[i]);
        
        if(i>=optimalWireIndexOfInversion)
        {
            driftDistances_corrected[i] *= -1.0;
        }
    }
    */
    
    
    //--------------------------------------------------------
    //      Raytrace without removal of wires
    double  xPosition, thetaRaytrace;
    double rSquared_best = 0.0;

    int nRemovedWires = 0;
    auto wireAccount = std::vector<int>(wire_num, 1);
    
    OptomiseDriftDistanceInversion(driftDistances, driftDistances_corrected, wires, wireAccount);

    LeastSquaresFit(driftDistances_corrected, wires, wireAccount, xPosition, thetaRaytrace, rSquared_best);
    AddEntry(xPosition, thetaRaytrace, rSquared_best, (wire_num-nRemovedWires), nRemovedWires, wire_num);
    AddTrackVisDriftTimes(driftDistances_corrected, wireAccount);
    AddTrackVisDriftTimesRaw(driftDistances, wireAccount);
    
    //--------------------------------------------------------------------------------------
    //      Raytrace with the wire corresponding to the minimum drift distance removed
    int     smallestDriftDistanceIndex = 0;
    double  smallestDriftDistance = 0.0;
    
    for(int i=0; i<wire_num; i++)
    {
        if(i==0 || driftDistances[i]<smallestDriftDistance)
        {
            smallestDriftDistanceIndex = i;
            smallestDriftDistance = driftDistances[i];
        }
    }
    
    bool removeMinimumDriftDistanceFirst = false;
    
    if(removeMinimumDriftDistanceFirst)
    {
        wireAccount[smallestDriftDistanceIndex] = 0;
        nRemovedWires++;
        
        OptomiseDriftDistanceInversion(driftDistances, driftDistances_corrected, wires, wireAccount);
        LeastSquaresFit(driftDistances_corrected, wires, wireAccount, xPosition, thetaRaytrace, rSquared_best);
        AddEntry(xPosition, thetaRaytrace, rSquared_best, (wire_num-nRemovedWires), nRemovedWires, wire_num);
        AddTrackVisDriftTimes(driftDistances_corrected, wireAccount);
        AddTrackVisDriftTimesRaw(driftDistances, wireAccount);
    }
    
    //--------------------------------------------------------
    /*
    for(int i=0; i<wire_num; i++)
    {
        trackVisWireNumbers.push_back(i-smallestDriftDistanceIndex);
    }
    */

    //------------------------------------------------
    double rSquaredThreshold = 0.995;
    while(nRemovedWires<wire_num && (wire_num-(nRemovedWires+1))>=4 && rSquared_best<rSquaredThreshold)
    {
        //------------------------------------------------
        int     wireRemovalIndex_Best = 0;
        //double  xPosition_best = 0.0, thetaRaytrace_best = 0.0, rSquared_best = 0.0;
        double  rSquared;
        double  xPosition_best = 0.0, thetaRaytrace_best = 0.0;
        
        //------------------------------------------------------------------------------------
        //      Determine which wire is optimal (w.r.t. to the rSquared metric) to remove
        bool firstUsedWireFound = false;
        for(int i=0; i<wire_num; i++)
        {
            if(wireAccount[i]==1)
            {
                wireAccount[i] = 0;
                
                //------------------------------------------------
                OptomiseDriftDistanceInversion(driftDistances, driftDistances_corrected, wires, wireAccount);
                LeastSquaresFit(driftDistances_corrected, wires, wireAccount, xPosition, thetaRaytrace, rSquared);
                
                //------------------------------------------------
                if(!firstUsedWireFound || (rSquared>rSquared_best))
                {
                    firstUsedWireFound = true;
                    wireRemovalIndex_Best = i;
                    xPosition_best = xPosition;
                    thetaRaytrace_best = thetaRaytrace;
                    rSquared_best = rSquared;
                }
                
                wireAccount[i] = 1;
            }
        }
        
        //------------------------------------------------------------------------------------
        nRemovedWires++;
        wireAccount[wireRemovalIndex_Best] = 0;
        OptomiseDriftDistanceInversion(driftDistances, driftDistances_corrected, wires, wireAccount);

        AddEntry(xPosition_best, thetaRaytrace_best, rSquared_best, (wire_num-nRemovedWires), nRemovedWires, wire_num);
        AddTrackVisDriftTimes(driftDistances_corrected, wireAccount);
        AddTrackVisDriftTimesRaw(driftDistances, wireAccount);
    }
    
    DetermineTrackVisWireNumbers(driftDistances, wireAccount);
    
    //------------------------------------------------------------------------------------
    ArrangeDescendingRSquared();
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::LeastSquaresFit(std::vector<double> driftDistances, std::vector<int> wires, std::vector<int> wireAccount, double &xPosition, double &thetaRaytrace, double &rSquared)
{
    const double X_WIRESPACE = 4.0;     // distance (mm) between signal wires

    //------------------------------------------------------------------------------------------------
    //      Now do the least square fit
    double sum_0 = 0.0, sum_x = 0.0, sum_z = 0.0, sum_xz = 0.0, sum_x2 = 0.0;
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        if(wireAccount[i]==1)
        {
            double x_tttv = X_WIRESPACE*(float(wires[i]));   // left -> high Ex
            sum_0  += 1.0;
            sum_x  += x_tttv;
            sum_z  += driftDistances[i];
            sum_xz += x_tttv*driftDistances[i];
            sum_x2 += TMath::Power(x_tttv,2);
        }
    }
    
    double a1 = (sum_x*sum_z-sum_0*sum_xz)/(TMath::Power(sum_x,2)-sum_0*sum_x2);
    double b1 = (sum_x*sum_xz-sum_x2*sum_z)/(TMath::Power(sum_x,2)-sum_0*sum_x2);
    
    xPosition  = (-1.)*b1/a1;
    thetaRaytrace = (-1.)*57.29578*atan(a1); // [deg.]
    
    double averagedd=sum_z/sum_0;
    
    //------------------------------------------------------------------------------------------------
    //      From this loop we obtain really sum of squares of residuals and total sum of squares
    double sstot = 0.0, ssres = 0.0, wirechisq = 0.0;
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        if(wireAccount[i]==1)
        {
            ssres += TMath::Power(driftDistances[i] - (a1*X_WIRESPACE*(wires[i])+b1),2);
            ++wirechisq;
            sstot += TMath::Power(driftDistances[i] - averagedd,2);
        }
    }
    
    rSquared = 1.0 - (ssres/sstot);
}
