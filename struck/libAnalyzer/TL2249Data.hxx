#ifndef TL2249Data_hxx_seen
#define TL2249Data_hxx_seen

#include <vector>

#include "TGenericData.hxx"

/// Class for each ADC measurement
/// For the definition of obscure variables see the LeCroy2249 manual.
class LADCMeasurement {

  friend class TL2249Data;

public:
  
  /// Get the ADC measurement
  uint32_t GetMeasurement() const {return (adc_measurement_word & 0xfff);}

  
private:

  /// Fields to hold the header, measurement, trailer and error words.
  uint32_t adc_measurement_word;
  
  /// Constructor; need to pass in header and measurement.
  LADCMeasurement(uint32_t header, uint32_t measurement):
    adc_measurement_word(measurement){
    
  }


  LADCMeasurement();    
};


/// Class for storing data from LeCroy 2249 module
class TL2249Data: public TGenericData {

 
public:

  /// Constructor
  TL2249Data(int bklen, int bktype, const char* name, void *pdata);


  void Print();
  
  /// Get the Vector of ADC Measurements.
  std::vector<LADCMeasurement>& GetMeasurements() {return fMeasurements;}


private:

  /// Vector of ADC Measurements.
  std::vector<LADCMeasurement> fMeasurements;


  /// Fields to hold the measurement

};

#endif
