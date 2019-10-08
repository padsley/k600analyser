#!/bin/bash

#PR236b, 0 degr, 42Ca
for COUNT in {177,178,180}


do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/home/neveling/DATA/MID/PR236b/run0000"$COUNT".mid.gz"
      #FILE="/run/media/neveling/Transcend/DATA/MID/PR236b/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/home/neveling/DATA/MID/PR236b/run000"$COUNT".mid.gz"
      #FILE="/run/media/neveling/Transcend/DATA/MID/PR236b/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/home/neveling/DATA/MID/PR236b/run00"$COUNT".mid.gz"
      #FILE="/run/media/neveling/Transcend/DATA/MID/PR236b/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/home/neveling/DATA/MID/PR236b/run0"$COUNT".mid.gz"
      #FILE="/run/media/neveling/Transcend/DATA/MID/PR236b/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/home/neveling/DATA/MID/PR236b/run"$COUNT".mid.gz"
      #FILE="/run/media/neveling/Transcend/DATA/MID/PR236b/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE

  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR236b/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR236b/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR236b/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR236b/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR236b/sorted"$COUNT".root"
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










