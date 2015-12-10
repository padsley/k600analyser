#ifndef GateauSort_h
#define GateauSort_h 1


#include <TCutG.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <TROOT.h>
#include <cmath>
#include "multiTDC.h"

void GateauSort(int TDCHits, int *TDC_channel_import, float *TDC_value_import, GateauData *fatty);

int GetTotalWiresFired(multiTDC mTDC);

int GetWiresFiredPerSector(multiTDC mTDC, int plane, int sector);





#endif