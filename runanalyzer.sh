#!/bin/bash

############################################################
#
#	shell script
#	
#
############################################################

rm -rf sortedRun*.root

for i in `seq 103 108`;

do
	if (($i < 10))
		then
		./analyzer -i ~/iThemba/PR242/DATA/midasRaw/run0100$i.mid.gz
		mv output.root sortedRun100$i.root
	fi

	if (($i < 100 )) && (($i > 10))
		then
		./analyzer -i ~/iThemba/PR242/DATA/midasRaw/run010$i.mid.gz
		mv output.root sortedRun10$i.root
	fi

	if (($i > 100 ))
		then
		./analyzer -i ~/iThemba/PR242/DATA/midasRaw/run01$i.mid.gz
		mv output.root sortedRun1$i.root
	fi
done


mv sortedRun* ../../iThemba/PR242/DATA/rootFiles/FirstWeekend/
