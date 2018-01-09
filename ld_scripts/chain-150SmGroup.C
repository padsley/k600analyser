{
  TChain *DATAChain = new TChain("DATA","MEGA Tree");
  char name[128];

  // 150Sm, November/December 2014, Weekend 2 
  Int_t nrofruns=20;

	//std::vector<int> runs;

	

  //Int_t run_no[128]={121,122,126,128,129,137,147,148,153,155,161,162,164,167,170,172,173,178,180,186,188,196,200,201,205};
  Int_t run_no[128]={121,122,126,128,129,137,147,148,153,155,161,162,164,167,170,172,173,178,180,186};//,188,196,200,201,205};



  for(Int_t i=0;i<nrofruns;i++)	{
	  sprintf(name,"~/hermes-home/Analysis/iThemba/root/sorted00%3d.root",run_no[i]);
          cout<< name<<endl;
	  DATAChain->Add(name);
  }

}

