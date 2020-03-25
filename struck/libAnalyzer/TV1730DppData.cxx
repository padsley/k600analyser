#include "TV1730DppData.hxx"

#include <iostream>



TV1730DppData::TV1730DppData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  
  // Do decoding.  Decoding is complicated by the fact that there can be 
  // multiple events in the same bank.  So need to find and save multiple
  // event headers, trailers.

	fGlobalHeader.push_back(GetData32()[0]);
	fGlobalHeader.push_back(GetData32()[1]);
	fGlobalHeader.push_back(GetData32()[2]);
	fGlobalHeader.push_back(GetData32()[3]);
  
  // Do some sanity checking.  
  // Make sure first word has right identifier
  if( (GetData32()[0] & 0xf0000000) != 0xa0000000) 
    std::cerr << "First word has wrong identifier; first word = 0x" 
	      << std::hex << GetData32()[0] << std::dec << std::endl;

	int counter = 4;
	
	// Loop over channel data
	for(int ch = 0; ch < 16; ch++){
		
		if((1<<ch) & GetChMask()){


			uint32_t header0 = GetData32()[counter];
			counter++;
			uint32_t header1 = GetData32()[counter];
			counter++;
			uint32_t size = header0 & 0x7fff;
			ChannelMeasurement meas = ChannelMeasurement(ch,header0,header1);

			int nsamples = size - 2; // calculate number of samples.
			
			std::vector<uint32_t> Samples;
			//std::cout << "nsamples " << nsamples << std::endl;
			for(int i = 0; i < nsamples; i++){
				uint32_t sample = (GetData32()[counter] & 0x3fff);
				Samples.push_back(sample);
				sample = (GetData32()[counter] & 0x3fff0000) >> 16;
				Samples.push_back(sample);				
				counter++;
			}
			meas.AddSamples(Samples);

			fMeasurements.push_back(meas);

		}
	}
	


}

void TV1730DppData::Print(){

  std::cout << "V1730Dpp decoder for bank " << GetName().c_str() << std::endl;


}
