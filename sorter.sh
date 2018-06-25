#!/bin/bash


#================
#       12C
#for COUNT in {1029,1045,1046,1062,1080,1089,1090,1091,1092,1093,1094,1096,1099,1100,1101,1102,1106}
for COUNT in {1029,1045,1046,1062,1080,1089,1090,1091,1092,1093,1094,1096,1099,1100,1101,1102}

#================
#       Li2CO3
#for COUNT in {1023,1024,1025,1026,1030,1032,1033,1034,1036,1039,1041,1043,1044,1047,1048,1052,1053,1056,1057,1058,1059,1060,1070,1071,1073,1075,1078,1079,1084,1085,1086,1087}

#====================
#       Ancillary
#for COUNT in {1027,1028,1029,1031,1035,1037,1038,1045,1046,1049,1050,1051,1054,1055,1061,1062,1065,1067,1068,1069,1072,1080,1082,1083,1089,1090,1091,1092,1093,1094,1096,1099,1100,1101,1102,1108}

#================
#       Junk
#for COUNT in {1027,1037,1038,1042,1050,1054,1061,1064,1072,1081,1082,1083}

#================
#       Mylar
#for COUNT in {1049,1051,1065,1068}

#================
#       MT
#for COUNT in {1028,1031,1055,1067,1069}

#================
#       Total
#for COUNT in {1029,1045,1046,1062,1080,1089,1090,1091,1092,1093,1094,1096,1099,1100,1101,1102,1023,1024,1025,1026,1030,1032,1033,1034,1036,1039,1041,1043,1044,1047,1048,1052,1053,1056,1057,1058,1059,1060,1070,1071,1073,1075,1078,1079,1084,1085,1086,1087,1049,1051,1065,1068,1028,1031,1055,1067,1069,1027,1037,1038,1042,1050,1054,1061,1064,1072,1081,1082,1083}
#To be completed for sortedNoPIDoffsets
#for COUNT in {1048,1052,1053,1056,1057,1058,1059,1060,1070,1071,1073,1075,1078,1079,1084,1085,1086,1087,1027,1037,1038,1042,1050,1054,1061,1064,1072,1081,1082,1083,1049,1051,1065,1068,1028,1031,1055,1067,1069}


#for COUNT in {1029,1045}


do

#====================================================
#       Directories for the data and sorted data
dataDirectory="/media/psf/DATA/PR226/data"

#sortedDirectory="/media/psf/Home/Academic/Experiments/PR226_2017/sorted"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR226_2017/sortedNoPIDoffsets"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR226_2017/sortedTest"
sortedDirectory="/media/psf/Home/Academic/Experiments/PR226_2017/sortedNew"


  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE=$dataDirectory"/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE=$dataDirectory"/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE=$dataDirectory"/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE=$dataDirectory"/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE=$dataDirectory"/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2=$sortedDirectory"/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2=$sortedDirectory"/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2=$sortedDirectory"/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2=$sortedDirectory"/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2=$sortedDirectory"/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
  
      echo $FILE
      ./analyzer -i $FILE
      mv output.root $FILE2
      
      
#      if [ ! -e $FILE2 ]
#      then
#      echo $FILE
#      ./analyzer -i $FILE
#      mv output.root $FILE2
#      cp output.root $FILE2
#      fi
  fi
done
