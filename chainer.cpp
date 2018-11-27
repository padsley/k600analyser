{
  bool Nd142 = true;
  bool Mg24 = false;

  TChain *ch = new TChain("DATA");

  vector<int> runlist;

  if(Nd142)
    {
      ifstream input;
      input.open("Nd142_runs.dat");

      while(!input.eof())
	{
	  int dummy = 0;
	  input >> dummy;
	  runlist.push_back(dummy);
	}
    }
  else if(Mg24)
    {
      ifstream input;
      input.open("Mg24_runs.dat");
      
      while(!input.eof())
	{
	  int dummy = 0;
	  input >> dummy;
	  runlist.push_back(dummy);
	}
    }

  cout << "number of runs: " << runlist.size() << endl;

  for(int i=0;i<(int)runlist.size();i++)
    {
      char buffer[256];
      sprintf(buffer,"/vicepda/althome/k600user/PR283/online/sorted00%d.root",runlist[i]);
      cout << buffer << endl;
      TFile *f = TFile::Open(buffer);

      if(f)
	{
	  ch->Add(buffer);
	}
    }
}
