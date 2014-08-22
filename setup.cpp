#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>

//using namespace std;

int main()
{
  bool verbose = true;
  std::string Path = "Define.txt";
  printf("Using %s as the Definition File\n",Path.c_str());

  std::ifstream ConfigFile;
  ConfigFile.open(Path.c_str());
  std::string LineBuffer;
  std::string DataBuffer;

  bool readTDCModules = false;
  bool readW1Configuration = false;
  bool readMMMConfiguration = false;
  
  std::ofstream ConfigOutput;
  ConfigOutput.open("Parameters.c");

  while(!ConfigFile.eof())
    {
      getline(ConfigFile, LineBuffer);
      {
	std::cout << LineBuffer << std::endl;
	if(LineBuffer.compare(0,1,"%")==0) ConfigFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	if(LineBuffer.compare(0,10,"ADCModules") == 0)
	  {
	    ConfigFile >> DataBuffer;
	    if(verbose)std::cout << std::atof(DataBuffer.c_str()) << std::endl;
	    ConfigOutput << "int ADCModules = " << std::atof(DataBuffer.c_str()) << std::endl << std::endl;
	  }
      }
    }

}
