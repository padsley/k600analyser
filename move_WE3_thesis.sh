#!/bin/bash


#load odb
#odbedit -l odb_files/odb_1WE.odb
echo "THIS CONFIGURATION IS USED TO PRODUCE PLOTS FOR THE THESIS ONLY!"
echo "save current obd file and load accurate odb file  odb_files/odb_3WE_thesis.odb"
echo "!!! CAUTION!!! USING X1X2"


#place link for configfile
rm config.cfg
ln -s config_files/config_3WE_thesis.cfg config.cfg

#place link for calblelengtfile
rm CableLength.dat
ln -s CableLength/CableLength_3WE_thesis.dat CableLength.dat



#place links for lut
rm lut-x1.dat
rm lut-x2.dat
rm lut-u1.dat
rm lut-u2.dat

ln -s lut/lut-x1_WE3_thesis.dat lut-x1.dat
ln -s lut/lut-x2_WE3_thesis.dat lut-x2.dat
ln -s lut/lut-u1_WE3_thesis.dat lut-u1.dat
ln -s lut/lut-u2_WE3_thesis.dat lut-u2.dat
