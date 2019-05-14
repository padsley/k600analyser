#!/bin/bash

LOCATION0=/home/harshna/Documents/PR298/k600_data/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/24Mg/Colimator/21Aug_Sorted_26Mg_LinCal/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/24Mg/Colimator/22Aug_Sorted_152Eu_Quad_cal/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/24Mg/NoColimator/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/26Mg/Colimator/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/26Mg/NoColimator/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/154Sm/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/152Eu/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/56Co/
LOCATION1=/home/harshna/Documents/PR298/Sorted_data/
#LOCATION1=/media/harshna/Harshna&apos;s Passport Ultra/PR251 data /sorted runs/154Sm/

#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/116Sn/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/12C/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/CsCo/quad_on/
#LOCATION1=/home/harshna/Documents/PR251/Sorted_data/CsCo/quad_off/


OVERWRITE=1

#24Mg 
for COUNT in {10009,10011,10012}
#10015,10019,10023,10027,10031,10035,10039,10048,10054,10055,10059,10062,10075,10076,10077,10078,10079,10080,10081,10082,10083,10084,10085,10086,10087,10088,10089,10090,10092}

#{10093,10094,10097,10103,10107,10114,10118,10119,10123,10127,10131,10135,10139,10143,10147,10150,10158,10162,10166,10170,10174,10176,10180,10184,10188,10191,10194,10197,10202,10204,10205,10215}
#{10225,10228,10233,10237,10243,10246,10249,10250,10253,10257,10261,10265,10269,10272,10275,10281,10284,10287,10293,10296,10299,10302} 
#{20007,20012,20015,20019,20023,20027,20030,20035,20038,20042,20047,20050,20053,20058,20060,20062,20065,20068,20071,20074,20078,20081,20084,20088,20091,20093,20097,20101,20104,20107,20110,20114}


do

  echo "File number being processed: " $COUNT

  if [ $COUNT -lt 10 ]
  then
      FILE=$LOCATION0"run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE=$LOCATION0"run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE=$LOCATION0"run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE=$LOCATION0"run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE=$LOCATION0"run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2=$LOCATION1"sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2=$LOCATION1"sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2=$LOCATION1"sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2=$LOCATION1"sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2=$LOCATION1"sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 -o $OVERWRITE -eq 1 ]
      then
      ./analyzer -i $FILE
      mv output.root $FILE2   
      fi
  fi
done
echo "Go home"
