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
    bool useU1 = true;
    bool useX2 = true;
    bool useU2 = true;
    
    //----------------------------------------------------------------------------
    std::string directoryPath = "../sorted";
    
    //------------------------------------------------------------------------------------------------------------------------
    //std::vector<int> runNumbers{22077,22078,22080,22082,22083,22088,22089,22090,22091,22092,22093,22094};

    std::vector<int> runNumbers{22279,22283,22284,22286,22278,22285,22289,22290,22295,22296,22297,22298,22299,22300,22301,22304,22305,22367,22364,22365,22368,22369,22370,22371,22372,22373,22374,22375,22376,22377,22378,22379,23115,23116,23117,23118,23119,23124,23125,23128,23129,23153,23156,23157,23159,23163,23164,23165,23166,23168,23171,23173,23174,23176,23180,23181,23184,23186,23187,23189,23191,23161,23162,23190,23265,23266,23267,23268,23271,23276,23280,23281,23282,23283,23284,23285,23287,23288,23294,23296,23297,23298,23299,23300,23301,23272,23273,23293};

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
        if(i<6155 || i>8000)
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
