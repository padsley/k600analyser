#!/bin/bash



#for COUNT in {1069,1070,1071,1072,1076,1077,1078,1082,1084,1085,1087,1088,1090,1091,1092,1093,1098,1099,1100}
for COUNT in {2030,2037,2041,2042,2057,2063,2081,2084,2088,2092,2096,2100,2104,2123,2128,2131,2136,2141,2146,2151,2156,2161,2167} # 26Mg PR236
#for COUNT in {1041,1045,1051,1084,1090,1094,1099,1105,1110,1112,1117,1123,1128,1133,1138,1144,1149} ### These are 24Mg runs PR260
#for COUNT in {1042,1047,1053,1085,1091,1096,1101,1106,1113,1119,1125,1130,1135,1140,1146,1151} ### These are 48Ca runs PR260
#for COUNT in {1043,1048,1054,1086,1087,1092,1097,1102,1107,1114,1115,1120,1126,1131,1136,1141,1142,1147} ### These are 44Ca runs PR260
#for COUNT in {1044,1049,1050,1089,1093,1098,1103,1108,1116,1122,1127,1132,1137,1143,1148} ### These are 42Ca runs PR260

do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
    FILE="/home/sunday/DATA/MID/PR236/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
     FILE="/home/sunday/DATA/MID/PR236/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
     FILE="/home/sunday/DATA/MID/PR236/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
     FILE="/home/sunday/DATA/MID/PR236/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
     FILE="/home/sunday/DATA/MID/PR236/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR236/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR236/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR236/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR236/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/sunday/DATA/ROOT/PR236/sorted"$COUNT".root"
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
