/*
 * =====================================================================================
 *
 *       Filename:  TSIS3302.hxx
 *
 *    Description: Class for the Samples from SIS3302 ADC Measurement. 
 *
 *        Version:  1.0
 *        Created:  09/10/2015 11:52:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lee Pool (LCP), lcpool@tlabs.ac.za
 *        Company:  Ithemba Labs.
 *
 * =====================================================================================
 */

#ifndef TSIS3302_hxx_seen
#define TSIS3302_hxx_seen

#include <vector>

#include "TGenericData.hxx"

/// Class for each SIS3302 gamma v1415 ADC measurement
/// For the definition of obscure variables see the SIS3302 v1415 gamma manual.
class VADCMeasurement {

  friend class TSIS3302Data;

public:
  
  /// Get a Raw Sample
  uint32_t GetRawMeasurement() const {return (adc_measurement_raw_word);}

 private:

  /// Fields to hold the header, measurement, trailer and error words.
  uint32_t adc_header_word;
  uint32_t adc_measurement_raw_word;

  /// Constructor; need to pass in header and measurement.
  VADCMeasurement(uint32_t header, uint32_t raw_sample):
    adc_header_word(header),
    adc_measurement_raw_word(raw_sample){
  }


  VADCMeasurement();    
};


/// Class for storing data from SIS3302 gamma v1415 VME Module
class TSIS3302Data: public TGenericData {

 
public:

  /// Constructor
  TSIS3302Data(int bklen, int bktype, const char* name, void *pdata);


  void Print();
  
 /// Get Pileup Flag
  uint32_t GetPilupFlag() const {return ((fAdc_use_word) >> 31);}
  
  /// Get Retrigger Flag
  uint32_t GetRetriggerFlag() const {return ((fAdc_use_word) >> 30);}

  /// Get ADC N+1 Neighbour trigger flag
  uint32_t GetADCNplus1flag() const {return ((fAdc_use_word) >> 29);}
  
	/// Get Max energy Value
	uint32_t GetMaxEnergyValue() const {return (fAdc_energy_max_value);}

  /// Get ADC N-1 Neighbour trigger flag
  uint32_t GetADCNminus1flag() const {return ((fAdc_use_word) >> 28);}

  /// Get Fast Trigger Counter 
  uint32_t GetFastTriggerCounter() const {return ((fAdc_use_word>>24)&0xf);}
  
  /// Get Trigger Flag
  uint32_t GetTriggerFlag() const {return ((fAdc_use_word)&0x1);}

    /// Get the Vector of ADC Measurements.
  std::vector<VADCMeasurement>& GetMeasurements() {return fMeasurements;}


private:

  /// Vector of SIS3302 ADC Measurements.
  std::vector<VADCMeasurement> fMeasurements;


  /// Fields to hold the header, measurement, trailer and error words.
  uint32_t fAdc_header_word;
  uint32_t fAdc_use_word;
  uint32_t fAdc_energy_max_value;
  uint32_t fAdc_energy_first_value;
  uint32_t fAdc_timestamp_lower;
  uint32_t fAdc_trailer_word;

};

#endif //TSIS3302_hxx_seen
