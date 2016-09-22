#!/bin/bash

BASE_PATH=/npdisks/data4/PR264
SORTED_PATH=/npdisks/data4/njh521/pr264/sorted


for COUNT in {1087..1087}


do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="$BASE_PATH/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="$BASE_PATH/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="$BASE_PATH/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="$BASE_PATH/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="$BASE_PATH/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="$SORTED_PATH/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="$SORTED_PATH/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="$SORTED_PATH/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="$SORTED_PATH/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="$SORTED_PATH/sorted"$COUNT".root"
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
