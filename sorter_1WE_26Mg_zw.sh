#!/bin/bash


export ROOTSYS=/usr/bin
export MIDASSYS=\$HOME/packages/midas/
export MIDASSYS=\$HOME/packages/midas/



#for COUNT in {1069,1070,1071,1072,1076,1077,1078,1082,1084,1085,1087,1088,1090,1091,1092,1093,1098,1099,1100}
for COUNT in {146,27,28,29,32,110,120,124,127,132,136,152,160,161,165,171,174,177,182,184,187,194,202,208,210,213,217,220}

do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/data/64Zn_pp/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/data/64Zn_pp/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/data/64Zn_pp/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/data/64Zn_pp/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/data/64Zn_pp/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted0000"$COUNT"_zw.root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted000"$COUNT"_zw.root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted00"$COUNT"_zw.root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted0"$COUNT"_zw.root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted"$COUNT"_zw.root"
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
