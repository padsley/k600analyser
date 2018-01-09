{
gStyle->SetPalette(1,0);


Char_t name1[30];
     
sprintf(name1,".x rd_gate_tofX1_run01085.C");
gROOT->ProcessLine(name1);
sprintf(name1,".x rd_gate_pad1tof_run01085.C");
gROOT->ProcessLine(name1);
sprintf(name1,".x rd_gate_pad2tof_run01085.C");
gROOT->ProcessLine(name1);
sprintf(name1,".x rd_gate_pad1pad2_run01085.C");
gROOT->ProcessLine(name1);


sprintf(name1,".x histos.C");
gROOT->ProcessLine(name1);

TCut CUTbasic = "CUTtofX1 && CUTpad1tof && CUTpad2tof && CUTpad1pad2";
TCut CUTflags = "X1flag==0 && U1flag==0 && U2flag==0";
TCut CUTy = "Y1>-5 && Y1<20";

}
