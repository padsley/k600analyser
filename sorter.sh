#!/bin/bash


for COUNT in {1102..1104}
do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="./data/midasFiles/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="./data/midasFiles/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="./data/midasFiles/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="./data/midasFiles/run0"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="./data/rootFiles/wknd_1/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="./data/rootFiles/wknd_1/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="./data/rootFiles/wknd_1/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="./data/rootFiles/wknd_1/sorted0"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
    echo $FILE
    # Execute analyzer on data $FILE
    ./analyzer -i $FILE
    # Move output root file to another directory with user defined name 
    mv output.root $FILE2
    fi
done


