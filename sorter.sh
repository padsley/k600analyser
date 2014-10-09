#!/bin/bash

for COUNT in {0..1}
do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/experiment-data/PR231/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/experiment-data/PR231/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/experiment-data/PR231/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/experiment-data/PR231/run0"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/k600user/PR231/data/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/k600user/PR231/data/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/k600user/PR231/data/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/k600user/PR231/data/sorted0"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 ]
	  then
	  #./analyzer -i /k600user/PR231/data/run000"$COUNT".mid.gz
	  ./analyzer -i $FILE
	  #mv output.root /k600user/PR231/data/sorted000$COUNT.root
	  mv output.root $FILE2
      fi
  fi
done