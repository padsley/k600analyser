#ifndef TMesytecData_hxx_seen
#define TMesytecData_hxx_seen

#include <vector>

#include "TGenericData.hxx"


/// Class for each TDC measurement
/// For the definition of obscure variables see the CAEN V1190 manual.
/// Currently doesn't support reading out extended timestamp!
///
class ADCMeasMesy {

  friend class TMesytecData;

public:
  
  /// Get the ADC measurement
  uint32_t GetMeasurement() const;

  /// Get Module ID
  uint32_t GetModuleID() const {return (adc_header_word & 0xff0000) >> 16;};

  /// Get the channel number
  uint32_t GetChannel() const {return ((adc_measurement_word & 0x1f0000) >> 16);}

  /// Is Out Of Range?
  bool IsOutOfRange() const {return ((adc_measurement_word & 0x4000) == 0x4000);}
  
private:

  /// Fields to hold the header, measurement, extendedtimestamp words.
  uint32_t adc_header_word;
  uint32_t adc_measurement_word;
  uint32_t adc_extendedtimestamp_word;
  
  /// Constructor; need to pass in header and measurement.
  ADCMeasMesy(uint32_t header, uint32_t measurement):
    adc_header_word(header),
    adc_measurement_word(measurement),
    adc_extendedtimestamp_word(0){};


  ADCMeasMesy();    
};




/// Class to store data from Mesytec MADC32 module.
/// For details on this module see:
/// http://daq-plone.triumf.ca/HR/VME/mesytec-modules/MADC-32_V14_fw0126.pdf/at_download/file
class TMesytecData: public TGenericData {

public:

  /// Constructor
  TMesytecData(int bklen, int bktype, const char* name, void *pdata);

  void Print();

  /// Get the Vector of ADC Measurements.
  std::vector<ADCMeasMesy>& GetMeasurements() {return fMeasurements;}

  /// Get ADC resolution (11, 12 or 13 bit);
  /// Return values: 1 -> 11 bit, 2 -> 12 bit, 3 -> 13 bit (??)
  uint32_t GetADCResolution() const {return (fGlobalHeader & 0x7000) >> 12;};

  /// Get Module ID
  uint32_t GetModuleID() const {return (fGlobalHeader & 0xff0000) >> 16;};
  
  /// Get Output format (???)
  uint32_t GetOutputFormat() const {return (fGlobalHeader & 0x8000) >> 15;};
  
  // Get overall timestamp
  uint32_t GetTimeStamp() const {return (fGlobalTrailer & 0x3fffffff);};
  
private:

  /// The overall global header
  uint32_t fGlobalHeader;

  /// The overall global trailer
  uint32_t fGlobalTrailer;

  /// Vector of ADC Measurements.
  std::vector<ADCMeasMesy> fMeasurements;

};




#endif
