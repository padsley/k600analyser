#include "TMesytecData.hxx"

// Must be either 11, 12 or 13 bit; default is 11 bit
int gBitResolution = 0x7ff; 


uint32_t ADCMeasMesy::GetMeasurement() const {return (adc_measurement_word & gBitResolution);}



TMesytecData::TMesytecData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{

  /// Save the current header.
  fGlobalHeader =0;

  for(int i = 0; i < GetSize(); i++){
    uint32_t word = GetData32()[i];

    if( (word & 0xc0000000) == 0x40000000){ // header word
      fGlobalHeader = word;
      // Figure out which bit resolution we are using
      int bitresolution = ((fGlobalHeader & 0x7000) >> 12);
      if(bitresolution == 1)
	gBitResolution = 0x7ff; 
      else if(bitresolution == 2)
	gBitResolution = 0xfff;
      else if(bitresolution == 4)
	gBitResolution = 0x1fff;
      
    }

    if( (word & 0xc0000000) == 0x00000000 && 
	(word & 0x3fe00000) == 0x04000000 ){ // Data word
      fMeasurements.push_back(ADCMeasMesy(fGlobalHeader,word));
    }
    
    if( (word & 0xc0000000) == 0x00000000 && 
	(word & 0x3fe00000) == 0x04000000 ){ // Data extended timestamp
      // Not currently supported!!!
    }
    
    if((word & 0xc0000000) == 0xc0000000){ // trailer word
      fGlobalTrailer = word;
    }

  }

}
 

void TMesytecData::Print(){
 
  std::cout << "Data for Mesytec module in bank " << GetName() << std::endl;
  std::cout << "Module ID = " << GetModuleID() << std::endl;
  std::cout << "Bit resolution : 0x" << std::hex 
	    << gBitResolution << std::dec << std::endl;

  std::cout << "Timestamp = " << GetTimeStamp() << std::endl;

  std::vector<ADCMeasMesy> measurements = GetMeasurements();
  std::cout << "Number of measurements: " << measurements.size() << std::endl;

  for(unsigned int i = 0; i < measurements.size(); i++){
    ADCMeasMesy adcmeas = measurements[i];
    std::cout << "meas: " << adcmeas.GetMeasurement() 
	      << " [chan="<< adcmeas.GetChannel()
	      <<",OOR=" << adcmeas.IsOutOfRange() << "]  ";
    if(i%2==1) std::cout <<  std::endl;
  }
  std::cout <<  std::endl;


}
