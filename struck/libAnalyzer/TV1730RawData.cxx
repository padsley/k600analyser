#include "TV1730RawData.hxx"

#include <iostream>



TV1730RawData::TV1730RawData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  
  // Do some sanity checking.  
  // Make sure first word has right identifier
	if( (GetData32()[0] & 0xf0000000) != 0xa0000000){ 
	  std::cerr << "First word has wrong identifier; first word = 0x" 
		    << std::hex << GetData32()[0] << std::dec << std::endl;
	  return;
	}

	fGlobalHeader.push_back(GetData32()[0]);
	fGlobalHeader.push_back(GetData32()[1]);
	fGlobalHeader.push_back(GetData32()[2]);
	fGlobalHeader.push_back(GetData32()[3]);
  

  
  int counter = 4;
	
	int number_available_channels = 0;
	for(int ch = 0; ch < 16; ch++){		
		if((1<<ch) & GetChMask()){
			number_available_channels++;
		}
	}

	int nwords_per_channel = (GetEventSize() - 4)/number_available_channels;
	//	std::cout << "Number of channels " << nwords_per_channel << std::endl;
	
	// Loop over channel data
	for(int ch = 0; ch < 16; ch++){
		
		if((1<<ch) & GetChMask()){

			std::vector<uint32_t> Samples;
			for(int i = 0; i < nwords_per_channel; i++){
				uint32_t sample = (GetData32()[counter] & 0x3fff);
				Samples.push_back(sample);
				sample = (GetData32()[counter] & 0x3fff0000) >> 16;
				Samples.push_back(sample);				
				counter++;
			}
			RawChannelMeasurement meas = RawChannelMeasurement(ch);
			meas.AddSamples(Samples);

			fMeasurements.push_back(meas);
			
		}
	}

}

void TV1730RawData::Print(){

  std::cout << "V1730 decoder for bank " << GetName().c_str() << std::endl;


}
