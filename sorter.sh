#!/bin/bash

for COUNT in {1089..1101}
do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/media/Elements/PR226/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/media/Elements/PR226/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/media/Elements/PR226/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/media/Elements/PR226/run0"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/media/Elements/PR226/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/media/Elements/PR226/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/media/Elements/PR226/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/media/Elements/PR226/sorted0"$COUNT".root"
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


