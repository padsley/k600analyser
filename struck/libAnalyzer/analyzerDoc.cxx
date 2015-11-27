/*! \page analyzerClass Analyzer Classes

\section analyzerIntro Introduction


This set of classes provides the following functionality 

- A class TRootanaEventLoop that handles most of the functionality associated
with an event loop.  The user creates a derived class that inherits from TRootanaEventLoop.
- A set of bank decoder classes.

These classes depend on the Midas interface classes (TMidasFile, TMidasEvent, TMidasOnline).

\section eventLoop Event Loop Class

The TRootanaEventLoop provides the functionality to handle many of the features for a standard
event loop including 

- looping over events in a midas file or
- getting online events by connecting to midas server

The idea is that the user provides a class that derives from TRootanaEventLoop.  This derived class
would provides begin-of-run actions, end-of-run actions and midas event actions.  

The following simple example shows a full program that simply prints out the event number for
each event:


\verbatim
#include <stdio.h>
#include "TRootanaEventLoop.hxx"

class Analyzer: public TRootanaEventLoop {
public:

  Analyzer() {};

  virtual ~Analyzer() {};

  void BeginRun(int transition,int run,int time){}

  // Get the midas event and print event number
  bool ProcessMidasEvent(TDataContainer& dataContainer){
    std::cout << "Event Number " << dataContainer.GetMidasEvent().GetSerialNumber() << std::endl;
    return true;
  }
};   

int main(int argc, char *argv[])
{  
  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);
}  
\endverbatim 
  



\section histogramCreation Histogram Creation, Begin-Of-Run/End-Of-Run

Users will naturally want to create and fill ROOT histograms.
This needs to be done with some care because of:
- interaction with the output ROOT file: by default, for each run, the analyzer opens a new ROOT file outputNNNNN.root 
and make it the current directory (gOutputFile) for newly created ROOT objects, i.e. those create by "new TH1(...)". 
At the end of a run, all these objects are saved into the file, the file is closed and all these objects disappear 
from memory. To create ROOT objects that persist across runs, use some other ROOT directory (i.e. gOnlineHistDir->cd()).
- when using the netDirectoryServer (HAVE_LIBNETDIRECTORY), the contents of gOnlineHistDir and gOutputFile are 
exported to outside applications. Other programs, i.e. ROODY, can use TNetDirectory to "see" the histograms 
(and other objects) as they are filled. (Note: this does not work for most TTree objects because they cannot 
be easily "exported").

The general rule is therefore that you should create histograms in your event loop BeginRun() function, rather than 
in the constructor or the Initialize() method. That is the only way to ensure that the histograms are created after the
output ROOT file is created.  Also, you should ensure that you delete the old histograms before producing new ones.  The following
example shows how this would look: 

\verbatim
#include <stdio.h>
#include "TRootanaEventLoop.hxx"
#include <TH1F.h> 

class Analyzer: public TRootanaEventLoop {
private:
  
  TH1F* evtno_histo; // histogram the event numbers.
  
public:

  Analyzer() {
    evtno_histo = 0; // initialize pointer to null.
  };

  virtual ~Analyzer() {};

  void BeginRun(int transition,int run,int time){
    // delete old histogram, if it exists.
    TH1F *old = (TH1F*)gDirectory->Get("evtno_histo");
    if (old) delete old;

    // create new histogram
    evtno_histo = new TH1F("evtno_histo","Event Number Histogram",1000,0,10000);
  }

  bool ProcessMidasEvent(TDataContainer& dataContainer){
    evtno_histo->Fill(dataContainer.GetMidasEvent().GetSerialNumber());
    return true;
  }
}; 

int main(int argc, char *argv[])  
{
  Analyzer::CreateSingleton<Analyzer>();  
  return Analyzer::Get().ExecuteLoop(argc, argv);  
}
\endverbatim 

You also have disable the automatic creation of ROOT files for each run by using the
function TRootanaEventLoop::DisableRootOutput(true) and then create histograms where ever
you want in your program.
 

\section decoder Bank Decoder  

Naturally we want to do more than just checking the event numbers; we need to process the data in 
the data banks.  The rootana analyzer classes facilitate this by providing decoding 
classes for a number of standard MIDAS banks.

Since the data being stored in each bank is different, the format of the decoder classes will also be
quite different.  You should see the individual decoders to see how the data is organized; 
for now we currently have the following decoders:
- TV792Data
- TV1190Data
- TMesytecData

The following example shows how to acces information for a particular V792 bank called 'ADC0' and
histogram the results:


\verbatim
#include <stdio.h>
#include "TRootanaEventLoop.hxx"
#include <TH1F.h>
#include <TV792Data.hxx>

class Analyzer: public TRootanaEventLoop {
private:
  
  TH1F* adc; // ADC spectrum for all V792 channels in bank.
  
public:

  Analyzer() {
    adc = 0; // initialize pointer to null.
  };

  virtual ~Analyzer() {};

  void BeginRun(int transition,int run,int time){
    // delete old histogram, if it exists.
    TH1F *old = (TH1F*)gDirectory->Get("adc");
    if (old) delete old;

    // create new histogram
    adc = new TH1F("adc","V792 ADC Spectrum",4200,0,4200);
  }

  bool ProcessMidasEvent(TDataContainer& dataContainer){

    // Get pointer to the V792 decoded data; need to provide the 
    // bank name.
    TV792Data *data = dataContainer.GetEventData<TV792Data>("ADC0");
    if(data){
      // Loop over all measurement for all channels
      std::vector<VADCMeasurement> measurements = data->GetMeasurements();
      for(unsigned int i = 0; i < measurements.size(); i++){ // loop over measurements
        adc->Fill(measurements[i].GetMeasurement());
      }
    }
    
    return true;
  }
}; 

int main(int argc, char *argv[])
{
  Analyzer::CreateSingleton<Analyzer>();
  return Analyzer::Get().ExecuteLoop(argc, argv);
}
\endverbatim



You can also get access to the raw information in the MIDAS banks, if you haven't yet 
written a decoder for a particular bank type.  You do this by doing accessing a bank using 
the TGenericData class.

\section analyzerFlowControl Program Flow Control

The following images try to describe the program flow graphically. This is largely the same information
as what is described above, but presently differently.


\image html event_loop_offline_diagram.png "Program flow for offline rootana analyzer" width=10cm

\image html event_loop_online_diagram.png "Program flow for online rootana analyzer" width=10cm


\section histogram Histogram Classes

We also provide an ABC class for set of histograms: THistogramArrayBase.
The derived histogram-set class would specify how to create and fill an interesting
set of histograms.
The hope is that these generic histogram-sets would make it easy to provide 
generic analyzer displays.  

It remains to be seen how useful these histogram-set classes
are in other context.

T. Lindner
*/

