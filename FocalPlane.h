#ifndef FOCALPLANE_H
#define FOCALPLANE_H 1

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>

void printevent(Int_t hits, Int_t wire[], Int_t time[]);
void printevent2(Int_t hits, Int_t wire[], Double_t dist[]);
void printv1190data(DWORD *data, int tdcmodulecounter, int i);

void read_misalignment(int *_matime, char *fileName); 
void read_lut(float lutx[], int offset, char *fileName);
void read_cable(float cable[], char* fileName); 

void setupchannel2wireXoldXold(unsigned int chan2wire[]);
void setupchannel2wireXoldXU(unsigned int chan2wire[]);
void setupchannel2wireXUXU(unsigned int chan2wire[]) ;
void setupchannel2wireXUXold(unsigned int chan2wire[]);
void setupchannel2wire(unsigned int chan2wire[]) ;

void getRefTimes(int time[], int ntdc, DWORD ptdc[]);
void sortTDCs(Int_t hits, Int_t wire[], Int_t time[]);
void getRidOfHotWire(Int_t *_hits,Int_t *_hits2, Int_t wire[], Int_t time[]);
void fixDoubleHit(Double_t dd[],Int_t wire[], Int_t badwire, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_last);
void fixZ(Double_t dd[],Int_t wire[], Int_t *_flag1, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_last);
void fixW(Double_t dd[],Int_t wire[], Int_t *_flag1, Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_last);
void TryToImproveFit(Double_t dd[],Int_t wire[], Int_t *_wire_num, Int_t *_wireID_min, Int_t *_wireID_first, Int_t *_wireID_last, Double_t *_Xttif, Double_t *_Thttif, Double_t *_chisqttif, Double_t *_attif, Double_t *_bttif);
void raytrace(Double_t dd[],Int_t wire[],Double_t *_X,Double_t *_Th,Double_t *_chisq,Int_t wire_num,Double_t res[],Int_t *_flag, Int_t chamber, Double_t wused[] , Double_t dused[], Int_t *_wire_used, Int_t *_badwirecount, Int_t *_multmin, Int_t *_chisqminimization);


void CalcCorrX(Double_t X, Double_t Y, Double_t ThetaSCAT, Double_t *Xcorr);
double CalcQBrho(double Xcorr);
double CalcTfromXcorr(double Xcorr, double mass);
double CalcTfromRigidity(double rig, double mass);
double CalcTfromP(double p, double mass);
double CalcExDirect(double Xcorr);
double CalcEx(double Xcorr);
double CorrectEx(double mEx);

double CalcYFP(double x, double u, double thFP);
double CalcThetaScat(double X1, double ThFP);
double CalcPhiScat(double X1, double ThFP, double Y1);
double CalcTheta(double X1, double ThFP, double Y1);
double CalcThetaFP(double X1, double X2);
double CalcPhiFP(double X1, double Y1, double X2, double Y2,  double thFP);


#endif

