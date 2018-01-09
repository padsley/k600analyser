{
// you have to first plot the

TCutG *cut12=new TCutG();
CUTpad1pad2 = (TCutG*) gPad -> WaitPrimitive("CUTG");

CUTpad1pad2 -> SetName("CUTpad1pad2"); 
CUTpad1pad2 -> SetVarX("pad2");
CUTpad1pad2 -> SetVarY("pad1");

// uncomment next section only if you want to write the gate to somewhere

file20 = new TFile("CUTpad1pad2_run01085.root","NEW");
CUTpad1pad2 -> Write();
file20 -> ls();
file20 -> Close();

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()

}
