#!/bin/bash


# PEPPERPOT =============================================================================
#
#   pp', Jan 2022
#   8 degr, 12C?, 2 new VDCs; PEPPERPOT, 
#for COUNT in {1020,1021,1022,1023,1024,1025,1026}
#
#   pp', Febr 2022
#   8 degr, 24Mg, 2 new VDCs; PEPPERPOT, 
#for COUNT in {2019,2020}
#
#   pp' , March/April 2022
#   4 degr, 24Mg, 2 new VDCs; PEPPERPOT, bad X2 preamp
#for COUNT in {3038,3039,3040}


# pd natCa, 2nd field (Exp field) =======================================================
#
#   4 degr, natCa, 2 new VDCs;    		Ca Experimental Field, March/Apr 2022
#for COUNT in {3050,3051,3053,3054,3055,3075,3076,3077}
#
#   8 degr natCa			       	Ca Experimental Field, Febr 2022
#for COUNT in {2026,2030,2031,2032,2033,2035,2036,2038,2039,2041,2042}
#
#   12 degr, natCa			       	Ca Experimental Field, Febr 2022
#for COUNT in {2049,2050,2052,2053,2055,2056}
#
#   16 degr, natCa			       	Ca Experimental Field, Febr 2022
for COUNT in {2062,2063,2065,2069,2071,2072,2074,2075,2077,2078}
#
#   20 degr, natCa			       	Ca Experimental Field, Febr 2022
#for COUNT in {2087,2088,2089,2091,2092,2093,2095,2096,2100,2101,2103,2104,2107,2108,2110}
#
#   26 degr, natCa, 2 new VDCs; bad X2 preamp	Ca Experimental Field, March/Apr 2022
#for COUNT in {3014,3015,3016,3019,3020,3021,3023,3024,3026,3027,3032}


# pd natCa, 1st field (GS field) =======================================================
#
#   4 degr, natCa, 1 new VDC & 1 old VDC  		Ca G.S. Field, March/April 2022
#for COUNT in {3089,3090}
#
#   8 degr, natCa,  1 new VDC & 1 old VDC  		Ca G.S. Field, March/April 2022
#for COUNT in {3091,3092,3093,3095,3096,3097}
#	
#   12 degr, natCa,  1 new VDC & 1 old VDC 		Ca G.S. Field, March/April 2022
#for COUNT in {3108,3109,3110,3114}
#
#   16 degr, natCa,  1 new VDC & 1 old VDC 		Ca G.S. Field, March/April 2022
#for COUNT in {3119,3120}
#
#   26 degr, natCa, 2 new VDCs, bad X2 preamp		Ca G.S. Field, March/April 2022
#for COUNT in {3013,3028,3029,3030,3031}


# pd 119Sn, GS field, which is different to Ca gs field=======================================================
#
#   4 degr, 119Sn, 2 new VDCs, bad X2 preamp  		Sn G.S. Field, March/April 2022
#for COUNT in {3046}


#----------------------------------------------------------------------------------------------------
# All Febr 2022 runs 
#for COUNT in {2026,2030,2031,2032,2033,2035,2036,2038,2039,2041,2042,2049,2050,2052,2053,2055,2056,2062,2063,2065,2069,2071,2072,2074,2075,2077,2078,2087,2088,2089,2091,2092,2093,2095,2096,2100,2101,2103,2104,2107,2108,2110}






#----------------------------------------------------------------------------------------------------

do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      #FILE="/home/neveling/DATA/MID/PR333/run0000"$COUNT".mid.gz"
      FILE="/run/media/neveling/a32d8cab-46f8-4a87-9254-323414cb72a7/DATA/MID/PR333/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      #FILE="/home/neveling/DATA/MID/PR333/run000"$COUNT".mid.gz"
      FILE="/run/media/neveling/a32d8cab-46f8-4a87-9254-323414cb72a7/DATA/MID/PR333/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      #FILE="/home/neveling/DATA/MID/PR333/run00"$COUNT".mid.gz"
      FILE="/run/media/neveling/a32d8cab-46f8-4a87-9254-323414cb72a7/DATA/MID/PR333/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      #FILE="/home/neveling/DATA/MID/PR333/run0"$COUNT".mid.gz"
      FILE="/run/media/neveling/a32d8cab-46f8-4a87-9254-323414cb72a7/DATA/MID/PR333/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      #FILE="/home/neveling/DATA/MID/PR333/run"$COUNT".mid.gz"
      FILE="/run/media/neveling/a32d8cab-46f8-4a87-9254-323414cb72a7/DATA/MID/PR333/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR333/sorted0000"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/33Srigidity/sorted0000"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/31Srigidity/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR333/sorted000"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/33Srigidity/sorted000"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/31Srigidity/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR333/sorted00"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/33Srigidity/sorted00"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/31Srigidity/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR333/sorted0"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/33Srigidity/sorted0"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/31Srigidity/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/neveling/DATA/ROOT/PR333/sorted"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/33Srigidity/sorted"$COUNT".root"
      #FILE2="/home/neveling/DATA/ROOT/PR333/31Srigidity/sorted"$COUNT".root"
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
