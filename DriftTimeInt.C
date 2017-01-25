//#include <iostream>
// use e.g.                root[] h1=hU1_DriftTimeGood
{
  ofstream outx1;
  outx1.open("lutx1file");
  ofstream outu1;
  outu1.open("lutu1file");
  ofstream outx2;
  outx2.open("lutx2file");
  ofstream outu2;
  outu2.open("lutu2file");

  Int_t run_no[]={1084};  
  Int_t nrofruns=sizeof(run_no)/sizeof(run_no[0]);  
  Int_t histI;
  Char_t namerun[128],addtdc[128];

  TH1F *hX1 = new TH1F("hX1","drifttime",14999,0.,14999.);
  TH1F *hU1 = new TH1F("hU1","drifttime",14999,0.,14999.);
  TH1F *hX2 = new TH1F("hX2","drifttime",14999,0.,14999.);
  TH1F *hU2 = new TH1F("hU2","drifttime",14999,0.,14999.);

  for(Int_t r=0;r<nrofruns;r++) {
      sprintf(namerun,"/home/lindsay/K600/DATA/PR217/sorted0%d.root",run_no[r]);
      //cout << "run #: "<<run_no[r] <<endl;
      TFile *runfile = TFile::Open(namerun);

      sprintf(addtdc,"hX1->Add(hX1_DriftTimeGood)");
      gROOT->ProcessLine(addtdc);
      sprintf(addtdc,"hU1->Add(hU1_DriftTimeGood)");
      gROOT->ProcessLine(addtdc);
      sprintf(addtdc,"hX2->Add(hX2_DriftTimeGood)");
      gROOT->ProcessLine(addtdc);
      sprintf(addtdc,"hU2->Add(hU2_DriftTimeGood)");
      gROOT->ProcessLine(addtdc);

      runfile->Close();
  }

  float lut;
  Int_t nbins;
 
  nbins = hX1->GetNbinsX(); 
  for(Int_t i=0;i<nbins;i++) {        // Now do the integration
    lut = 1.0-(hX1->Integral(0,i)/hX1->Integral(0,nbins));
    //printf("%g\n",1.0-(h1->Integral(0,i)/h1->Integral(0,nbins)));
    outx1 << lut  << endl;
  }

  nbins = hU1->GetNbinsX(); 
  for(Int_t i=0;i<nbins;i++) {        // Now do the integration
    lut = 1.0-(hU1->Integral(0,i)/hU1->Integral(0,nbins));
    outu1 << lut  << endl;
  }

  nbins = hX2->GetNbinsX(); 
  for(Int_t i=0;i<nbins;i++) {        // Now do the integration
    lut = 1.0-(hX2->Integral(0,i)/hX2->Integral(0,nbins));
    outx2 << lut  << endl;
  }

  nbins = hX1->GetNbinsX(); 
  for(Int_t i=0;i<nbins;i++) {        // Now do the integration
    lut = 1.0-(hU2->Integral(0,i)/hU2->Integral(0,nbins));
    outu2 << lut  << endl;
  }


  outx1.close();
  outu1.close();
  outx2.close();
  outu2.close();
}

  // First set bincontent to 0 for those bins that should not form
  // part of the integration
  // Dec09: X1:6500 8200   X2:6500 8200   U1:6500 8200  U2:6500 8000
  // this is off course made redundent if you take the spectra made clean
  // in the analysis
  //
  // To do:
  // TH1F *h1 = new TH1F("h1","drifttime",14999,0.,14999.);
  // Then put the appropriete drifttime into h1:
  // h1=hX1_DriftTimeGood
  // Now execute this script.

/*
  for(Int_t j=0;j<6400;j++) {
    h1->SetBinContent(j,0);
  }
  for(Int_t j=8000;j<nbins;j++) {
    h1->SetBinContent(j,0);
  }
*/

