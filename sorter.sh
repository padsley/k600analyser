#!/bin/bash

#================================
#	PR194

#   WE1
#   12C target, 0.3 mg/cm^2
#for COUNT in {1090,1114,1115,1120,1121,1125,1131,1132,1138,1156,1157}
#   Beam dropped out at approximately run 1171

#   WE2
#   12C target, 0.3 mg/cm^2
#for COUNT in {2002,2079}


#   TEST
#for COUNT in {1090,10901}

#   INCLUSIVE
#for COUNT in {1090,1114,1115,1120,1121,1125,1131,1132,1138,1156,1157,2002,2079}
#   PRUNED
#for COUNT in {1090,1114,1115,1120,1121,1125,1131,1138,1156,1157}

#for COUNT in {1090,1114}
#for COUNT in {1090,103}

#   TEMP
#for COUNT in {1090,1114,1115}
#for COUNT in {1090,10901}
#for COUNT in {1090,1114,1115,1120}
for COUNT in {1090,10901}

#   Removed run 1132 (insufficient data)

#for COUNT in {1090,109}

do

#====================================================
#       Directories for the data and sorted data
#dataDirectory="/media/psf/DATA/PR194/data"
dataDirectory="/media/psf/Home/Academic/Experiments/PR194/data"

#sortedDirectory="/media/psf/Home/Academic/Experiments/PR194/sorte"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR194/sortedNoPIDoffsets"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR194/sortedAutoTrim"
sortedDirectory="/media/psf/Home/Academic/Experiments/PR194/sortedTest"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR194/sortedNew"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR194/sortedNew2"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR194/sortedNoXMapping"


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
