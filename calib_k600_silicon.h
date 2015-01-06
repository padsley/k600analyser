#include <TF1.h>
#include <TFile.h>
#include <TROOT.h>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <string>
#include <iostream>
#include <TCutG.h>
#include <fstream>
#include <TSpectrum.h>
#include <algorithm>
#include <TF1.h>
#include <TGraph.h>
#include <TSystem.h>

double oPsides[64], gPsides[64];
double oNsides[64], gNsides[64];
double oADC[128];
double gADC[128];


using std::cout;
using std::endl;
using std::sort;

TFile *ffile;

//double alpha3_E[3]={5156.59,5485.56,5804.77};

// double alpha_E[7] = {5340.36, 5423.15, 5685.37, 6050.78, 6288.08, 6778.3, 8784.86};

double alpha_E[5] = {5423.15, 5685.37, 6288.08, 6778.3, 8784.86};

//double alpha_E[4] = {4784.34,5489.48,6002.35,7686.82};

void mOutput()
{
  ofstream output;
  output.open("calib_pars.h");

  output << "double oADC[128] = {";
  for(int i=0;i<128;i++)
  {
    output << oADC[i];
    if(i<127)output << ",";
  }  
  output << "};\n\n";

  output << "double gADC[128] = {";
  for(int i=0;i<128;i++)
  {
    output << gADC[i];
    if(i<127)output << ",";
  }  
  output << "};\n\n";

/*

  output << "double oPsides[64] = {";  
  for(int i=0;i<64;i++)
  {
    output << oPsides[i];
    if(i<63)output << ",";
  }
  output << "};\n\n";
  

  output << "double gPsides[64] = {";
  for(int i=0;i<64;i++)
  {
    output << gPsides[i];
    if(i<63)output << ",";
  }
  output << "};\n\n";
  

  output << "double oNsides[64] = {";
  for(int i=0;i<64;i++)
  {
    output << oNsides[i];
    if(i<63)output << ",";
  }
  output << "};\n\n";

  
  output << "double gNsides[64] = {";
  for(int i=0;i<64;i++)
  {
    output << gNsides[i];
    if(i<63)output << ",";
  }
  output << "};\n\n";
*/

}
