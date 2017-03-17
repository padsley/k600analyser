#!/bin/bash


#24Mg Collimator
for COUNT in {2227,2233,2238,2243,2250,2256,2272,2281,2290,2307,2320,2321,2322,2328,2336,2356,2362,2370,2376,2383,2398,2403,2408,2412,2426,2431,2435,2442,2446,2455,2459,2464}

#24Mg NoCollimator
#for COUNT in {2039,2043,2047,2051,2055,2098,2100,2107,2119,2163,2197,2207}


do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/experiment-data/PR251/2016/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]}
  then
      FILE="/experiment-data/PR251/2016/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/home/luna/data/PR251/2016/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/home/luna/data/PR251/2016/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/home/luna/data/PR251/2016/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted0000"$COUNT".root"
      FILE2="/home/luna/codes/PR251-analysis/sortedfiles/24Mg/Collimator/Corrected/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted000"$COUNT".root"
      FILE2="/home/luna/codes/PR251-analysis/sortedfiles/24Mg/Collimator/Corrected/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted00"$COUNT".root"
      FILE2="/home/luna/codes/PR251-analysis/sortedfiles/24Mg/Collimator/Corrected/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted0"$COUNT".root"
      FILE2="/home/luna/codes/PR251-analysis/sortedfiles/24Mg/Collimator/Corrected/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted"$COUNT".root"
      FILE2="/home/luna/codes/PR251-analysis/sortedfiles/24Mg/Collimator/Corrected/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 ]
      then
      echo $FILE
      ./analyzer -i $FILE
      mv output.root $FILE2
      fi
  fi
done
