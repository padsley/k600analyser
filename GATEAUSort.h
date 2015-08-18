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

  void Initialise();
  void CountWires();
  
    //What data structures do we need here?
private :
  int TotalWiresFired;
  int **SectorWiresFired;
  vector<vector<int>> WiresFired;
  
public :
  //Setters
  void SetTotalWiresFired(int fire)	{TotalWiresFired = fire;}
  void SetSectorWiresFired(int PCB, int sector, int fire){SectorWiresFired[PCB-1][sector-1] = fire;}
  
  //Getters
  int GetTotalWiresFired() {return TotalWiresFired;}
  
  //Test functions
  bool TestWireCounter();
};

#endif

GATEAUData *GATEAUSort(int ntdc, int *TDC_channel_import, float *TDC_value_import);
