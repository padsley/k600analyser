#!/bin/bash

# Weekend 1 - Remember to change file locations as well.  
#for COUNT in {1093,1095,1097,1100,1101,1103,1104,1106,1107,1108,1110,1111,1112,1114,1116,1117,1118,1120,1121,1123,1126,1127,1128}
#for COUNT in {1093,1100,1104,1108,1112,1117,1123,1128}
#for COUNT in {1102,1093}

# Weekend 2 24Mg
#for COUNT in {2155,2155}

#Weekend 2 26Mg (all)
#for COUNT in {2123,2128,2131,2136,2141,2146,2151,2156,2161,2167}

#Weekend 2 Mylar 
#for COUNT in {2122,2132,2137,2142,2147,2152}

# Weekend 2 40Ca
#for COUNT in {2125,2126,2130,2134,2135,2139,2140,2144,2145,2149,2150,2154}

# Weekend 2 90Zr
#for COUNT in {2158,2160,2162,2166,2168,2169}

# PR236 - Weekend 3 - 13C
#for COUNT in {3078,3110,3123,3132,3141}

# PR236 - Weekend 3 - 24Mg
for COUNT in {3077,3081,3086,3090,3094,3098,3102,3106,3111,3119,3125,3129,3133,3138,3142,3146,3151,3155,3159,3163,3167,3171,3177,3181,3185,3188,3191,3195} 

# PR236 - Weekend 3 - 26Mg
#for COUNT in {3213,3214,3216,3227,3228,3229,3233,3234,3235,3236}

# PR236 - Weekend 3 - 27Al
#for COUNT in {3179,3187}

# PR236 - Weekend 3 - 28Si
#for COUNT in {3150,3154,3158,3162,3166,3170,3175}  

# PR236 - Weekend 3 - 58Ni
#for COUNT in {3084,3092,3100,3109,3116,3128,3137,3145,3183}

# PR236 - Weekend 3 - 120Sn
#for COUNT in {3178,3182,3186,3190,3193,3194,3196}

# PR236 - Weekend 3 - 208Pb
#for COUNT in {3079,3080,3085,3088,3089,3093,3096,3097,3101,3104,3105,3115,3117,3121,3127,3131,3136,3140,3144}

# PR236 - Weekend 3 - All
#for COUNT in {3138,3077,3078,3079,3080,3081,3084,3085,3086,3088,3089,3090,3092,3093,3094,3096,3097,3098,3100,3101,3102,3104,3105,3106,3109,3110,3111,3115,3116,3117,3119,3121,3123,3125,3127,3128,3129,3131,3132,3133,3136,3137,3140,3141,3142,3144,3145,3146,3150,3151,3154,3155,3158,3159,3162,3163,3166,3167,3170,3171,3175,3177,3178,3179,3181,3182,3183,3185,3186,3187,3188,3190,3191,3193,3194,3195,3196};

do
  echo "inside DO loop, with runnr = " $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/media/lindsay/Elements/PR236/Weekend-3/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/media/lindsay/Elements/PR236//Weekend-3/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/media/lindsay/Elements/PR236/Weekend-3/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/media/lindsay/Elements/PR236/Weekend-3/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/media/lindsay/Elements/PR236/Weekend-3/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo "midas file name: " $FILE

  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR236/Weekend-3/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR236/Weekend-3/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR236/Weekend-3/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR236/Weekend-3/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/lindsay/K600/DATA/PR236/Weekend-3/sorted"$COUNT".root"
  else
      echo "This might not work"
  fi
  echo "and desired rootfile name : " $FILE2

  if [ -e $FILE ]
  then
      echo "entered IF loop to start analyzer"

      if [ ! -e $FILE2 ]
      then
      echo $FILE
      echo "./analyzer -i "$FILE
      ./analyzer -i $FILE
      mv output.root $FILE2
      fi
  fi
done
