#!/bin/bash

for COUNT in {1000..1101}
do
  echo $COUNT
  FILE="/media/Elements/PR226/run0"$COUNT".mid.gz"
  echo $FILE
  FILE2="/media/Elements/PR226/sorted0"$COUNT".root"
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 ]
	  then
	  ./analyzer -i /media/Elements/PR226/run0$COUNT.mid.gz
	  mv output.root /media/Elements/PR226/sorted0$COUNT.root
      fi
  fi
done