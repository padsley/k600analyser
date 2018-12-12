#!/bin/bash

#24Mg Collimator
#for COUNT in {2227,2233,2238,2243,2250,2256,2272,2281,2290,2307,2320,2321,2322,2328,2336,2356,2362,2370,2376,2383,2398,2403,2408,2412,2426,2431,2435,2442,2446,2455,2459,2464}

#24Mg NoCollimator
#for COUNT in {2039,2043,2047,2051,2055,2098,2100,2107,2119,2163,2197,2207}

#PR298

#144Sm  
#{10016,10017,10020,10021,10024,10025,10028,10029,10032,10033,10036,10037,10040,10049,10051,10056,10057,10060,10061,10064,10065,10095,10096,10098,10100,10101,10104,10105,10108,10109,10110,10115,10116}
#{10120,10121,10124,10125,10128,10129,10132,10133,10136,10137,10140,10141,10144,10145,10146,10148,10149,10151,10152,10155}
#{10160,10161,10163,10164,10167,10168,10171,10172,10177,10178,10181,10182,10185,10186,10189,10190,10192,10193,10195,10196,10198,10199,10206,10208}
# {10216,10217,10226,10230,10231,10234,10235,10238,10239,10240,10241,10242,10244,10245,10247,10248,10251,10254,10255,10258,10259}
#{10262,10263,10266,10267,10270,10271,10273,10274,10276,10277,10278,10279,10280,10282,10283,10284,10285,10286,10288,10292,10294,10295,10297,10298,10300,10301,10303,}

#{20054,20056,20057,20059,20061,20063,20064,20066,20067,20069,20070,20072,20073,20075,20076,20079,20080,20082,20083,20085,20086,20089,20090,20092,20094,20095,20096,20098,20099,20100,20102,20103,20105,20106,20108,20109}



#24Mg
#{10009,10011,10012,10015,10019,10023,10027,10031,10035,10039,10097,10103,10107,10114}



OVERWRITE=1


for COUNT in {20054,20056,20057,20059,20061,20063,20064,20066,20067,20069,20070,20072,20073,20075,20076,20079,20080,20082,20083,20085,20086,20089,20090,20092,20094,20095,20096,20098,20099,20100,20102,20103,20105,20106,20108,20109}

do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/experiment-online-data-smstore3/PR298/run0000"$COUNT".mid.gz"
     #  FILE="/vicepda/althome/k600user/PR298/data/run0000"$COUNT".mid.gz"

  elif [ $COUNT -lt 100 ]
  then
      FILE="/experiment-online-data-smstore3/PR298/run000"$COUNT".mid.gz"
     #  FILE="/vicepda/althome/k600user/PR298/data/run000"$COUNT".mid.gz"

  elif [ $COUNT -lt 1000 ]
  then
      FILE="/experiment-online-data-smstore3/PR298/run00"$COUNT".mid.gz"
     #  FILE="/vicepda/althome/k600user/PR298/data/run00"$COUNT".mid.gz"

  elif [ $COUNT -lt 10000 ]
  then
      FILE="/experiment-online-data-smstore3/PR298/run0"$COUNT".mid.gz"
     #  FILE="/vicepda/althome/k600user/PR298/data/run0"$COUNT".mid.gz"

  elif [ $COUNT -lt 100000 ]
  then
      FILE="/experiment-online-data-smstore3/PR298/run"$COUNT".mid.gz"
     #  FILE="/vicepda/althome/k600user/PR298/data/run"$COUNT".mid.gz"

  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted0000"$COUNT".root"
      FILE2="/vicepda/althome/k600user/PR298/online/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted000"$COUNT".root"
      FILE2="/vicepda/althome/k600user/PR298/online/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted00"$COUNT".root"
      FILE2="/vicepda/althome/k600user/PR298/online/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted0"$COUNT".root"
      FILE2="/vicepda/althome/k600user/PR298/online/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
#      FILE2="/vicepda/althome/k600user/PR251/2016/sortedfiles/sorted"$COUNT".root"
      FILE2="/vicepda/althome/k600user/PR298/online/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo $FILE2
  if [ -e $FILE ]
  then
      if [ ! -e $FILE2 -o $OVERWRITE -eq 1 ]
      then
      echo $FILE
      ./analyzer -i $FILE
      mv output.root $FILE2
      fi
  fi
done
