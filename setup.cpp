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
      else if(LineBuffer.compare(0,10,"ADCModules") == 0)
      {
	ConfigFile >> DataBuffer;
	if(verbose)std::cout << std::atof(DataBuffer.c_str()) << std::endl;
	ConfigOutput << "int ADCModules = " << std::atof(DataBuffer.c_str()) << ";" << std::endl << std::endl;
      }
      else if(LineBuffer.compare(0,10,"TDCModules") == 0)
      {
	ConfigFile >> DataBuffer;
	if(verbose)std::cout << std::atof(DataBuffer.c_str()) << std::endl;
	ConfigOutput << "int TDCModules = " << std::atof(DataBuffer.c_str()) << ";" << std::endl << std::endl;
      }
      else if(LineBuffer.compare(0,3,"MMM") == 0)
      {
	  readMMMConfiguration = true;
	  
	  while(readMMMConfiguration)
	  {
	    int NumberOfMMM;
	    int **MMMADCChannelLimits;
	    int **MMMTDCChannelLimits;
	    
	   ConfigFile >> DataBuffer;
	   std::cout << DataBuffer << std::endl;
	   if(DataBuffer.compare(0,6,"Number")==0)
	   {
	      ConfigFile >> DataBuffer;
	      if(verbose)std::cout << std::atof(DataBuffer.c_str()) << std::endl;
	      ConfigOutput << "int NumberOfMMM = " << std::atof(DataBuffer.c_str()) << ";" << std::endl << std::endl;
	   }
	   else if(DataBuffer.compare(0,1,"1")==0)
	   {
	     ConfigFile >> DataBuffer;
	     if(DataBuffer.compare(0,3,"ADC")==0)
	     {
	       
	     }
	   else if(DataBuffer.compare(0,3,"MMM") == 0)
	   {
	     readMMMConfiguration = false;
	     ConfigOutput << "int MMMADCChannelLimits[" << NumberOfMMM
	     
	  }
      }
    }   
  }
}
  