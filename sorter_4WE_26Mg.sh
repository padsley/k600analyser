#!/bin/bash


export ROOTSYS=/usr/bin
export MIDASSYS=\$HOME/packages/midas/
export MIDASSYS=\$HOME/packages/midas/


RAW_DATA_DIR="/run/media/fdiel/PIVODRIVE/iThemba/raw_data/"
#for COUNT in {3088,3011,3013,3014,3025,3029,3032,3033,3038,3042,3077,3080,3084,3096,3109,3113,3118,3124,3129,3132,3138,3142,3146,3150,3156,3161,3164,3172}
#for COUNT in {3088,3013,3014,3025,3029,3032,3033,3038,3077,3080,3084,3096,3109,3113,3118,3124,3129,3132,3138,3142,3146,3161,3172}
for COUNT in {3096,3109,3113,3118,3124,3129,3132,3138,3142,3146,3161,3172}

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
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted"$COUNT".root"
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
