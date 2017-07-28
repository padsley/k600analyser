#!/bin/bash


#================================
#	PR226_12C - TEST
for COUNT in {1029,1035}

do
#====================================================
#       Directories for the data and sorted data
dataDirectory="/media/psf/DATA/PR226/data"
sortedDirectory="/media/psf/Home/Academic/Experiments/PR226_12C/sorted"

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
