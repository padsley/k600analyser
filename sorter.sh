#!/bin/bash

# PR292 - 58Ni(p,p'g), 24Mg, 12C
  
#for COUNT in {4032,4034,4035,4036,4037,4039,4040,4041,4043,4044,4045,4047,4048,4049,4051}
for COUNT in {4037,4045}

do
  echo "inside DO loop, with runnr = " $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/media/lindsay/Elements/PR292/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/media/lindsay/Elements/PR292/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/media/lindsay/Elements/PR292/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/media/lindsay/Elements/PR292/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/media/lindsay/Elements/PR292/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo "midas file name: " $FILE

  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR292/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR292/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR292/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR292/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR292/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo "and desired rootfile name : " $FILE2

  if [ -e $FILE ]
  then
      echo "entered IF loop to start analyzer"

      if [ ! -e $FILE2 ]
      then
      echo $FILE
      echo "./analyzer -i "$FILE
      ./analyzer -i $FILE
      mv output.root $FILE2
      fi
  fi
done
