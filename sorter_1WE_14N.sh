#!/bin/bash


export ROOTSYS=/usr/bin
export MIDASSYS=\$HOME/packages/midas/
export MIDASSYS=\$HOME/packages/midas/



RAW_DATA_DIR="/run/media/fdiel/PIVODRIVE/iThemba/raw_data/"

#for COUNT in {1069,1070,1071,1072,1076,1077,1078,1082,1084,1085,1087,1088,1090,1091,1092,1093,1098,1099,1100}
for COUNT in {125,214}
do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE=$RAW_DATA_DIR"run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE=$RAW_DATA_DIR"run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE=$RAW_DATA_DIR"run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE=$RAW_DATA_DIR"run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE=$RAW_DATA_DIR"run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_14N/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_14N/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_14N/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_14N/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_14N/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 ]
      then
      echo $FILE
#       ./analyzer -i /media/Elements/PR226/run000"$COUNT".mid.gz
      ./analyzer -i $FILE
#       mv output.root /media/Elements/PR226/sorted000$COUNT.root
      mv output.root $FILE2
      fi
  fi
done
