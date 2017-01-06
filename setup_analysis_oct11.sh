#!/bin/bash

echo For experiment PR170: October 2011 makeup beamtime:
echo Copy the correct config, CableLength and lut files into the working files...
cp config-oct11.cfg config.cfg
cp CableLength-oct11-pr170.dat CableLength.dat
cp lut-u2-oct11-pr170.dat lut-u2.dat
cp lut-x1-oct11-pr170.dat lut-x1.dat
cp lut-x2-oct11-pr170.dat lut-x2.dat
echo Copying done...

echo Also copy the proper scaler.c file. Remember to ...make...
cp scaler-pr170-oct11.c scaler.c 

echo DO NOT FORGET TO MAKE SURE THE CORRECT ODB FILE IS LOADED!
echo ALSO DO NOT FORGET TO RECOMPILE BEFORE COMMENCING ANALYSIS

