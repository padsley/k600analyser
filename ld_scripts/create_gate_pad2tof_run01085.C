{
// you have to first plot the

TCutG *cut2=new TCutG();
CUTpad2tof = (TCutG*) gPad -> WaitPrimitive("CUTG");

CUTpad2tof -> SetName("CUTpad2tof"); 
CUTpad2tof -> SetVarX("tof");
CUTpad2tof -> SetVarY("pad2");

// uncomment next section only if you want to write the gate to somewhere

file20 = new TFile("CUTpad2tof_run01085.root","NEW");
CUTpad2tof -> Write();
file20 -> ls();
file20 -> Close();

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()

}
