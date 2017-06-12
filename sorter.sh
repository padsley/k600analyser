#!/bin/bash



#for COUNT in {32206,32207,32208,32211,32212,32216,32217,32219,32220,32224,32225,32226,32231,32232,32234,32235,32238}
#for COUNT in {32199,32213,32222,32230,32237}
#for COUNT in {32206,32207,32208,32212,32216}
for COUNT in {32206,32207}

do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA-PR184/MID/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA-PR184/MID/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA-PR184/MID/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA-PR184/MID/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/media/lindsay/Elements/Backups/Linux/DATA-PR184/MID/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR184/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR184/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR184/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR184/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR184/sorted"$COUNT".root"
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
