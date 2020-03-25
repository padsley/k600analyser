#include "TV1720RawData.h"

#include <iomanip>
#include <iostream>

void TV1720RawData::HandlZLECompressedData(){

  // >>> Loop over ZLE data and fill up  

  uint32_t chMask    = GetData32()[1] & 0xFF;
  uint32_t iPtr=4;
  for(int iCh=0; iCh<8; iCh++){
    if (chMask & (1<<iCh)){
      uint32_t chSize = GetData32()[iPtr];
      uint32_t iChPtr = 1;// The chSize space is included in chSize
      uint32_t iBin=0;
      iPtr++;

      TV1720RawChannel channel(iCh, IsZLECompressed());
      
    while(iChPtr<chSize){
	uint32_t goodData = ((GetData32()[iPtr]>>31) & 0x1);
	uint32_t nWords = (GetData32()[iPtr] & 0xFFFFF);  
	if(goodData){ 
	  std::vector<uint32_t> samples;
	  for(uint32_t iWord=0; iWord<nWords; iWord++){
	    iPtr++;
	    iChPtr++;	   
	    samples.push_back((GetData32()[iPtr]&0xFFF));
	    samples.push_back(((GetData32()[iPtr]>>16)&0xFFF));
	  }
	  channel.AddZlePulse(TV1720RawZlePulse(iBin, samples));
	}

	iBin += (nWords*2);
	iChPtr++;
	iPtr++;	

      }

      fMeasurements.push_back(channel);
    }
  }

}

void TV1720RawData::HandlUncompressedData(){

  // Skip the header.
  uint32_t iPtr=4;
  uint32_t chMask    = GetData32()[1] & 0xFF;

  int nActiveChannels=0;
  for(int iCh=0; iCh<8; iCh++){
    if(chMask & (1<<iCh))
      nActiveChannels++;
  }
  // Assume that we have readout the same number of samples for each channel.
  // The number of 32 bit double-samples per channel is then
  // N32samples = (bank size - 4)/ nActiveChannels 
  int N32samples = (GetEventSize() - 4)/ nActiveChannels;

  // Loop over channels
  for(int iCh=0; iCh<8; iCh++){
        
    if(!(chMask & (1<<iCh))){
      // Add empty channel data objects, to keep vector simple.
      TV1720RawChannel channel(iCh, IsZLECompressed());
      fMeasurements.push_back(channel);
      continue;
    }

    TV1720RawChannel channel(iCh, IsZLECompressed());
      
    for(int j = 0; j < N32samples; j++){
      uint32_t samp1 = (GetData32()[iPtr]&0xFFF);
      uint32_t samp2 = ((GetData32()[iPtr]>>16)&0xFFF);
      
      channel.AddADCSample(samp1);
      channel.AddADCSample(samp2);      
      iPtr++;
    }

    fMeasurements.push_back(channel);

  }  
}


TV1720RawData::TV1720RawData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{
  
  fGlobalHeader0 = GetData32()[0];
  fGlobalHeader1 = GetData32()[1];
  fGlobalHeader2 = GetData32()[2];
  fGlobalHeader3 = GetData32()[3];
  


  if(IsZLECompressed()){
    HandlZLECompressedData();
  }else{
    HandlUncompressedData();
  }

}

void TV1720RawData::Print(){

  std::cout << "V1720 decoder for bank " << GetName().c_str() << std::endl;
  std::cout << "Bank size: " << GetEventSize() << std::endl;
  
  if( IsZLECompressed())
    std::cout << "Data is ZLE compressed." << std::endl;
  else
    std::cout << "Data is not ZLE compressed." << std::endl;

  std::cout << "Channel Mask : " << GetChannelMask() << std::endl;
   
  std::cout << "Event counter : " << GetEventCounter() << std::endl;
  std::cout << "Trigger tag: " << GetTriggerTag() << std::endl;
  

  std::cout << "Number of channels with data: " << GetNChannels() << std::endl;
  for(int i = 0 ; i < GetNChannels(); i++){

    TV1720RawChannel channelData = GetChannelData(i);

    std::cout << "Channel: " << channelData.GetChannelNumber() << std::endl;
    
    if(IsZLECompressed()){
      std::cout << "Number of ZLE pulses: " <<  channelData.GetNZlePulses() << std::endl;
      for(int j = 0; j < channelData.GetNZlePulses(); j++){
	std::cout << "Pulse: " << j << std::endl;
	TV1720RawZlePulse pulse = channelData.GetZlePulse(j);	
	std::cout << "first sample bin: " << pulse.GetFirstBin() << std::endl;
	std::cout << "Samples ("<< pulse.GetNSamples()<<  " total): " <<std::endl;
	for(int k = 0; k < pulse.GetNSamples(); k++){
	  std::cout << pulse.GetSample(k) << ", ";
	  if(k%12 == 11) std::cout << std::endl;
	}
	std::cout << std::endl;
	
      }
    }

  }

}
