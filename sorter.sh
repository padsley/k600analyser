#!/bin/bash



#for COUNT in {1069,1070,1071,1072,1076,1077,1078,1082,1084,1085,1087,1088,1090,1091,1092,1093,1098,1099,1100}
for COUNT in {1041,1045,1051,1084,1090,1094,1099,1105,1110,1112,1117,1123,1128,1133,1138,1144,1149}

do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/home/sunday/DATA/MID/PR260/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/home/sunday/DATA/MID/PR260/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/home/sunday/DATA/MID/PR260/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/home/sunday/DATA/MID/PR260/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/home/sunday/DATA/MID/PR260/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR260/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR260/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR260/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR260/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR260/sorted"$COUNT".root"
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
