#ifndef __GATEAUData__
#define __GATEAUData__

#include <vector>
#include <stdio.h>
#include <TCutG.h>

class GATEAUData
{
public :
  GATEAUData();
  virtual ~GATEAUData();
  
    //What data structures do we need here?
private :
  int TotalWiresFired;
  int **SectorWiresFired;
  
public :
  //Setters
  void SetTotalWiresFired(int fire)	{TotalWiresFired = fire;}
  
  //Getters
  
  //Test functions
};

#endif

