{

TFile * file20_0 = new TFile("CUTtofX1_run01085.root","OLD");
TCutG *cut_0   = (TCutG*) file20_0 -> Get( "CUTtofX1" );
cut_0->SetVarX("X1pos");
cut_0->SetVarY("tof");

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()
}
