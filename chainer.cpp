{
  bool Li2CO3 = true;
  bool Carbon = false;

  TChain *ch = new TChain("DATA");

  vector<int> runlist;

  if(Li2CO3)
    {
      ifstream input;
      input.open("Li2CO3_runs.dat");

      while(!input.eof())
	{
	  int dummy = 0;
	  input >> dummy;
	  runlist.push_back(dummy);
	}
    }
  else if(Carbon)
    {
      ifstream input;
      input.open("C12_runs.dat");
      
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
      sprintf(buffer,"/media/Elements/PR226/sorted0%d.root",runlist[i]);
      cout << buffer << endl;
      TFile *f = TFile::Open(buffer);

      if(f)
	{
	  ch->Add(buffer);
	}
    }
}
