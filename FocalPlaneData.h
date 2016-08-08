#ifndef __FocalPlaneData__
#define __FocalPlaneData__ 1

/*
 * 
 * This is a header file defining the data structure for focal plane events for K600 analysis.
 * It should be used as a basis for another analysis file, an example of which should be found in the same directory.
 * 
 * Author: Philip Adsley, August 2016
 * E-mail: padsley@gmail.com
 * 
 * Please contact me if you find errors or have useful suggestions
 * 
 */

#include <vector>
#include <stdio.h>
#include <TCutG.h>

class FocalPlaneData
{
public :
  FocalPlaneData();
  virtual ~FocalPlaneData();
  
  private :
  //The actual members of the class
  
  
  
  //Should add Getters for the diagnostic values in case people want to use them to test events themselves under certain conditions
  
  void PrintEvent();
  bool TestEvent();
  void ClearEvent();
  
  unsigned int SizeOfEvent();
};

#endif
