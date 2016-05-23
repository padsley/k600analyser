// Simple example of how to define a class that derives from 
// TRootanaEventLoop, thereby abstracting a way a lot of the 
// default stuff associated with analyzer.cxx

#include <stdio.h>
#include <iostream>

#include "TRootanaEventLoop.hxx"
#include "TH1D.h"
#include "TV1190Data.hxx"
#include "TV792Data.hxx"

class MyTestLoop: public TRootanaEventLoop {

  int nnn ;

public:

  MyTestLoop() {
    nnn = 0;
  };

  virtual ~MyTestLoop() {};

  void BeginRun(int transition,int run,int time){
    std::cout << "Custom: begin run " << run << std::endl;
  }

  void EndRun(int transition,int run,int time){
    std::cout << "Custom end run " << run <<std::endl;
  }

  bool ProcessMidasEvent(TDataContainer& dataContainer){


    void *ptr;
    int size = dataContainer.GetMidasData().LocateBank(NULL, "FR10", &ptr);
    if (ptr){
      nnn++;
      if(nnn%100 == 0){
	std::cout << "Current run :  " << GetCurrentRunNumber() << std::endl;
	std::cout << "Trying to handle this event... " << size << " " << nnn << std::endl;
      }

      fOnlineHistDir->cd();
      char sname[256];
      sprintf(sname, "size%d", 0);
      TH1D* hsize = (TH1D*)fOnlineHistDir->Get(sname);
      if (!hsize){
	printf("Create [%s]\n", sname);
	hsize = new TH1D(sname, sname, 600, 0, 6000);
      }      
      hsize->Fill(size);
    }


    TV1190Data *v1190 = dataContainer.GetEventData<TV1190Data>("TDC0");
    if(v1190){ 

      std::cout << "TDC measurements for V1190" << std::endl;
      std::vector<TDCMeasurement>& measurements = v1190->GetMeasurements();
      for(unsigned int i = 0; i < measurements.size(); i++){
	TDCMeasurement tdcmeas = measurements[i];
	
	std::cout << "Measurement: " <<    tdcmeas.GetMeasurement() << " for tdc/chan "  <<
	  tdcmeas.GetTDCNumber() << "/"<< tdcmeas.GetChannel() << std::endl;

      }	  
      
    }

    TV792Data *v792 = dataContainer.GetEventData<TV792Data>("ADC0");
    if(v792 ){ 
 
      v792->Print();
      
    }
 
    return true;
  }


  // Describe some other command line argument
  void Usage(void){
    std::cout << "\t-D option: this is a fun new option " << std::endl;
  }

  // Define some other command line argument
  bool CheckOption(std::string option){
    const char* arg = option.c_str();
    if (strncmp(arg,"-D",2)==0){
      std::cout << arg+2 << std::endl;
      std::cout << "I'm happy with this flag!" << std::endl;
      return true;
    }

    return false;
  }

}; 






int main(int argc, char *argv[])
{

  MyTestLoop::CreateSingleton<MyTestLoop>();
  return MyTestLoop::Get().ExecuteLoop(argc, argv);

}

