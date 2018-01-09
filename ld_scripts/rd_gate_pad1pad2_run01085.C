{

TFile * file20_12 = new TFile("CUTpad1pad2_run01085.root","OLD");
TCutG *cut_12   = (TCutG*) file20_12 -> Get( "CUTpad1pad2" );
cut_12->SetVarX("pad2");
cut_12->SetVarY("pad1");

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()
}
