#================================
#	PR166 - WE1

#------------------------------------------------
#   Test runs
#   Still during testing phases
#   Runs 11141 and 11145 have properly documented run numbers
#for COUNT in {11141,11145,11146,11147,11148,11149,11150}


#------------------------------------------------
#   Purely sorted between different targets
#   12C target ("thin"), 0.3 mg/cm^2
#for COUNT in {11155,11156,11322,11323,11324,11336}
#   12C target ("thick"), 1.052 mg/cm^2
#for COUNT in {11157,11158,11159,11340,11345,11346}


#------------------------------------------------
#   Sorted by both TARGET and ANGLE

#============================================================================================================
#           8 Degrees
#============================================================================================================
#   12C target ("thin"), 0.3 mg/cm^2
#for COUNT in {11155,11156}
#   12C target ("thick"), 1.052 mg/cm^2
#for COUNT in {11157,11158,11159}

#============================================================================================================
#           10 Degrees
#============================================================================================================
#   12C target ("thin"), 0.3 mg/cm^2
#for COUNT in {11322,11323,11324,11336}
#   12C target ("thick"), 1.052 mg/cm^2
#for COUNT in {11340,11345,11346}


#--------------------------------
#   Total inclusive sort list
#for COUNT in {11141,11145,11146,11147,11148,11149,11150,11155,11156,11157,11158,11159,11322,11323,11324,11336,11340,11345,11346}

#------------------------------------------------
#   Inclusive sort list (without test runs)
#for COUNT in {11155,11156,11157,11158,11159,11322,11323,11324,11336,11340,11345,11346}
for COUNT in {11155,11156,11158,11322,11323,11324,11336,11345,11346}

#   Removed 11157 (pepperport)
#   Removed 11159 (test run: tdc0 plug moved to tdc6)
#   Removed 11340 (Contamination in Y position)


#--------------------------------
#for COUNT in {11155,111555346}


do

#====================================================
#       Directories for the data and sorted data
dataDirectory="/media/psf/DATA/PR166/data"
#dataDirectory="/media/psf/Home/Academic/Experiments/PR166_2010/data"

sortedDirectory="/media/psf/Home/Academic/Experiments/PR166_2010/sorted"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR166_2010/sortedNoPIDoffsets"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR166_2010/sortedAutoTrim"
#sortedDirectory="/media/psf/Home/Academic/Experiments/PR166_2010/sortedTest"


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
  elif [ $COUNT -lt 1000000 ]
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
  elif [ $COUNT -lt 1000000 ]
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
