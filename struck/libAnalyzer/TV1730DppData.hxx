#ifndef TV1730DppData_hxx_seen
#define TV1730DppData_hxx_seen

#include <vector>

#include "TGenericData.hxx"

/// Class for each channel measurement
/// For the definition of obscure variables see the CAEN V1730 manual (for DPP readout).
class ChannelMeasurement {

  friend class TV1730DppData;

public:
  
	/// See CAEN DPP manual for definition of all these variables
	bool GetDualTraceEnabled(){return (header1 & 0x80000000) >> 31;}
	bool GetChargeEnabled(){return (header1 & 0x40000000) >> 30;}
	bool GetTimeEnabled(){return (header1 & 0x20000000) >> 29;}
	bool GetBaselineEnabled(){return (header1 & 0x10000000) >> 28;}
	bool GetSamplesEnabled(){return (header1 & 0x08000000) >> 27;}
	int GetNSamples(){
		int header_size = (header1 & 0xfff)*8;
		if(header_size != fSamples.size())
			std::cerr << "v17390::ChannelMeasurement N samples doesn't match!!" 
								<< header_size << " " << fSamples.size() <<std::endl;
		return header_size;
	}
	
	int GetChannel(){ return fChan;}

  /// Get Errors
  uint32_t GetSample(int i){
		if(i >= 0 && i < fSamples.size())
			return fSamples[i];
		return 9999999;
	}

	void AddSamples(std::vector<uint32_t> Samples){
		fSamples = Samples;
	}

private:

	int fChan; // channel number
	uint32_t header0;
	uint32_t header1;

  /// Constructor; need to pass in header and measurement.
  ChannelMeasurement(int chan, uint32_t iheader0, uint32_t iheader1){
		fChan = chan;
		header0 = iheader0;
		header1 = iheader1;
	}

	std::vector<uint32_t> fSamples;


};


/// Class to store DPP data from CAEN V1730.
class TV1730DppData: public TGenericData {

public:

  /// Constructor
  TV1730DppData(int bklen, int bktype, const char* name, void *pdata);


  /// Get Event Counter
  uint32_t GetEventCounter() const {return (fGlobalHeader[2]);};

  /// Get Geographical Address
  uint32_t GetGeoAddress() const {return (fGlobalHeader[1] & 0xf8000000) >> 27 ;};

  /// Get the extended trigger time tag
  uint32_t GetTriggerTimeTag() const {return fGlobalHeader[3];};

	/// Get channel mask
	uint32_t GetChMask(){return (fGlobalHeader[1] & 0xff);};

  void Print();

  /// Get the Vector of TDC Measurements.
  std::vector<ChannelMeasurement>& GetMeasurements() {return fMeasurements;}



private:
  
  // We have vectors of the headers/trailers/etc, since there can be 
  // multiple events in a bank.

  /// The overall global header
  std::vector<uint32_t> fGlobalHeader;  

  /// Vector of V1730 Measurements.
  std::vector<ChannelMeasurement> fMeasurements;

};

#endif
