#!/bin/bash

#================================
#	PR240 - preWE1 (CAKE Calibration)
#for COUNT in {108,109,110,111}

#for COUNT in {71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,112}


#================================
#	PR240 - WE1
#for COUNT in {479,484,488,490,492,496,499,502,505,539,543,549,551,553,554,557,561}
#for COUNT in {479,484,488,490,492,496,499,502,505}
#for COUNT in {539,543,549,551,553,554,557,561}


#================================
#	PR240 - WE2
#for COUNT in {1016,1020,1023,1027,1029,1033,1035,1037,1038,1054,1058,1067,1070,1074,1081,1082,1084,1090,1091,1093,1095,1097,1098,1104,1124,1127,1137,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262,1265,1270,1278,1280,1287}

#================================
#	PR240 - WE1 && WE2
#or COUNT in {479,484,488,490,492,496,499,502,505,539,543,549,551,553,554,557,561,1016,1020,1023,1027,1029,1033,1035,1037,1038,1054,1058,1067,1070,1074,1081,1082,1084,1090,1091,1093,1095,1097,1098,1104,1124,1127,1137,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262,1265,1270,1278,1280,1287}

#		INCLUSIVE, PRUNED
for COUNT in {479,484,488,490,492,496,499,502,505,1016,1020,1023,1027,1029,1033,1035,1037,1038,1054,1058,1067,1070,1074,1081,1082,1084,1090,1091,1093,1095,1097,1098,1104,1124,1127,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262}

#       To Complete
#for COUNT in {1020,1023,1027,1029,1033,1035,1037,1038,1054,1058,1067,1070,1074,1081,1082,1084,1090,1091,1093,1095,1097,1098,1104,1124,1127,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262,1280,1287}

#================================
#	PR240 - preWE1 && WE1 && WE2
#for COUNT in {108,109,110,111,479,484,488,490,492,496,499,502,505,539,543,549,551,553,554,557,561,1016,1020,1023,1027,1029,1033,1035,1037,1038,1054,1058,1067,1070,1074,1081,1082,1084,1090,1091,1093,1095,1097,1098,1104,1124,1127,1137,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262,1265,1270,1278,1280,1287}

#================================
#	PR240 - WE1 && WE2
#for COUNT in {479,484,488,490,492,496,499,502,505,539,543,549,551,553,554,557,561,1016,1020,1023,1027,1029,1033,1035,1037,1038,1054,1058,1067,1070,1074,1081,1082,1084,1090,1091,1093,1095,1097,1098,1104,1124,1127,1137,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262,1265,1270,1278,1280,1287}


#====================================================
#	PR240 - WE1 && WE2 and CAKE calibration runs

#for COUNT in {72,74,85,86,479,484,488,490,492,496,499,502,505,539,543,549,551,553,554,557,561,1016,1020,1023,1027,1029,1033,1035,1037,1038,1054,1058,1067,1070,1074,1081,1082,1084,1090,1091,1093,1095,1097,1098,1104,1124,1127,1137,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262,1265,1270,1278,1280,1287}

#for COUNT in {1016,101}

#for COUNT in {479,4799}
#for COUNT in {72,72232}

#================================
#	PR240 - TEST
#for COUNT in {479,484,488}
#for COUNT in {1265,1270,1278,1280,1287}

#for COUNT in {479,47999}

#   Removed run 1137 (bad resolution)
#   Removed run 1265 (insufficient data)
#   Removed run 1280, 1287

#for COUNT in {1097,1098,1104,1124,1127,1137,1157,1160,1186,1189,1206,1208,1244,1256,1258,1262,1265,1270,1278,1280,1287}


do

#====================================================
#       Directories for the data and sorted data
dataDirectory="/media/psf/Home/Academic/Experiments/PR240/data"
#dataDirectory="/media/psf/DATA/PR240/data"

#sortedDirectory="/media/psf/Home/Academic/Experiments/PR240/sorted"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR240/sortedTest"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR240/sortedNoPIDoffsets"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR240/sortedAutoTrim"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR240/sortedCAKECal"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR240/sortedNew"
sortedDirectory="/media/psf/Home/Academic/Experiments/PR240/sortedNew2"


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
