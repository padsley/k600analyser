#!/bin/bash


#load odb
#odbedit -l odb_files/odb_1WE.odb
echo "save current obd file and load accurate odb file  odb_files/basic.odb"



#place link for configfile
rm config.cfg
ln -s config_files/config_zero.cfg config.cfg


#place links for lut
rm lut-x1.dat
rm lut-x2.dat
rm lut-u1.dat
rm lut-u2.dat

ln -s lut/lut-x1_zero.dat lut-x1.dat
ln -s lut/lut-x2_zero.dat lut-x2.dat
ln -s lut/lut-u1_zero.dat lut-u1.dat
ln -s lut/lut-u2_zero.dat lut-u2.dat



