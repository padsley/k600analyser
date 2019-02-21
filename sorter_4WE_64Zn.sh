#!/bin/bash


export ROOTSYS=/usr/bin
export MIDASSYS=\$HOME/packages/midas/
export MIDASSYS=\$HOME/packages/midas/


RAW_DATA_DIR="/run/media/fdiel/PIVODRIVE/iThemba/raw_data/"
#for COUNT in {1069,1070,1071,1072,1076,1077,1078,1082,1084,1085,1087,1088,1090,1091,1092,1093,1098,1099,1100}
#for COUNT in {2109,2081,2084,2092,2094,2097,2101,2104,2105,2107,2112,2119,2120,2121,2123,2126,2129,2131,2132,2133,2137,2138,2142,2143,2145,2146,2148,2149,2153,2154,2157,2160,2161,2163,2164,2166,2167,2168,2171,2172}
#for COUNT in {3089,3020,3023,3026,3030,3031,3034,3040,3078,3079,3081,3085,3087,3090,3091,3097,3103,3112,3114,3115,3122,3123,3125,3133,3136,3139,3140,3143,3144,3147,3148,3151,3152,3155,3157,3159,3162,3165,3167,3168,3169,3173}
for COUNT in {3097,3103,3112,3114,3115,3122,3123,3125,3133,3136,3139,3140,3143,3144,3147,3148,3151,3152,3155,3157,3159,3162,3165,3167,3168,3173}
#for COUNT in {3097,}
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
      FILE2="/home/fdiel/Analysis_local/iThemba/root_64Zn/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_64Zn/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_64Zn/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_64Zn/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_64Zn/sorted"$COUNT".root"
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
