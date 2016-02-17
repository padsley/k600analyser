#include "TL2249Data.hxx"


/// Constructor
TL2249Data::TL2249Data(int bklen, int bktype, const char* name, void *pdata):
  TGenericData(bklen, bktype, name, pdata)
{
  
  for(int i = 0; i < GetSize(); i++){
    
    uint32_t word = GetData16()[i];
    fMeasurements.push_back(LADCMeasurement(i,word));

  }    

}

void TL2249Data::Print(){

  std::cout << "L2249 decoder for bank " << GetName().c_str() << std::endl;
  
  for(int i = 0; i < (int)fMeasurements.size(); i++){
    std::cout << "chan " << i  << " meas = " << fMeasurements[i].GetMeasurement() << " " ;
  }
  std::cout << std::endl;



}
