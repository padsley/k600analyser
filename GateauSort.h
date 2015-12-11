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

int CalcTotalWiresFired(multiTDC mTDC);

int CalcWiresFiredPerSector(multiTDC mTDC, int plane, int sector);

int CalcGroupWiresFiredPerSector(multiTDC mTDC, int plane, int sector);

double CalcSectorSlope(multiTDC, int plane, int sector);

double CalcSectorIntercept(multiTDC, int plane, int sector);

double CalcFigureOfMerit(multiTDC, int plane, int sector);



#endif