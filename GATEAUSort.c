#include "GATEAUSort.h"

GATEAUData::GATEAUData()
{
}

GATEAUData::~GATEAUData()
{
}

void GATEAUData::Initialise()
{
  //initialise the GATEAUData class
  extern int NumberOfGATEAUPCB;
  extern int NumberOfSectorsPerPCB;
  SectorWiresFired = new int*[NumberOfGATEAUPCB];
  for(int i=0;i<NumberOfGATEAUPCB;i++)
    {
      SectorWiresFired[i] = new int[NumberOfSectorsPerPCB];
    }
}

bool GATEAUData::TestWireCounter()
{
  extern int NumberOfGATEAUPCB;
  extern int NumberOfSectorsPerPCB;
  int sum = 0;
  for(int i=0;i<NumberOfGATEAUPCB;i++)
    {
      for(int j=0;j<NumberOfSectorsPerPCB;j++)
	{
	  sum += SectorWiresFired[i][j];
	}
    }
  if(sum == TotalWiresFired)return true;
  else
    {
      printf("Total wires fired != the sum of sector wires fired\n return GATEAUDATA::TestWireCounter as false\n");
      return false;
    }
}

void GATEAUData::CountWires()
{

}



GATEAUData *GATEAUSort(int ntdc, int *TDC_channel_imprt, float *TDC_value_import)
{
  GATEAUData *gat = new GATEAUData();
  gat->Initialise();

  gat->CountWires();

  //gat->TestWireCounter();//Needs to be part of an 'if' function
  
  return gat;
}
