{
  bool Mg24 = false;
  bool O16 = true;

  TChain *ch = new TChain("DATA");

  vector<int> runlist;

  if(O16)
    {
      ifstream input;
      input.open("16O_runs.dat");

      while(!input.eof())
	{
	  int dummy = 0;
	  input >> dummy;
          if(dummy>1)
	  {
	    runlist.push_back(dummy);
          }
	}
    }
  else if(Mg24)
    {
      ifstream input;
      input.open("24Mg_runs.dat");
      
      while(!input.eof())
	{
	  int dummy = 0;
	  input >> dummy;
          if(dummy>1)
	  {
 	    runlist.push_back(dummy);
	  }
	}
    }

  cout << "number of runs: " << runlist.size() << endl;

  for(int i=0;i<(int)runlist.size();i++)
    {
      char buffer[256];
      sprintf(buffer,"/vicepda/althome/k600user/PR263/sortedfiles/sorted0%d.root",runlist[i]);
      cout << buffer << endl;
      TFile *f = TFile::Open(buffer);

      if(f)
	{
	  ch->Add(buffer);
	}
    }

gROOT->ProcessLine(".x /vicepda/althome/k600user/PR263/sortedfiles/padtof1047.C"); // USE THIS KIND OF LINE FOR CUTS SAVED AS C FILES ".C"
DATA->SetAlias("fpEnergy","X1pos*7.995-4374.9");
}
