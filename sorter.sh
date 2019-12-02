#!/bin/bash

#================================
#	PR251
#for COUNT in {2040,2044,2048,2052,2056,2075,2076,2079,2097,2102,2109}

#   INCLUSIVE, PRUNED
for COUNT in {2040,2044,2048,2052,2056,2075,2076,20977,2102,2109}

#for COUNT in {2097,2102,2109}


#   Removed runs: 2079 (misalignment)

#for COUNT in {2040,20401}
#for COUNT in {2040,204}

do

#====================================================
#       Directories for the data and sorted data
#dataDirectory="/media/psf/Home/Desktop/Data/PR251"
dataDirectory="/media/psf/DATA/PR251/data"

#sortedDirectory="/media/psf/Home/Academic/Experiments/PR251/sorted"
sortedDirectory="/media/psf/Home/Academic/Experiments/PR251/sortedNew"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR251/sortedTest"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR251/sortedAutoTrim"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR251/sortedNew"


  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE=$dataDirectory"/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE=$dataDirectory"/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE=$dataDirectory"/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE=$dataDirectory"/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE=$dataDirectory"/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2=$sortedDirectory"/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2=$sortedDirectory"/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2=$sortedDirectory"/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2=$sortedDirectory"/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2=$sortedDirectory"/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
  
      echo $FILE
      ./analyzer -i $FILE
      mv output.root $FILE2
      
      
#      if [ ! -e $FILE2 ]
#      then
#      echo $FILE
#      ./analyzer -i $FILE
#      mv output.root $FILE2
#      cp output.root $FILE2
#      fi
  fi
done
