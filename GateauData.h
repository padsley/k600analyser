#ifndef __GateauData__
#define __GateauData__ 1

/*
 * 
 * This is a header file defining the data structure for silicon events for K600 analysis.
 * It should be used as a basis for another analysis file, an example of which should be found in the same directory.
 * 
 * Author: Philip Adsley, June 2014
 * E-mail: padsley@gmail.com
 * 
 * Please contact me if you find errors or have useful suggestions
 * 
 */
#include <vector>
#include <stdio.h>
#include <TCutG.h>

class GateauData
{
public :
  GateauData();
  virtual ~GateauData();
  
  private :
  //The actual members of the class
  //int SiliconHits;
  
  int WiresFired; //Total number of wires that fired per sector
  
  int SectorWiresFired[2][5]; //Numbers of wires that fired per sector
  
  int ListWiresFired[2][5][16]; //Fairly self-explanatory
  
  int ValuesWiresFired[2][5][16]; //Same as above
  
public :
  //Setters - set the physical values for each physical event
  void SetWiresFired(int wires) {WiresFired = wires;}
  void SetWiresFiredForSector(int plane, int sector, int wires) {SectorWiresFired[plane][sector] = wires;}
  
  void PrintEvent();
  bool TestEvent();
  void ClearEvent();
  
  unsigned int SizeOfEvent();
};

#endif
