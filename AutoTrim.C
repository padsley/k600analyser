#include <stdio.h>
#include <fstream>

#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TFile.h>
#include <TF1.h>
#include <TCanvas.h>
#include "TError.h"

// clang++ AutoTrim.C -o AutoTrim `root-config --cflags --libs` -lgsl -lgslcblas

int main()
{
    //------------------------------------------------------------
    char filePath[1028];
    char name[128];
    char condition[256];
    char command[256];
    
    //------------------------------------------------------------
    Int_t t0corr=8000;		// Corrected bin number for cable length shift
    const Int_t nbins=14999;		// Bin size of TDCchan histos
    const Int_t histImax= 650;		// 420 for VDC 1 alone; set to 896 for both VDC 1 and 2
    const Int_t histImin = 0;
    int edgeDeterminationStrategy;
    int rebinFactor;

    std::vector<std::shared_ptr<TH1F>> hTDC;
    std::vector<std::shared_ptr<TH1F>> hTDC_bkgSub;
    
    ///////////////////////////////////////////////////////////////
    ////                Chaining the ROOT Trees
    ///////////////////////////////////////////////////////////////

    //----------------------------------------------------------------------------
    //std::vector<int> runNumbers = {54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 69, 70, 71, 72, 73, 74, 75, 76};
    
    //      INCLUSIVE (WE 1, 2, 3 and 4)
    std::vector<int> runNumbers{54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 69, 70, 71, 72, 73, 74, 75, 76, 83, 84, 85, 86, 87, 88, 89, 90, 92, 116, 117, 118, 119, 122, 125, 127, 129, 131, 133, 134, 135, 136, 138, 140, 141, 142, 144, 145, 146, 147, 148, 149, 150, 158, 159, 160, 161, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 176, 177, 178, 179, 180, 181, 183, 184, 186, 188, 190, 230, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 248, 255, 256, 257, 258, 259, 263, 264, 265, 266, 267, 268, 269, 270, 275, 276, 277, 278, 279, 280, 281, 282, 283, 285, 286, 287, 288, 291, 294, 295, 296, 297, 298, 299, 301, 302, 303, 304, 305, 306, 307, 308, 456, 457, 458, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487, 489, 490, 491, 492, 493, 494, 495, 496, 498, 499, 500, 511, 512, 514, 515, 516, 520};

    //std::vector<int> runNumbers = {54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 69, 70, 71, 72, 73, 74, 75, 76, 83, 84, 85, 86, 87, 88, 89, 90, 92, 116, 117, 118, 119, 122, 125, 127, 129, 131, 133, 134, 135, 136, 138, 140, 141, 142, 144, 145, 146, 147, 148, 149, 150, 158, 159, 160, 161, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 176, 177, 178, 179, 180, 181, 183, 184, 186, 188, 190};

    
    //------------------------------------------------------------------------------------------------------------------
    //      edgeDeterminationStrategy==0: Fit to find the lowest gradient and extrapolate to find x-axis intercept
    //      edgeDeterminationStrategy==1: Use the maximum bin value (maxBin) as a metric to then find the approximate intercept points of the TDC spectra. Suitable for low statistics
    edgeDeterminationStrategy = 0;
    
    //------------------------------------------------------------------------------------------------
    //      rebinFactor == N; N bins shall be regrouped into each bin in the rebinned histogram
    //      This aids poors statistics
    rebinFactor = 8;
    
    //----------------------------------------------------------------------------
    std::string directoryPath = "../sortedAutoTrim";
    
    //----------------------------------------------------------------------------
    int nRunsAnalysed = (int) runNumbers.size();
    
    if(nRunsAnalysed>(int) runNumbers.size()) {nRunsAnalysed = (int) runNumbers.size();}
    
    std::ofstream outputFile;
    outputFile.open("CableLengthtmp.dat");
    TFile *arrayoutf = new TFile("AutoTrim.root", "RECREATE");
    
    //----------------------------------------------------------------------------
    double slopeLength = 40.0;
    int integralThreshold = 500;

    //----------------------------------------------------------------------------
    std::cout << std::endl;
    std::cout << "Extracting Histograms" << std::endl;

    //----------------------------------------------------------------------------
    for(int histI=histImin; histI<=histImax; histI++)
    {
        std::cout << "\rChannel (" << histI << "/" << histImax << ")" << std::flush;

        sprintf(name,"hTDC_%d",histI);
        hTDC.push_back(std::make_shared<TH1F>(name,"Added TDC channel",nbins,0,nbins));
        
        sprintf(name,"hTDC_bkgSub_%d",histI);
        hTDC_bkgSub.push_back(std::make_shared<TH1F>(name,"Added TDC channel",nbins,0,nbins));
        
        //--------------------------------------------------------------------------------
        // Loop over each of the run files...
        //--------------------------------------------------------------------------------
        
        for(int i=0; i<nRunsAnalysed; i++)
        {
            if(directoryPath.find_last_of("/") != (directoryPath.length()-1))
            {
                sprintf(filePath, "%s/sorted", directoryPath.c_str());
            }
            else
            {
                sprintf(filePath, "%ssorted", directoryPath.c_str());
            }
            
            int nMissingZeros = 0;
            if(runNumbers[i]<10000) {nMissingZeros++;}
            if(runNumbers[i]<1000) {nMissingZeros++;}
            if(runNumbers[i]<100) {nMissingZeros++;}
            if(runNumbers[i]<10) {nMissingZeros++;}
            
            for(int j=0; j<nMissingZeros; j++)
            {
                sprintf(filePath, "%s0", filePath);
            }
            
            if(runNumbers[i]<10) {sprintf(filePath, "%s%1d.root", filePath, runNumbers[i]);}
            else if(runNumbers[i]<100) {sprintf(filePath, "%s%2d.root", filePath, runNumbers[i]);}
            else if(runNumbers[i]<1000) {sprintf(filePath, "%s%3d.root", filePath, runNumbers[i]);}
            else if(runNumbers[i]<10000) {sprintf(filePath, "%s%4d.root", filePath, runNumbers[i]);}
            
            
            //--------------------------------------------------------
            std::shared_ptr<TFile> runfile = std::shared_ptr<TFile>(TFile::Open(filePath));
            //TFile *runfile = TFile::Open(filePath);
            //TFile runfile(filePath);
            
            char name[128];
            sprintf(name,"TDCchan_%d",histI);

            TH1F *hTemp = 0;
            runfile->GetObject(name, hTemp);
            
            hTDC[histI]->Add(hTemp, 1.0);
            hTDC_bkgSub[histI]->Add(hTemp, 1.0);
            
            runfile->Close();
        }
    }

    gErrorIgnoreLevel = kError; // Only error message
    
    std::cout << std::endl;

    if(edgeDeterminationStrategy==0)
    {
        std::cout << "Fitting Edges" << std::endl;
        
        //----------------------------------------------------
        for(int histI=histImin; histI<=histImax; histI++)
        {
            std::cout << "\rChannel (" << histI << "/" << histImax << ")" << std::flush;
            
            //------------------------------------------------
            hTDC[histI]->Rebin(rebinFactor);
            hTDC_bkgSub[histI]->Rebin(rebinFactor);
            
            //------------------------------------------------
            double noiseThreshold = 0.0;
            std::shared_ptr<TF1> fNoise = std::make_shared<TF1>("fNoise", "pol0", 0.0, 15000.0);
            
            //------------------------------------------------
            int maximumBin = hTDC[histI]->GetMaximumBin();
            int maximumBinValue = hTDC[histI]->GetBinContent(maximumBin);
            double maxBinLowEdge = hTDC[histI]->GetBinLowEdge(maximumBin);
            
            //------------------------------------------------
            //      Fit low side
            hTDC[histI]->Fit("fNoise", "Q+", "", maxBinLowEdge-3000.0, maxBinLowEdge-1500.0);
            double noiseThresholdLow = fNoise->GetParameter(0);
            
            //------------------------------------------------
            //      Fit high side
            hTDC[histI]->Fit("fNoise", "Q+", "", maxBinLowEdge+1500.0, maxBinLowEdge+3000.0);
            double noiseThresholdHigh = fNoise->GetParameter(0);
            
            //------------------------------------------------
            //      Selecting the lower noise
            if(noiseThresholdLow>noiseThresholdHigh)
            {
                noiseThreshold = noiseThresholdLow;
            }
            else
            {
                noiseThreshold = noiseThresholdHigh;
            }
            
            //------------------------------------------------
            //      Removing the noise
            for(int i=0; i<hTDC[histI]->GetNbinsX(); i++)
            {
                double binContent = hTDC_bkgSub[histI]->GetBinContent(i+1);
                double newBinContent = binContent - noiseThreshold;
                
                if(newBinContent<0.0)
                {
                    newBinContent = 0.0;
                }
                
                hTDC_bkgSub[histI]->SetBinContent(i+1, newBinContent);
            }
            
            //------------------------------------------------
            int integral = hTDC_bkgSub[histI]->Integral();
            
            //------------------------------------------------
            if(histI<140)
            {
                hTDC[histI]->GetXaxis()->SetRangeUser(7000.0, 9000.0);
            }
            else
            {
                hTDC[histI]->GetXaxis()->SetRangeUser(6500.0, 9000.0);
            }

            //------------------------------------------------
            std::shared_ptr<TF1> fSlope = std::make_shared<TF1>("fSlope", "pol1", 0.0, 15000.0);
            std::vector<double> lowestSlope_PolPars = {0.0, 0.0};
            
            for(double xValue=maxBinLowEdge; xValue<15000.0-slopeLength; xValue+=10.0)
            {
                fSlope->SetParameter(0, 0.0);
                fSlope->SetParameter(1, 0.0);

                double countMetric = hTDC[histI]->Integral((int) (xValue/rebinFactor), (int) ((xValue+slopeLength)/rebinFactor));
                
                //--------------------------------------------
                if(integral>integralThreshold && countMetric>2.0*maximumBinValue && ((histI<140 && xValue>7700.0) || (histI>=140 && xValue>7000.0)))
                {
                    hTDC_bkgSub[histI]->Fit("fSlope", "Q+", "", xValue, xValue+slopeLength);
                    
                    double offset = fSlope->GetParameter(0);
                    double slope = fSlope->GetParameter(1);
                    
                    if(slope<lowestSlope_PolPars[1])
                    {
                        lowestSlope_PolPars[0] = offset;
                        lowestSlope_PolPars[1] = slope;
                    }
                }
            }
            
            //------------------------------------------------
            int xIntercept = 0;
            int offset = 0;
            
            if(lowestSlope_PolPars[1]!=0.0)
            {
                xIntercept = (int) -lowestSlope_PolPars[0]/lowestSlope_PolPars[1];
            }
            
            //      Minus is for the convention of the k600analyser: offsetTime = originalTime - offset;
            offset = -(t0corr - xIntercept);
            
            //------------------------------------------------
            if(offset<-5000 || offset>5000)
            {
                offset = 0;
            }
            
            if(integral<integralThreshold)
            {
                offset = 0;
            }
            
            double stdDev = hTDC_bkgSub[histI]->GetStdDev();
            
            if(stdDev<400 || stdDev>2000)
            {
                offset = 0;
            }
            
            outputFile << offset << std::endl;
            
            //------------------------------------------------
            arrayoutf->cd();
            hTDC[histI]->Write();
            hTDC_bkgSub[histI]->Write();
        }
        
        gErrorIgnoreLevel = kWarning; // error and warning message
    }
    
    //----------------------------------------------------
    if(edgeDeterminationStrategy==1)
    {
        std::cout << "Finding approximate intercepts" << std::endl;
        
        //----------------------------------------------------
        for(int histI=histImin; histI<=histImax; histI++)
        {
            std::cout << "\rChannel (" << histI << "/" << histImax << ")" << std::flush;
            
            //------------------------------------------------
            hTDC[histI]->Rebin(rebinFactor);
            hTDC_bkgSub[histI]->Rebin(rebinFactor);
            
            //------------------------------------------------
            double noiseThreshold = 0.0;
            std::shared_ptr<TF1> fNoise = std::make_shared<TF1>("fNoise", "pol0", 0.0, 15000.0);
            
            //------------------------------------------------
            int maximumBin = hTDC[histI]->GetMaximumBin();
            int maximumBinValue = hTDC[histI]->GetBinContent(maximumBin);
            double maxBinLowEdge = hTDC[histI]->GetBinLowEdge(maximumBin);
            
            //------------------------------------------------
            //      Fit low side
            hTDC[histI]->Fit("fNoise", "Q+", "", maxBinLowEdge-3000.0, maxBinLowEdge-1500.0);
            double noiseThresholdLow = fNoise->GetParameter(0);
            
            //------------------------------------------------
            //      Fit high side
            hTDC[histI]->Fit("fNoise", "Q+", "", maxBinLowEdge+1500.0, maxBinLowEdge+3000.0);
            double noiseThresholdHigh = fNoise->GetParameter(0);
            
            //------------------------------------------------
            //      Selecting the lower noise
            if(noiseThresholdLow>noiseThresholdHigh)
            {
                noiseThreshold = noiseThresholdLow;
            }
            else
            {
                noiseThreshold = noiseThresholdHigh;
            }
            
            //------------------------------------------------
            //      Removing the noise
            for(int i=0; i<hTDC[histI]->GetNbinsX(); i++)
            {
                double binContent = hTDC_bkgSub[histI]->GetBinContent(i+1);
                double newBinContent = binContent - noiseThreshold;
                
                if(newBinContent<0.0)
                {
                    newBinContent = 0.0;
                }
                
                hTDC_bkgSub[histI]->SetBinContent(i+1, newBinContent);
            }
            
            //------------------------------------------------
            bool foundIntercept = false;
            
            int xIntercept = 0.0;
            double targetY = 0.4*maximumBinValue;
            
            for(int i=0; i<hTDC[histI]->GetNbinsX()-1; i++)
            {
                double y0 = hTDC_bkgSub[histI]->GetBinContent(i+1);
                double y1 = hTDC_bkgSub[histI]->GetBinContent(i+2);
                
                double x0 = hTDC_bkgSub[histI]->GetBinCenter(i+1);
                double x1 = hTDC_bkgSub[histI]->GetBinCenter(i+2);

                if((int) y0 == (int) targetY)
                {
                    xIntercept = (int) x0;
                }
                else if((int) y1 == (int) targetY)
                {
                    xIntercept = (int)x1;
                }
                else if(y0<=targetY && targetY<=y1)
                {
                    double m, c;
                    
                    m = (y1-y0)/(x1-x0);
                    c = y0 - (m*x0);
                    
                    xIntercept = (int) (targetY-c)/m;
                }
            }

            //------------------------------------------------
            int integral = hTDC_bkgSub[histI]->Integral();
            
            //------------------------------------------------
            //      Minus is for the convention of the k600analyser: offsetTime = originalTime - offset;
            int offset = -(t0corr - xIntercept);
            
            if(offset<-5000 || offset>5000)
            {
                offset = 0;
            }
            
            if(integral<integralThreshold)
            {
                offset = 0;
            }
            
            double stdDev = hTDC_bkgSub[histI]->GetStdDev();
            
            if(stdDev<400 || stdDev>2000)
            {
                offset = 0;
            }
            
            outputFile << offset << std::endl;
            
            
            //------------------------------------------------
            arrayoutf->cd();
            hTDC[histI]->Write();
            hTDC_bkgSub[histI]->Write();
        }
        
        gErrorIgnoreLevel = kWarning; // error and warning message
    }
    
    //----------------------------------------------------
    outputFile.close();
    
    return 0;
}

