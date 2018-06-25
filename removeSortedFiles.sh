#!/bin/bash


#================
for COUNT in {22077,22078,22080,22082,22083,22088,22089,22090,22091,22092,22093,22094}



do
#====================================================
#       Directories for the data and sorted data
#dataDirectory="/media/psf/DATA/PR226/data"

sortedDirectory="/media/psf/Home/Academic/Experiments/PR166_2011/sorted"


  echo $COUNT

  if [ $COUNT -lt 10 ]
  then
      FILE=$sortedDirectory"/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE=$sortedDirectory"/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE=$sortedDirectory"/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE=$sortedDirectory"/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE=$sortedDirectory"/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ -e $FILE ]
  then

      rm $FILE

  fi
done
