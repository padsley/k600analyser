#!/bin/bash


for COUNT in {01069,01070,01071,01072,01076,01077,01078,01082,01084,01085,01087,01088,01090,01091,01092,01093,01098,01099,01100}


do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA2/MID/run0000"$COUNT".mid"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA2/MID/run000"$COUNT".mid"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA2/MID/run00"$COUNT".mid"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA2/MID/run0"$COUNT".mid"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA2/MID/run"$COUNT".mid"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR217/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR217/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR217/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR217/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR217/sorted"$COUNT".root"
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
