#!/bin/bash

LOCATION0=/home/luna/data/PR251/2016/
#LOCATION1=/home/luna/codes/PR251-analysis/sortedfiles/24Mg/Collimator/Corrected/Calibrated/
LOCATION1=/home/luna/codes/PR251-analysis/sortedfiles/26Mg/Collimator/Corrected/Calibrated/

OVERWRITE=1

#24Mg Collimator
#for COUNT in {2227,2233,2238,2243,2250,2256,2272,2281,2290,2307,2320,2321,2322,2328,2336,2356,2362,2370,2376,2383,2398,2403,2408,2412,2426,2431,2435,2442,2446,2450,2455,2459,2464}
#26Mg Collimator
#for COUNT in {2221,2222,2225,2232,2237,2242,2249,2255,2267,2271,2276,2280,2285,2294,2304,2308,2309,2323,2329,2337,2348,2357,2363,2371,2384} #all
for COUNT in {2232,2237,2242,2249,2255,2267,2271,2276,2280,2285,2294,2304,2308,2309,2323,2329,2337,2348,2357,2363,2371,2384}

do

  echo "File number being processed: " $COUNT

  if [ $COUNT -lt 10 ]
  then
      FILE=$LOCATION0"run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE=$LOCATION0"run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE=$LOCATION0"run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE=$LOCATION0"run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE=$LOCATION0"run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2=$LOCATION1"sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2=$LOCATION1"sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2=$LOCATION1"sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2=$LOCATION1"sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2=$LOCATION1"sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 -o $OVERWRITE -eq 1 ]
      then
      ./analyzer -i $FILE
      mv output.root $FILE2   
      fi
  fi
done
echo "Go home"
