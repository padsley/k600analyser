{

TFile * file20_2 = new TFile("CUTpad2tof_run01085.root","OLD");
TCutG *cut_2   = (TCutG*) file20_2 -> Get( "CUTpad2tof" );
cut_2->SetVarX("tof");
cut_2->SetVarY("pad2");

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()
}
