#ifndef TV1720RawData_hxx_seen
#define TV1720RawData_hxx_seen

#include <vector>

#include "TGenericData.hxx"


/// Class to store information from a single V1720 ZLE pulse.
class TV1720RawZlePulse {

 public:
  
  /// Constructor
 TV1720RawZlePulse(int firstBin, std::vector<uint32_t> samples):
  fFirstBin(firstBin),fSamples(samples){};

 TV1720RawZlePulse(){};
  

  /// Get the first bin for this pulse
  int GetFirstBin() const {
    return fFirstBin;    
  }

  /// Get the number of samples.
  int GetNSamples() const {
    return fSamples.size();
  }

  /// Get the first bin for this pulse
  int GetSample(int i) const {
    if(i >= 0 && i < (int)fSamples.size())
      return fSamples[i];
    
    return -1;
  }

 private:
  
  /// The first bin for this ZLE pulse.
  int fFirstBin;

  /// The set of samples for this ZLE pulse.
  std::vector<uint32_t> fSamples;
    

};

/// Class to store information from a single V1720 channel.
/// Class will store either the full ADC waveform (if not compressed)
/// or a vector of TV1720RawZlePulse (if compressed).
class TV1720RawChannel {
  
 public:

  /// constructor
 TV1720RawChannel(int channel, bool iscompressed): 
  fChannelNumber(channel),fIsZLECompressed(iscompressed){
      
  }


  int GetChannelNumber() const {return fChannelNumber;};

  
  bool IsZLECompressed() const {return fIsZLECompressed;};
  
  
  /// Get the ADC sample for a particular bin (for uncompressed data).
  int GetNSamples() const{return fWaveform.size();};

  /// Get the ADC sample for a particular bin (for uncompressed data).
  int GetADCSample(int i) const{
    if(i >= 0 && i < (int)fWaveform.size())
      return fWaveform[i];

    // otherwise, return error value.
    return -1;
  }

  /// Get the number of ZLE pulses (for compressed data)
  int GetNZlePulses() const {return fZlePulses.size();};
  

  /// Get the ZLE pulse (for compressed data
  TV1720RawZlePulse GetZlePulse(int i) const { 
    if(i >= 0 && i < (int)fZlePulses.size())
      return fZlePulses[i];

    // otherwise, return error value.
    return TV1720RawZlePulse();

  }

  /// Returns true for objects with no ADC samples or ZLE pulses.
  int IsEmpty() const {
    if(fZlePulses.size()==0 && fWaveform.size()==0)
      return true;
    return false;
  }

  /// Add an ADC sample
  /// Warning: this method just adds an ADC sample to the back 
  /// of the vector.  Must add in order and must not add any sample twice.
  void AddADCSample(uint32_t sample){ fWaveform.push_back(sample);};
  
  /// Add an ZLE pulse
  /// Warning: this method just adds a ZLE pulse to the back
  /// of the vector.  Must add in order and must not add any pulse twice.
  void AddZlePulse(TV1720RawZlePulse pulse){ fZlePulses.push_back(pulse);};
  

 private:

  /// Channel number
  int fChannelNumber;

  /// Is ZLE compressed
  bool fIsZLECompressed;
  
  std::vector<TV1720RawZlePulse> fZlePulses;
  std::vector<uint32_t> fWaveform;

};


/// Class to store data from CAEN V1720, 250MHz FADC.
///
/// Full info on CAEN V1720 module
/// http://daq-plone.triumf.ca/HR/VME/CAEN/v1720rev9.pdf/view
///
/// This class encapsulates the data from a single board (in a single MIDAS bank).
/// This decoder is for the default or ZLE version of the firmware.  Not the DPP firmware
class TV1720RawData: public TGenericData {

public:

  /// Constructor
  TV1720RawData(int bklen, int bktype, const char* name, void *pdata);

  /// Get the number of 32-bit words in bank.
  uint32_t GetEventSize() const {return (fGlobalHeader0 & 0xffffff);};

  /// Get the channel mask; ie, the set of channels for which we 
  /// have data for this event.
  uint32_t GetChannelMask() const {return (fGlobalHeader1 & 0xff);};

  /// Is the V1720 data ZLE compressed?
  bool IsZLECompressed() const {return ((fGlobalHeader1 >> 24) & 0x1);};

  /// Get event counter
  uint32_t GetEventCounter() const {return ((fGlobalHeader2) & 0xffffff);};

  /// Get trigger tag
  uint32_t GetTriggerTag() const {return ((fGlobalHeader3) & 0xffffffff);};


  /// Get Number of channels in this bank.
  int GetNChannels() const {return fMeasurements.size();}
  
  /// Get Channel Data
  TV1720RawChannel GetChannelData(int i) {
    if(i >= 0 && i < (int)fMeasurements.size())
      return fMeasurements[i];

    return TV1720RawChannel(0,0);
  }
  

  void Print();


private:

  /// Helper method to handle ZLE compressed data.
  void HandlZLECompressedData();

  /// Helper method to handle uncompressed data.
  void HandlUncompressedData();

  /// The overall global headers
  uint32_t fGlobalHeader0;
  uint32_t fGlobalHeader1;
  uint32_t fGlobalHeader2;
  uint32_t fGlobalHeader3;
  

  /// Vector of V1720 measurements
  std::vector<TV1720RawChannel> fMeasurements;

};

#endif
