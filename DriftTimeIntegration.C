#include <stdio.h>
#include <fstream>

#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TFile.h>
#include <TF1.h>
#include <TCanvas.h>

void DriftTimeIntegration()
{
    bool useX1 = true;
    bool useU1 = false;
    bool useX2 = true;
    bool useU2 = false;
    
    //----------------------------------------------------------------------------
    std::string directoryPath = "../sortedNew";
    
    //------------------------------------------------------------------------------------------------------------------------
    std::vector<int> runNumbers{54,55,56,57,58,59,60,61,62,63,64,65,66,67,69,70,71,72,73,74,75,76,83,84,85,86,87,88,89,90,92,116,117,118,119,122,125,127,129,131,133,134,135,136,138,140,141,142,144,145,146,147,148,149,150,158,159,160,161,163,164,165,166,167,168,169,170,171,172,173,174,176,177,178,179,180,181,183,184,186,188,190,230,233,234,235,236,237,238,239,240,241,242,243,244,245,246,248,255,256,257,258,259,263,264,265,266,267,268,269,270,275,276,277,278,279,280,281,282,283,285,286,287,288,291,294,295,296,297,298,299,301,302,303,304,305,306,307,308,456,457,458,460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,481,482,483,484,485,486,487,489,490,491,492,493,494,495,496,498,499,500,511,512,514,515,516,520};

    //------------------------------------------------
    TFile *outputFile = new TFile("DriftTimeIntegration.root", "RECREATE");
    
    ofstream outx1;
    if(useX1) {outx1.open("lutx1file");}
    
    ofstream outu1;
    if(useU1) {outu1.open("lutu1file");}
    
    ofstream outx2;
    if(useX2) {outx2.open("lutx2file");}

    ofstream outu2;
    if(useU2) {outu2.open("lutu2file");}
    
    //------------------------------------------------
    TH1F *hDriftTimes_X1 = new TH1F("hDriftTimes_X1","",14999,0.,14999.);
    TH1F *hDriftTimes_U1 = new TH1F("hDriftTimes_U1","",14999,0.,14999.);
    TH1F *hDriftTimes_X2 = new TH1F("hDriftTimes_X2","",14999,0.,14999.);
    TH1F *hDriftTimes_U2 = new TH1F("hDriftTimes_U2","",14999,0.,14999.);
    
    TH1F *hLUT_X1 = new TH1F("hLUT_X1","",14999,0.,14999.);
    TH1F *hLUT_U1 = new TH1F("hLUT_U1","",14999,0.,14999.);
    TH1F *hLUT_X2 = new TH1F("hLUT_X2","",14999,0.,14999.);
    TH1F *hLUT_U2 = new TH1F("hLUT_U2","",14999,0.,14999.);

    //------------------------------------------------
    char filePath[1028];

    for(int i=0; i<(int) runNumbers.size(); i++)
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
        else if(runNumbers[i]<100000) {sprintf(filePath, "%s%5d.root", filePath, runNumbers[i]);}
        
        //--------------------------------------------------------
        std::shared_ptr<TFile> runfile = std::shared_ptr<TFile>(TFile::Open(filePath));

        //--------------------------------------------------------
        TH1F *hTempX1 = 0;
        runfile->GetObject("hX1_DriftTimeGood", hTempX1);
        hDriftTimes_X1->Add(hTempX1, 1.0);
        
        //--------------------------------------------------------
        TH1F *hTempU1 = 0;
        runfile->GetObject("hU1_DriftTimeGood", hTempU1);
        hDriftTimes_U1->Add(hTempU1, 1.0);

        //--------------------------------------------------------
        TH1F *hTempX2 = 0;
        runfile->GetObject("hX2_DriftTimeGood", hTempX2);
        hDriftTimes_X2->Add(hTempX2, 1.0);

        //--------------------------------------------------------
        TH1F *hTempU2 = 0;
        runfile->GetObject("hU2_DriftTimeGood", hTempU2);
        hDriftTimes_U2->Add(hTempU2, 1.0);
        
        runfile->Close();
    }
    
    //------------------------------------------------
    for(int i=0; i<hDriftTimes_X1->GetNbinsX(); i++)
    {
        if(i<6000 || i>8000)
        {
            hDriftTimes_X1->SetBinContent(i, 0.0);
        }
    }
    
    for(int i=0; i<hDriftTimes_U1->GetNbinsX(); i++)
    {
        if(i<6000 || i>8000)
        {
            hDriftTimes_U1->SetBinContent(i, 0.0);
        }
    }
    
    for(int i=0; i<hDriftTimes_X2->GetNbinsX(); i++)
    {
        if(i<6000 || i>8000)
        {
            hDriftTimes_X2->SetBinContent(i, 0.0);
        }
    }
    
    for(int i=0; i<hDriftTimes_U2->GetNbinsX(); i++)
    {
        if(i<6000 || i>8000)
        {
            hDriftTimes_U2->SetBinContent(i, 0.0);
        }
    }
    
    
    //------------------------------------------------
    if(useX1)
    {
        int nbins_X1 = hDriftTimes_X1->GetNbinsX();
        for(int i=0;i<nbins_X1;i++)
        {
            float lut = 1.0-(hDriftTimes_X1->Integral(0,i)/hDriftTimes_X1->Integral(0,nbins_X1));
            outx1 << lut  << endl;
            
            hLUT_X1->SetBinContent(i+1, lut);
        }
        
        outx1.close();
    }
    
    //------------------------------------------------
    if(useU1)
    {
        int nbins_U1 = hDriftTimes_U1->GetNbinsX();
        for(int i=0;i<nbins_U1;i++)
        {
            float lut = 1.0-(hDriftTimes_U1->Integral(0,i)/hDriftTimes_U1->Integral(0,nbins_U1));
            outu1 << lut  << endl;
            
            hLUT_U1->SetBinContent(i+1, lut);
        }
        
        outu1.close();
    }
    
    //------------------------------------------------
    if(useX2)
    {
        int nbins_X2 = hDriftTimes_X2->GetNbinsX();
        for(int i=0;i<nbins_X2;i++)
        {
            float lut = 1.0-(hDriftTimes_X2->Integral(0,i)/hDriftTimes_X2->Integral(0,nbins_X2));
            outx2 << lut  << endl;
            
            hLUT_X2->SetBinContent(i+1, lut);
        }
        
        outx2.close();
    }
    
    //------------------------------------------------
    if(useU2)
    {
        int nbins_U2 = hDriftTimes_X1->GetNbinsX();
        for(int i=0;i<nbins_U2;i++)
        {
            float lut = 1.0-(hDriftTimes_U2->Integral(0,i)/hDriftTimes_U2->Integral(0,nbins_U2));
            outu2 << lut  << endl;
            
            hLUT_U2->SetBinContent(i+1, lut);
        }
        
        outu2.close();
    }
    
    //------------------------------------------------
    outputFile->cd();
    hDriftTimes_X1->Write();
    hDriftTimes_U1->Write();
    hDriftTimes_X2->Write();
    hDriftTimes_U2->Write();
    
    hLUT_X1->Write();
    hLUT_U1->Write();
    hLUT_X2->Write();
    hLUT_U2->Write();
    
    //------------------------------------------------
    outputFile->Close();
    delete outputFile;
}
