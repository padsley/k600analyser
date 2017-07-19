/*
 * =====================================================================================
 *
 *       Filename:  TSIS3302.cxx
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/10/2015 14:30:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lee Pool (LCP), lcpool@tlabs.ac.za
 *        Company:  Ithemba Labs.
 *
 * =====================================================================================
 */

#include "TSIS3302.hxx"

/// Constructor
TSIS3302Data::TSIS3302Data(int bklen, int bktype, const char* name, void *pdata):
  TGenericData(bklen, bktype, name, pdata)
{
  
  fAdc_header_word = 0;
  fAdc_trailer_word = 0;
	int floop_start = 2;
	int floop_end = GetSize();

	fAdc_header_word = GetData32()[0];
	fAdc_timestamp_lower = GetData32()[1];

	fAdc_trailer_word = GetData32()[floop_end-1];
	fAdc_use_word = GetData32()[floop_end-2];
  	fAdc_energy_first_value = GetData32()[floop_end-3];
	fAdc_energy_max_value = GetData32()[floop_end-4];
	floop_end = floop_end - 4;

  for(int i = floop_start; i < floop_end; i++){
    
    uint32_t word = GetData32()[i];
    fMeasurements.push_back(VADCMeasurement(fAdc_header_word,word));

  }//For    

}//Constructor

void TSIS3302Data::Print(){

  std::cout << "SIS3302 decoder for bank " << GetName().c_str() << std::endl;
  
  std::cout << "TriggerFlag = " << GetTriggerFlag() << ", Fast Trigger Counter =  " << GetFastTriggerCounter() 
	    	    << ", Get max energy value = " << GetMaxEnergyValue() << ", Retrigger flag = "
	    << GetRetriggerFlag() << ", Pile-up Flag " << GetPilupFlag() << std::endl;
  
	for(int i = 0; i < fMeasurements.size(); i++){
    std::cout << " meas = " << fMeasurements[i].GetRawMeasurement();
    if((i-2)%3 == 0) std::cout << std::endl;
  }
  std::cout << std::endl;
}//Printf






