{
// you have to first plot the

TCutG *cut1=new TCutG();
CUTpad1tof = (TCutG*) gPad -> WaitPrimitive("CUTG");

CUTpad1tof -> SetName("CUTpad1tof"); 
CUTpad1tof -> SetVarX("tof");
CUTpad1tof -> SetVarY("pad1");

// uncomment next section only if you want to write the gate to somewhere

file20 = new TFile("CUTpad1tof_run01085.root","NEW");
CUTpad1tof -> Write();
file20 -> ls();
file20 -> Close();

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()

}
