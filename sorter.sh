#!/bin/bash

#for COUNT in {1069,1070,1071,1072,1076,1077,1078,1082,1084,1085,1087,1088,1090,1091,1092,1093,1098,1099,1100}
for COUNT in {61,62}
 # {48,49,50,51,52,53,54,57,58,59,61,62,63,64,65,66,77,78,79,80,81,82,83,84,85,86,87,88,89,90,95,96,97,98,99,100,101,102,103,104,105,106,107,110,112,115,116,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,135,140,141,142,143,144,145,146,147,151,152,153,154,208,209,210,211,212,213,214,215,216,217,218,219,220,221,223,224,225,226,228,232,233,234,235,236,2237,238,239,240,250,251,252,254,257,258,260,261,263,266,268,269,271,273,275,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,320,321,322,323,324,325}



do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/experiment-online-data-smstore1/PR315/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/experiment-online-data-smstore1/PR315/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/experiment-online-data-smstore1/PR315/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/experiment-online-data-smstore1/PR315/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/experiment-online-data-smstore1/PR315/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
#      FILE2="/vicepda/althome/k600user/PR315//sorted/sorted0000"$COUNT".root"
      FILE2="../sorted/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
#      FILE2="/vicepda/althome/k600user/PR315//sorted/sorted000"$COUNT".root"
      FILE2="../sorted/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR315//sorted/sorted00"$COUNT".root"
      FILE2="../sorted/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR315//sorted/sorted0"$COUNT".root"
      FILE2="../sorted/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR315//sorted/sorted"$COUNT".root"
      FILE2="../sorted/sorted"$COUNT".root"
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
