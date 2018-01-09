

//#include <TCutG>
//#include <TFile>

//#include <string>

void make_CUTtofX1(string run_num){
// you have to first plot the

TCutG *cut=new TCutG();
CUTtofX1 = (TCutG*) gPad -> WaitPrimitive("CUTG");

CUTtofX1 -> SetName("CUTtofX1"); 
CUTtofX1 -> SetVarX("X1pos");
CUTtofX1 -> SetVarY("tof");

// uncomment next section only if you want to write the gate to somewhere

string tmp = "CUTtofX1_run00" + run_num + ".root";

file20 = new TFile(tmp.c_str(),"NEW");
CUTtofX1 -> Write();
file20 -> ls();
file20 -> Close();

// in order to get back to the root file you were looking at
// you should cd back into that file:     file->cd()
}





void rd_gate_tofX1(string file_name){

	TFile *file = new TFile(file_name.c_str(),"OLD");
	TCutG *cut = (TCutG*) file->Get( "CUTtofX1" );
	cut->SetVarX("X1pos");
	cut->SetVarY("tof");
}
