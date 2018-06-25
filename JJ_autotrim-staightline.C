// JJvZyl - Jan 2011
//
// JJ's code to 'autotrim' the TDCs and create a CableLength file. (RN comment)
//
// This code reads all the TDC channels (TDCchan_***) from a specified group of runfiles,
// calculates their first derivative, and determines the bin number of the minimum gradient
//
// The first 5 lines must be specified...
// Choose to draw a single TDC channel histo for diagnosis - set flag==1, else set flag=2
//
{
    ofstream outpfile;
    
    Int_t run_no[]={1090,1114,1115,1120,1121,1125,1131,1132,1138,1156,1157};
    Int_t nrofruns=sizeof(run_no)/sizeof(run_no[0]);
    Int_t binave  = 50;	// # bins to average over - ideally to give smooth curve: ~50 bins
    Int_t bingrad = 20;	// # bins to calc gradient - ideally > small noise and < large peaks, ~30 bins
    Int_t binsm   = 50;	// # bins to smooth gradient curve for minimum calc - ideally ~20
    
    Int_t t0corr=8000;		// Corrected bin number for cable length shift
    const Int_t nbins=14999;		// Bin size of TDCchan histos
    const Int_t histImax=895;		// 420 for VDC 1 alone; set to 896 for both VDC 1 and 2
    const Int_t histImin = 0;
    
    //Int_t xnumbins,xminbin=2300,xmaxbin=5000;	// Plot range for zoomed histo
    Int_t xnumbins,xminbin=5000,xmaxbin=9000;	// Plot range for zoomed histo
    Int_t tmin,tmax, ti, tf, i, j, k, histI;
    Int_t sum, flag, binp ,binm, numTDC;
    Float_t sumf, avei, avef, gradmin,gradmax, gradsm, grad, gradtemp1[nbins],gradtemp2[nbins],scale;
    Char_t namerun[128], addtdc[128], nameTDC[128], text1[100], text2[100];
    Float_t slope,offset;
    
    flag=2;
    
    // flag=1 for SINGLE TDC channel
    // flag=2 for ALL TDC chans + plotted during analysis
  		// flag=3 for ALL TDC chans + added TDC's written to output .root file
  		// flag=4 for ALL TDC chans, BUT no histos will be plotted during analysis
    
    //histImin=656;
    //histImax=671;		// 420 for VDC 1 alone; set to 896 for both VDC 1 and 2
    
    /*
     if(flag==1) {
     histImin=655;	//specify single TDC chan number
     histImax=histImin;
     }
     */
    
    xnumbins=xmaxbin-xminbin;
    TH1F *h_TDCadd[histImax+1];
    TH1F *h_ave = new TH1F("ave","Average",nbins,0,nbins);
    TH1F *h_grad = new TH1F("grad","Gradient",nbins,0,nbins);
    TH1F *hzoom = new TH1F("hzoom","Added TDC histo's (zoomed in)",xnumbins,xminbin,xmaxbin);
    
    if(flag!=1) outpfile.open("CableLengthtmp.dat");
        TFile *arrayoutf = new TFile("TDCarrays.root", "RECREATE");
        
        
        
        binp=(bingrad+binave)/2;
        binm=(bingrad-binave)/2;
        //  cout << binm <<" "<< binp<<endl;
        TCanvas *c1 = new TCanvas();
        
        for(histI=histImin;histI<=histImax;histI++) {
            slope=1.;    //initialize to something nonzero
            offset=1.;   //initialize to something nonzero
            cout << "TDC chan #: " << histI <<endl;
            sprintf(nameTDC,"TDCadd_%d",histI);
            h_TDCadd[histI] = new TH1F(nameTDC,"Added TDC channel",nbins,0,nbins);
            //*******************************************************
            // Loop over each of the run files...
            //*******************************************************
            for(Int_t r=0;r<nrofruns;r++) {
                sprintf(namerun,"../sortedAutoTrim/sorted0%d.root",run_no[r]);
                //sprintf(namerun,"/FAST/run%d.root",run_no[r]);
                if(flag==1) cout << "run #: "<<run_no[r] <<endl;
                TFile *runfile = TFile::Open(namerun);
                //runfile->cd("A_TDCdata");	// Subdirectory created in my f-plane.c code
                sprintf(addtdc,"h_TDCadd[histI]->Add(TDCchan_%d)",histI);
                gROOT->ProcessLine(addtdc);
                runfile->Close();
                
                
                //------------------------------------------------
                //      Removing the noise
                h_TDCadd[histI]->Smooth(20);
                
                int maximumBin = h_TDCadd[histI]->GetMaximumBin();
                double approximateNoiseLevel = 0.0;
                double approximateFlatLevel = 0.0;
                
                for(int i=maximumBin+300; i<maximumBin+800; i++)
                {
                    approximateNoiseLevel += h_TDCadd[histI]->GetBinContent(i)/500.0;
                }
                
                for(int i=0; i<h_TDCadd[histI]->GetNbinsX(); i++)
                {
                    double binContent = h_TDCadd[histI]->GetBinContent(i+1);
                    double newBinContent = binContent-approximateNoiseLevel;
                    
                    if(newBinContent<0.0)
                    {
                        newBinContent = 0.0;
                    }
                    
                    h_TDCadd[histI]->SetBinContent(i+1, newBinContent);
                }
            }
            if(flag==3) {
                arrayoutf->cd();	// Write added TDC histos to root file...and next line
                h_TDCadd[histI]->Write();
            }
            //***********************************************************************
            // Averaging raw TDC data, calc 1st derivative; fill histogram h_ave....
            //***********************************************************************
            for(avei=0.,avef=0., i=binp;i<nbins-binp;i++) {
                for(sum=0,j=i-binp;j<=i-binm;j++) sum=sum+h_TDCadd[histI]->GetBinContent(j);
                avei=1.*sum/(binave+1);		// Average smoothing, point 1
                for(sum=0,j=i+binm;j<=i+binp;j++) sum=sum+h_TDCadd[histI]->GetBinContent(j);
                avef=1.*sum/(binave+1);		// Average smoothing, point 2
                grad=(avef-avei)/(bingrad+1);		// 3-point Newton's difference quotient
                gradtemp1[i]=grad;			// Raw derivative array
                if(flag!=4) h_ave->SetBinContent(i-bingrad/2,avei);
            }
            //*************************************************************************
            // Smooth raw derivative, fill histogram h_grad, find min/max gradient....
            //*************************************************************************
            Float_t avemax=1.*h_ave->GetMaximum();
            //    cout << "avemax: "<<avemax<<endl;
            for(tmin=0,tmax=0,gradmin=0.,gradmax=0., i=binp+binsm/2;i<nbins-binp-binsm/2;i++) {
                for(sumf=0,k=i-binsm/2;k<=i+binsm/2;k++) sumf=sumf+gradtemp1[k];
                gradsm=sumf/(binsm+1);	// Average smoothing
                if(gradsm<gradmin) {		// Find MINIMUM gradient
                    gradmin=gradsm;
                    tmin=i;
                }
                if(gradsm>gradmax) {		// Find MAXIMUM gradient
                    gradmax=gradsm;
                    tmax=i;
                }
                gradtemp2[i]=gradsm;
            }
            scale=avemax/(gradmax-gradmin+0.0000001);// Expand curve for plotting
            //      cout<<"scale: "<<scale<<endl;
            if(flag!=4) {
                for(i=binp+binsm/2;i<nbins-binp-binsm/2;i++) {
                    h_grad->SetBinContent(i,gradtemp2[i]*scale-(gradmin*scale));
                }
            }
            //********Plotting raw TDC histogram*************************************
            if(flag!=4) {
                hzoom->SetStats(0);
                hzoom->SetMaximum(h_TDCadd[histI]->GetMaximum());
                hzoom->SetMinimum(0);
                //RN hzoom->Draw();
                h_TDCadd[histI]->SetLineColor(2);
                //RN h_TDCadd[histI]->Draw("same");
            }
            //*****************************************************************************
            // Drawing single TDC channel diagnostic histos - if flag=1
            //*****************************************************************************
            if(flag!=4) {
                //RN h_ave->Draw("same");
                h_grad->SetLineColor(kBlue);
                h_grad->SetLineWidth(1);
                //RN h_grad->Draw("same");
                
                // Some details on graphs:
                TPaveText *legend1 = new TPaveText(0.1,0.85,0.46,0.9,"NDC"); // x-start,y-start,x-end,y-end
                sprintf(text1,"ave = %d; grad = %d; sm = %d",binave,bingrad,binsm);
                legend1->SetTextSize(0.04);
                legend1->AddText(text1);
                legend1->Draw();
                //*********************************************************************************
                TPaveText* legend2 = new TPaveText(0.695,0.85,0.9,0.9, "NDC");
                sprintf(text2,"TDC chan = %d",histI);
                legend2->SetTextSize(0.04);
                legend2->AddText(text2);
                legend2->Draw();
                //c1->Update();
                //----------------------addition by Retief------------------------------------------
                //if ((h_ave->GetMean()!=0) || (h_ave->GetMean()>1000)  ){
                if ((h_ave->GetMean()>1000)  ){
                    
                    h_ave->Fit("pol1","","",tmin-20,tmin+20);
                    
                    h_ave->GetFunction("pol1")->SetLineColor(3);
                    h_ave->GetFunction("pol1")->SetLineWidth(3.4);
                    slope = h_ave->GetFunction("pol1")->GetParameter(1);
                    offset = h_ave->GetFunction("pol1")->GetParameter(0);
                    
                    hzoom->Draw("");
                    h_ave->Draw("same");
                    h_TDCadd[histI]->Draw("same");
                    h_grad->Draw("same");
                    c1->Update();
                }
            }
            //*****************************************************************************
            if(flag!=1) {
                if(slope!=0){				// RN's addition
                    tmin=-offset/slope;		// RN's addition
                }					// RN's addition
                else tmin=0;				// RN's addition
                if(tmin!=0) {
                    int value = 0;
                    
                    /*
                     if(histI>=0 && histI<64)
                     {
                     value = 0 <<endl;
                     }
                     else if(histI==80)
                     {
                     value = 0 <<endl;
                     }
                     */
                    if(value>-10000 && value <10000)
                    {
                        value =  tmin-t0corr;
                    }
                    else
                    {
                        value = 0.0;
                    }
                    
                    outpfile << value <<endl;
                }
                else outpfile << tmin <<endl;
            }
            if(flag==1) cout<<"tmin = "<<tmin<<";  gradmin = "<<gradmin<<"; corr = "<<tmin-t0corr<<endl;
            if(flag==1) cout<<"tmax = "<<tmax<<";  gradmax = "<<gradmax<<endl;
            if(flag==1) {
                tmin=-offset/slope;
                cout<<"straight line y=0 at x = "<<tmin<<"; "<<endl;    //RN's addition
            }
            
            arrayoutf->cd();
            h_TDCadd[histI]->Write();
        }
    if(flag!=1) outpfile.close();
        }

