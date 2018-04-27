#!/bin/bash


export ROOTSYS=/usr/bin
export MIDASSYS=\$HOME/packages/midas/
export MIDASSYS=\$HOME/packages/midas/



#for COUNT in {1069,1070,1071,1072,1076,1077,1078,1082,1084,1085,1087,1088,1090,1091,1092,1093,1098,1099,1100}
#for COUNT in {2109,2081,2084,2092,2094,2097,2101,2104,2105,2107,2112,2119,2120,2121,2123,2126,2129,2131,2132,2133,2137,2138,2142,2143,2145,2146,2148,2149,2153,2154,2157,2160,2161,2163,2164,2166,2167,2168,2171,2172}
for COUNT in {2062,2056,2057,2058,2059,2060,2061}
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
      FILE2="/home/fdiel/Analysis_local/iThemba/root_Au/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_Au/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_Au/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_Au/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_Au/sorted"$COUNT".root"
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
