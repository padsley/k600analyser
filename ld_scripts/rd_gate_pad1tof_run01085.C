{

TFile * file20_1 = new TFile("CUTpad1tof_run00146.root","RECREATE");
TCutG *cut_1   = (TCutG*) file20_1 -> Get( "CUTpad1tof" );
cut_1->SetVarX("tof");
cut_1->SetVarY("pad1");

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()
}
