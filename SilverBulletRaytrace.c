#include "SilverBulletRaytrace.h"
//      An adaption (by K.C.W. Li) of the pre-existing raytracing algorithm in Focalplane.c written by R. Neveling

int dataLabel_CombN;

//--------------------------------------------------------------------------------------
void GeneratePermutationLibrary(int maxN)
{
    for(int i=0; i<maxN; i++)
    {
        std::vector<std::vector<std::vector<int>>> permutations_perNTotalWires;
        
        for(int j=0; j<=i; j++)
        {
            int N = i+1;
            int r = j+1;
            
            permutations_perNTotalWires.push_back(GeneratePermuations(N, r));
        }
        
        permutationLibrary.push_back(permutations_perNTotalWires);
    }
    
    /*
    for(int i=0; i<permutationLibrary.size(); i++)
    {
        for(int j=0; j<permutationLibrary[i].size(); j++)
        {
            for(int k=0; k<permutationLibrary[i][j].size(); k++)
            {
                for(int l=0; l<permutationLibrary[i][j][k].size(); l++)
                {
                    std::cout << permutationLibrary[i][j][k][l] << " ";
                }

                std::cout << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::endl << std::endl;
    }
    */
}

//--------------------------------------------------------------------------------------
std::vector<std::vector<int>> GeneratePermuations(int N, int r)
{
    //------------------------------------------------
    double NFactorial = TMath::Factorial(N);
    double NMinusRFactorial = TMath::Factorial(N-r);
    double rFactorial = TMath::Factorial(r);
    
    int nPermutations = static_cast<int>(NFactorial/(NMinusRFactorial*rFactorial));
    
    //----------------------------------------------------
    //      Setting up the first initial permutation
    std::vector<std::vector<int>> permutations;
    
    auto permutation = std::vector<int>(N, 0);
    for(int i=0; i<r; i++)
    {
        permutation[i] = 1;
    }
    
    permutations.push_back(permutation);
    
    //------------------------------------------------
    //      Cyling through the permutations
    for(int i=0; i<nPermutations-1; i++)
    {
        NextPermutation(permutation);
        permutations.push_back(permutation);
    }
    
    return permutations;
}

//--------------------------------------------------------------------------------------------
//      Iterates the permutation-vector argument to its next lexicographic permutation.
//      The iteration is cyclical, i.e. if the argument is the last lexicographic
//      permutation, then the argument is iterated to the first lexicographic permutation.
void NextPermutation(std::vector<int> &permutation)
{
    bool    foundFirstPossibleIncrement = false;
    int     nChoicesFound = 0;
    
    int i = 0;
    while(i<static_cast<int>(permutation.size()) && !foundFirstPossibleIncrement)
    {
        if(permutation[i]==1)
        {
            //------------------------------------------------
            if(i!=static_cast<int>(permutation.size())-1)
            {
                if(permutation[i+1]==0)
                {
                    //------------------------------------------------
                    foundFirstPossibleIncrement = true;
                    
                    permutation[i] = 0;
                    permutation[i+1] = 1;
                    
                    //------------------------------------------------
                    for(int j=0; j<i; j++)
                    {
                        permutation[j] = 0;
                    }
                    
                    for(int j=0; j<nChoicesFound; j++)
                    {
                        permutation[j] = 1;
                    }
                }
                else
                {
                    nChoicesFound++;
                }
            }
            else // This is for the case in which the argument is the last lexicographic permutation
            {
                //--------------------------------------------------------------------
                int nTotalChoices = 0;
                
                for(int j=0; j<static_cast<int>(permutation.size()); j++)
                {
                    if(permutation[j]==1)
                    {
                        nTotalChoices++;
                        permutation[j] = 0;
                    }
                }
                
                //--------------------------------------------------------------------
                for(int j=0; j<nTotalChoices; j++)
                {
                    permutation[j] = 1;
                }
            }
        }
        
        i++;
    }
}

//--------------------------------------------------------------------------------------
SilverBulletRaytrace::SilverBulletRaytrace()
{
}

SilverBulletRaytrace::~SilverBulletRaytrace()
{
}

//--------------------------------------------------------------------------------------
int SilverBulletRaytrace::GetNAlternativeEvents()
{
    return nAlternativeEvents;
}

//--------------------------------------------------------------------------------------
double SilverBulletRaytrace::GetXPosition(int eventN)
{
    double xPosition = 0.0;
    
    if(eventN<nAlternativeEvents)
    {
        xPosition = xPositions[eventN];
    }
    
    return xPosition;
}

//--------------------------------------------------------------------------------------
double SilverBulletRaytrace::GetXPosition_TLC(int eventN)
{
    double XPosition_TLC = 0.0;
    
    if(eventN<nAlternativeEvents)
    {
        XPosition_TLC = xPositions_TLC[eventN];
    }
    
    return XPosition_TLC;
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::SetXPosition_TLC(int eventN, double a)
{
    if(eventN<nAlternativeEvents)
    {
        xPositions_TLC[eventN] = a;
    }
}

//--------------------------------------------------------------------------------------
double SilverBulletRaytrace::GetXPosition_TLC_mapped(int eventN)
{
    double XPosition_TLC_mapped = 0.0;
    
    if(eventN<nAlternativeEvents)
    {
        XPosition_TLC_mapped = xPositions_TLC_mapped[eventN];
    }
    
    return XPosition_TLC_mapped;
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::SetXPosition_TLC_mapped(int eventN, double a)
{
    if(eventN<nAlternativeEvents)
    {
        xPositions_TLC_mapped[eventN] = a;
    }
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::SetExcitationEnergy(int eventN, double a)
{
    if(eventN<nAlternativeEvents)
    {
        excitationEnergies[eventN] = a;
    }
}

//--------------------------------------------------------------------------------------
double SilverBulletRaytrace::GetThetaRaytrace(int eventN)
{
    double thetaRaytrace = 0.0;
    
    if(eventN<nAlternativeEvents)
    {
        thetaRaytrace = thetaRaytraces[eventN];
    }
    
    return thetaRaytrace;
}

//--------------------------------------------------------------------------------------
double SilverBulletRaytrace::GetThetaRaytraceCal(int eventN)
{
    double thetaRaytraceCal = 0.0;
    
    if(eventN<nAlternativeEvents)
    {
        thetaRaytraceCal = thetaRaytraceCals[eventN];
    }
    
    return thetaRaytraceCal;
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::SetThetaRaytraceCal(int eventN, double a)
{
    if(eventN<nAlternativeEvents)
    {
        thetaRaytraceCals[eventN] = a;
    }
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::AddEntry(double xPosition, double thetaRaytrace, double rSquared, double adjustedRSquared, int nWiresUsed, int nWiresRemoved, int nWiresTotal)
{
    xPositions.push_back(xPosition);
    xPositions_TLC.push_back(0.0);
    xPositions_TLC_mapped.push_back(0.0);
    excitationEnergies.push_back(0.0);
    thetaRaytraces.push_back(thetaRaytrace);
    thetaRaytraceCals.push_back(thetaRaytrace);
    rSquareds.push_back(rSquared);
    adjustedRSquareds.push_back(adjustedRSquared);
    nUsedWires.push_back(nWiresUsed);
    nRemovedWires.push_back(nWiresRemoved);
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::ArrangeDescendingRSquared()
{
    for(int i=0; i<static_cast<int>(rSquareds.size()); i++)
    {
        for(int j=i+1; j<static_cast<int>(rSquareds.size()); j++)
        {
            if(rSquareds[j]>rSquareds[i])
            {
                double temp_xPosition = xPositions[i];
                double temp_XPosition_TLC = xPositions_TLC[i];
                double temp_XPosition_TLC_mapped = xPositions_TLC_mapped[i];
                double temp_excitationEnergy = excitationEnergies[i];
                double temp_thetaRaytrace = thetaRaytraces[i];
                double temp_thetaRaytraceCal = thetaRaytraceCals[i];
                double temp_rSquared = rSquareds[i];
                double temp_adjustedRSquared = adjustedRSquareds[i];
                double temp_nUsedWires = nUsedWires[i];
                double temp_nRemovedWires = nRemovedWires[i];
                
                //------------------------------------------------
                xPositions[i] = xPositions[j];
                xPositions_TLC[i] = xPositions_TLC[j];
                xPositions_TLC_mapped[i] = xPositions_TLC_mapped[j];
                excitationEnergies[i] = excitationEnergies[j];
                thetaRaytraces[i] = thetaRaytraces[j];
                thetaRaytraceCals[i] = thetaRaytraceCals[j];
                rSquareds[i] = rSquareds[j];
                adjustedRSquareds[i] = adjustedRSquareds[j];
                nUsedWires[i] = nUsedWires[j];
                nRemovedWires[i] = nRemovedWires[j];
                
                //------------------------------------------------
                xPositions[j] = temp_xPosition;
                xPositions_TLC[j] = temp_XPosition_TLC;
                xPositions_TLC_mapped[j] = temp_XPosition_TLC_mapped;
                excitationEnergies[j] = temp_excitationEnergy;
                thetaRaytraces[j] = temp_thetaRaytrace;
                thetaRaytraceCals[j] = temp_thetaRaytraceCal;
                rSquareds[j] = temp_rSquared;
                adjustedRSquareds[j] = temp_adjustedRSquared;
                nUsedWires[j] = temp_nUsedWires;
                nRemovedWires[j] = temp_nRemovedWires;
            }
        }
    }
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::ArrangeDescendingAdjustedRSquared()
{
    for(int i=0; i<static_cast<int>(adjustedRSquareds.size()); i++)
    {
        for(int j=i+1; j<static_cast<int>(adjustedRSquareds.size()); j++)
        {
            if(adjustedRSquareds[j]>adjustedRSquareds[i])
            {
                double temp_xPosition = xPositions[i];
                double temp_XPosition_TLC = xPositions_TLC[i];
                double temp_XPosition_TLC_mapped = xPositions_TLC_mapped[i];
                double temp_excitationEnergy = excitationEnergies[i];
                double temp_thetaRaytrace = thetaRaytraces[i];
                double temp_thetaRaytraceCal = thetaRaytraceCals[i];
                double temp_rSquared = rSquareds[i];
                double temp_adjustedRSquared = adjustedRSquareds[i];
                double temp_nUsedWires = nUsedWires[i];
                double temp_nRemovedWires = nRemovedWires[i];
                
                //------------------------------------------------
                xPositions[i] = xPositions[j];
                xPositions_TLC[i] = xPositions_TLC[j];
                xPositions_TLC_mapped[i] = xPositions_TLC_mapped[j];
                excitationEnergies[i] = excitationEnergies[j];
                thetaRaytraces[i] = thetaRaytraces[j];
                thetaRaytraceCals[i] = thetaRaytraceCals[j];
                rSquareds[i] = rSquareds[j];
                adjustedRSquareds[i] = adjustedRSquareds[j];
                nUsedWires[i] = nUsedWires[j];
                nRemovedWires[i] = nRemovedWires[j];
                
                //------------------------------------------------
                xPositions[j] = temp_xPosition;
                xPositions_TLC[j] = temp_XPosition_TLC;
                xPositions_TLC_mapped[j] = temp_XPosition_TLC_mapped;
                excitationEnergies[j] = temp_excitationEnergy;
                thetaRaytraces[j] = temp_thetaRaytrace;
                thetaRaytraceCals[j] = temp_thetaRaytraceCal;
                rSquareds[j] = temp_rSquared;
                adjustedRSquareds[j] = temp_adjustedRSquared;
                nUsedWires[j] = temp_nUsedWires;
                nRemovedWires[j] = temp_nRemovedWires;
            }
        }
    }
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::ClearEvent()
{
    goodSbrEvent = false;
    goodSbrEventNr = 0;
    sbrXPosition = 0.0;
    sbrXPosition_TLC = 0.0;
    sbrXPosition_TLC_mapped = 0.0;
    sbrExcitationEnergy = 0.0;
    sbrThetaRaytrace = 0.0;
    sbrThetaRaytraceCal = 0.0;
    sbrRsquared = 0.0;
    sbrAdjustedRsquared = 0.0;
    sbrNUsedWires = 0.0;
    sbrNRemovedWires = 0.0;
    
    nAlternativeEvents = 0;
    nTotalWires = 0;
    
    xPositions.clear();
    xPositions_TLC.clear();
    xPositions_TLC_mapped.clear();
    excitationEnergies.clear();
    thetaRaytraces.clear();
    thetaRaytraceCals.clear();
    rSquareds.clear();
    adjustedRSquareds.clear();
    nUsedWires.clear();
    nRemovedWires.clear();
    
    trackVisDriftTimes.clear();
    trackVisDriftTimesRaw.clear();
    trackVisWireNumbers.clear();
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::DetermineGoodSbrEvent()
{
    bool    foundValidSbrEvent = false;
    
    bool    forceMinimumWires = true;
    int     nForcedMinimumWires = 3;
    
    bool    foundPreferredEvent = false;
    int     nPreferredMinimumWires = 4;
    double  adjustedRSquaredThreshold = 0.998;
    
    double  optimalEventAdjustedRSquared = 0.0;
    int     optimalEventIndex_SBR = 0;
    
    int eventN = 0;
    while(eventN<nAlternativeEvents && !foundPreferredEvent)
    {
        if(adjustedRSquareds[eventN]>optimalEventAdjustedRSquared && ((forceMinimumWires && nUsedWires[eventN]>=nForcedMinimumWires) || !forceMinimumWires))
        {
            foundValidSbrEvent = true;
            
            optimalEventAdjustedRSquared = adjustedRSquareds[eventN];
            optimalEventIndex_SBR = eventN;
            
            if(optimalEventAdjustedRSquared>=adjustedRSquaredThreshold && nUsedWires[eventN]>=nPreferredMinimumWires)
            {
                foundPreferredEvent = true;
            }
        }
        
        eventN++;
    }
    
    /*
     for(int j=0; j<nAlternativeEvents; j++)
     {
     if(!foundPreferredEvent && adjustedRSquareds[j]>optimalEventAdjustedRSquared && ((forceMinimumWires && nUsedWires[j]>=nForcedMinimumWires) || !forceMinimumWires))
     {
     foundValidSbrEvent = true;
     
     optimalEventAdjustedRSquared = adjustedRSquareds[j];
     optimalEventIndex_SBR = j;
     
     if(optimalEventAdjustedRSquared>=adjustedRSquaredThreshold && nUsedWires[j]>=nPreferredMinimumWires)
     {
     foundPreferredEvent = true;
     }
     }
     }
     */
    
    //------------------------------------------------
    if(foundValidSbrEvent)
    {
        if(adjustedRSquareds[optimalEventIndex_SBR]>0.998 && nRemovedWires[optimalEventIndex_SBR]<4 && nUsedWires[optimalEventIndex_SBR]>=3 && nUsedWires[optimalEventIndex_SBR]<=9)
        {
            goodSbrEvent = true;
            goodSbrEventNr = optimalEventIndex_SBR;
            
            sbrXPosition = xPositions[goodSbrEventNr];
            sbrXPosition_TLC = xPositions_TLC[goodSbrEventNr];
            sbrXPosition_TLC_mapped = xPositions_TLC_mapped[goodSbrEventNr];
            sbrExcitationEnergy = excitationEnergies[goodSbrEventNr];
            
            sbrThetaRaytrace = thetaRaytraces[goodSbrEventNr];
            sbrThetaRaytraceCal = thetaRaytraceCals[goodSbrEventNr];
            
            sbrRsquared = rSquareds[goodSbrEventNr];
            sbrAdjustedRsquared = adjustedRSquareds[goodSbrEventNr];
            sbrNUsedWires = nUsedWires[goodSbrEventNr];
            sbrNRemovedWires = nRemovedWires[goodSbrEventNr];
        }
    }
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
void SilverBulletRaytrace::DetermineTrackVisWireNumbers(std::vector<double> driftDistances, std::vector<int> wires, std::vector<int> wireAccount)
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
        trackVisWireNumbers.push_back(wires[i]-wires[smallestDriftDistanceIndex]);
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
        double  xPosition, thetaRaytrace, rSquared, adjustedRSquared;
        
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
        LeastSquaresFit(driftDistances_inversionTest, wires, wireAccount, xPosition, thetaRaytrace, rSquared, adjustedRSquared);
        
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
    
    nTotalWires = wire_num;
    
    for(int i=0; i<wire_num; i++)
    {
        //driftDistances.push_back(dd[i]);
        //wires.push_back(wire[i]);
        
        //------------------------------------------------
        double driftDistanceThreshold = 1.0;
        
        if(dataLabel_CombN==1)
        {
            driftDistanceThreshold = 1.47;
        }
        else if(dataLabel_CombN==2)
        {
            driftDistanceThreshold = 1.47; // back to 1.47 if 1.48 isn't good
        }
        else if(dataLabel_CombN==3)
        {
            driftDistanceThreshold = 1.47; // back to 1.47 if 1.48 isn't good
        }
        else if(dataLabel_CombN==4)
        {
            driftDistanceThreshold = 1.0;
        }
        
        //------------------------------------------------
        if(dd[i]>=driftDistanceThreshold)
        {
            driftDistances.push_back(dd[i]);
            wires.push_back(wire[i]);
        }
    }
    
    nTotalWires = static_cast<int>(driftDistances.size());
    
    driftDistances_corrected = driftDistances;
    
    //--------------------------------------------------------
    int     minimumWires = 3;
    double  rSquaredThreshold = 0.998;
    //double  adjustedRSquaredThreshold = 0.998;
    double  adjustedRSquaredThreshold = 1.0;
    
    int     nRemovedWires = 0;
    double  rSquared_bestOverall = 0.0;
    double  adjustedRSquared_bestOverall = 0.0;
    
    std::vector<int> finalWireAccount;
    
    //while(nRemovedWires<=(wire_num-minimumWires) && rSquared_bestOverall<rSquaredThreshold)
    while(nRemovedWires<=(wire_num-minimumWires) && adjustedRSquared_bestOverall<adjustedRSquaredThreshold)
    {
        double  xPosition_bestPerWireRemoval = 0.0, thetaRaytrace_bestPerWireRemoval = 0.0, rSquared_bestPerWireRemoval = 0.0, adjustedRSquared_bestPerWireRemoval = 0.0;
        
        //------------------------------------------------------------------------------------
        //      Determine which wire is optimal (w.r.t. to the rSquared metric) to remove
        int nUsedWires = wire_num - nRemovedWires;
        std::vector<std::vector<int>> wireAccount_perNChooseR = permutationLibrary[wire_num-1][nUsedWires-1];
        
        //------------------------------------------------------------------------------------
        //      This is to always manually remove the wire with the minimum drift distance
        /*
         if(nRemovedWires==0)
         {
         nRemovedWires++;
         }
         
         for(int i=0; i<static_cast<int>(wireAccount_perNChooseR.size()); i++)
         {
         bool firstEntry = false;
         double smallestWireDistance = 0.0;
         int smallestUsedWireIndex = 0;
         
         for(int j=0; j<static_cast<int>(wireAccount_perNChooseR[i].size()); j++)
         {
         if(wireAccount_perNChooseR[i][j]==1)
         {
         if(TMath::Abs(driftDistances_corrected[j])<smallestWireDistance || !firstEntry)
         {
         firstEntry = true;
         smallestUsedWireIndex = j;
         smallestWireDistance = TMath::Abs(driftDistances_corrected[j]);
         }
         }
         }
         
         if(smallestUsedWireIndex>static_cast<int>(wireAccount_perNChooseR[i].size()-1))
         {
         std::cout << "PROBLEM" << std::endl;
         }
         
         wireAccount_perNChooseR[i][smallestUsedWireIndex] = 0;
         }
         */
        
        //--------------------------------------------------------------------------------------------------------
        //      Looping over the various permutations of total wires (wire_num) and used wires (nUsedWires)
        int permutationIndex_best = 0;
        
        for(int i=0; i<static_cast<int>(wireAccount_perNChooseR.size()); i++)
        {
            double  xPosition, thetaRaytrace, rSquared, adjustedRSquared;
            
            //----------------------------------------------------------------------------------------------------
            //      For each permuation, find the optimal wire after which the drift distances are inverted
            OptomiseDriftDistanceInversion(driftDistances, driftDistances_corrected, wires, wireAccount_perNChooseR[i]);
            LeastSquaresFit(driftDistances_corrected, wires, wireAccount_perNChooseR[i], xPosition, thetaRaytrace, rSquared, adjustedRSquared);
            
            //------------------------------------------------
            if(i==0 || rSquared>rSquared_bestPerWireRemoval)
            {
                permutationIndex_best = i;
                xPosition_bestPerWireRemoval = xPosition;
                thetaRaytrace_bestPerWireRemoval = thetaRaytrace;
                rSquared_bestPerWireRemoval = rSquared;
                adjustedRSquared_bestPerWireRemoval = adjustedRSquared;
            }
        }
        
        rSquared_bestOverall = rSquared_bestPerWireRemoval;
        adjustedRSquared_bestOverall = adjustedRSquared_bestPerWireRemoval;
        
        //------------------------------------------------------------------------------------
        OptomiseDriftDistanceInversion(driftDistances, driftDistances_corrected, wires, wireAccount_perNChooseR[permutationIndex_best]);
        
        AddEntry(xPosition_bestPerWireRemoval, thetaRaytrace_bestPerWireRemoval, rSquared_bestPerWireRemoval, adjustedRSquared_bestPerWireRemoval, (wire_num-nRemovedWires), nRemovedWires, wire_num);
        AddTrackVisDriftTimes(driftDistances_corrected, wireAccount_perNChooseR[permutationIndex_best]);
        AddTrackVisDriftTimesRaw(driftDistances, wireAccount_perNChooseR[permutationIndex_best]);
        finalWireAccount = wireAccount_perNChooseR[permutationIndex_best];
        
        //------------------------------------------------------------------------------------
        nRemovedWires++;
    }
    
    if(wire_num>=minimumWires)
    {
        DetermineTrackVisWireNumbers(driftDistances, wires, finalWireAccount);
    }
    
    //------------------------------------------------------------------------------------
    //ArrangeDescendingRSquared();
    //ArrangeDescendingAdjustedRSquared();
    
    nAlternativeEvents = static_cast<int>(xPositions.size());
}

//--------------------------------------------------------------------------------------
void SilverBulletRaytrace::LeastSquaresFit(std::vector<double> driftDistances, std::vector<int> wires, std::vector<int> wireAccount, double &xPosition, double &thetaRaytrace, double &rSquared, double &adjustedRSquared)
{
    const double X_WIRESPACE = 4.0;     // distance (mm) between signal wires
    
    //--------------------------------------------------------------------
    //      Now do the linear regression
    double x_av = 0.0, y_av = 0.0, xy_av = 0.0, x2_av = 0.0, y2_av;
    double nWiresUsed = 0.0;
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        if(wireAccount[i]==1)
        {
            double x = X_WIRESPACE*(float(wires[i]));
            double y = driftDistances[i];
            
            nWiresUsed += 1.0;
            
            x_av += x;
            y_av += y;
            xy_av += x*y;
            x2_av += x*x;
            y2_av += y*y;
        }
    }
    
    x_av *= (1.0/nWiresUsed);
    y_av *= (1.0/nWiresUsed);
    xy_av *= (1.0/nWiresUsed);
    x2_av *= (1.0/nWiresUsed);
    y2_av *= (1.0/nWiresUsed);
    
    double a = 0.0, b = 0.0;
    double a_num = 0.0, a_den = 0.0;
    double b_num = 0.0, b_den = 0.0;
    
    a_num = (y_av*x2_av) - (x_av*xy_av);
    a_den = x2_av - (x_av*x_av);
    a = a_num/a_den;
    
    b_num = xy_av - (x_av*y_av);
    b_den = x2_av - (x_av*x_av);
    b = b_num/b_den;
    
    //--------------------------------------------------------------------
    xPosition = (-a)/b;
    thetaRaytrace = -1.0*57.295779513*atan(b);
    
    //--------------------------------------------------------------------
    double ss_xx = 0.0, ss_yy = 0.0, ss_xy = 0.0;
    
    for(int i=0; i<static_cast<int>(driftDistances.size()); i++)
    {
        if(wireAccount[i]==1)
        {
            double x = X_WIRESPACE*(float(wires[i]));
            double y = driftDistances[i];
            
            ss_xx += (x - x_av)*(x - x_av);
            ss_yy += (y - y_av)*(y - y_av);
            ss_xy += (x - x_av)*(y - y_av);
        }
    }
    
    rSquared = (ss_xy*ss_xy)/(ss_xx*ss_yy);
    adjustedRSquared = 1.0 - (((1 - rSquared)*(nWiresUsed - 1))/(nWiresUsed - 1 - 1));
}
