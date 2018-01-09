#!/bin/bash

runs=(146 27 28 29 32 110 120 124 127 132 136 152 160 161 165 171 174 177 182 184 187 194 202 208 210 213 217 220)

#runs="0 1 2 3"
threads=7

funcx() {
	echo $#
	while [ $# -gt 0 ]
		do
			for run in $(seq 1 $threads)
			do
				echo ${runs[$run]}
			done
			shift $threads
		done
}

func() {
	n=${#runs[@]}
	echo $n
	for run in $(seq 0 $threads $((n - 1)))
	do
		for i in $(seq 0 $((threads - 1)))
		do
			
		#	echo ${runs[$(($run + $i))]}
			analyze ${runs[$(($run + $i))]}
			
		done
		wait
	echo ""
	done
}

analyze(){
 echo $1
  if [ $1 -lt 10 ]
  then
      FILE="/data/64Zn_pp/run0000"$1".mid.gz"
  elif [ $1 -lt 100 ]
  then
      FILE="/data/64Zn_pp/run000"$1".mid.gz"
  elif [ $1 -lt 1000 ]
  then
      FILE="/data/64Zn_pp/run00"$1".mid.gz"
  elif [ $1 -lt 10000 ]
  then
      FILE="/data/64Zn_pp/run0"$1".mid.gz"
  elif [ $1 -lt 100000 ]
  then
      FILE="/data/64Zn_pp/run"$1".mid.gz"
  else
      echo "This might not work"
  fi
  echo $FILE
  if [ $1 -lt 10 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted0000"$1".root"
  elif [ $1 -lt 100 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted000"$1".root"
  elif [ $1 -lt 1000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted00"$1".root"
  elif [ $1 -lt 10000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted0"$1".root"
  elif [ $1 -lt 100000 ]
  then
      FILE2="/home/fdiel/Analysis_local/iThemba/root_26Mg/sorted"$1".root"
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
      ./analyzer -i $FILE &
#       mv output.root /media/Elements/PR226/sorted000$COUNT.root
      mv output.root $FILE2
      fi
  fi

}

func
#${runs}

