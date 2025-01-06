#!/bin/bash


# PEPPERPOT ===========================================================================
#
#   pp', 58Ni, September 2024
#   10 degr, 1 old and 1 new VDC; Pepperpot collimator, 
#   for COUNT in {106,107,108,109,110,113,114}
#   
#   For the TDCoffsets for pp', use 58Ni pp' runs that were used for resolution tuning
#   for COUNT in {106,107,108,109}
#======================================================================================
#
# (p,d) Full acceptance of 58Ni g.s. - September 2024 ================================
#
#   10 degr 58Ni (D1 = 285.62)
#   for COUNT in {130,131,132,133,142,144,147,148,152,153,155,176,177,178}
#
#   Subset for TDC offset creation and initial analysis (includes 58Ni, 24Mg and 97Mo)
#    for COUNT in {144,147,152,153,155,156,159,161,162,164,165,166,167,169,170,171,172,173,176,177}
    for COUNT in {144,147}
#
#--------------------------------------------------------------------------------------
# (p,d) for 24Mg - September 2024 =====================================================
#
#   10 degr 24Mg (D1 = 265.32)
#   for COUNT in {118,119,121,122,123,156,159,161,162}
#--------------------------------------------------------------------------------------
# (p,d) for 97Mo - September 2024 =====================================================
#
#   10 degr 97Mo (D1 = 299.40)
#   for COUNT in {166,167,169,170,171,173}
#--------------------------------------------------------------------------------------
# All September 2024 runs for use in calculating TDC offsets 
#   
#   for COUNT in {110,113,114,118,121,122,123,124,126,128,129,130,131,132,133,144,147,152,153,155,156,159,161,162,164,165,166,167,169,170,171,172,173,176,177}
#
#--------------------------------------------------------------------------------------

#--------------------------------------------------------------------------------------

do
  echo $COUNT
  if [ $COUNT -lt 10 ]
  then
      FILE="/media/lmdonaldson/Elements/PR373/DATA/MID/run0000"$COUNT".mid.gz"
  elif [ $COUNT -lt 100 ]
  then
      FILE="/media/lmdonaldson/Elements/PR373/DATA/MID/run000"$COUNT".mid.gz"
  elif [ $COUNT -lt 1000 ]
  then
      FILE="/media/lmdonaldson/Elements/PR373/DATA/MID/run00"$COUNT".mid.gz"
  elif [ $COUNT -lt 10000 ]
  then
      FILE="/media/lmdonaldson/Elements/PR373/DATA/MID/run0"$COUNT".mid.gz"
  elif [ $COUNT -lt 100000 ]
  then
      FILE="/media/lmdonaldson/Elements/PR373/DATA/MID/run"$COUNT".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $COUNT -lt 10 ]
  then
      FILE2="/home/lmdonaldson/K600/Data/PR373/ROOT/sorted0000"$COUNT".root"
  elif [ $COUNT -lt 100 ]
  then
      FILE2="/home/lmdonaldson/K600/Data/PR373/ROOT/sorted000"$COUNT".root"
  elif [ $COUNT -lt 1000 ]
  then
      FILE2="/home/lmdonaldson/K600/Data/PR373/ROOT/sorted00"$COUNT".root"
  elif [ $COUNT -lt 10000 ]
  then
      FILE2="/home/lmdonaldson/K600/Data/PR373/ROOT/sorted0"$COUNT".root"
  elif [ $COUNT -lt 100000 ]
  then
      FILE2="/home/lmdonaldson/K600/Data/PR373/ROOT/sorted"$COUNT".root"
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
