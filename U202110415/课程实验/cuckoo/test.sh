#!/bin/bash

seed=119384893020
memInKB=16777216
memInBytes=$(expr $memInKB \* 1024)
itemsAllowed=$(expr $memInBytes / 16)
ramThreshold=$(expr $itemsAllowed / 8)


inputFile='battery.csv'
outputFile='rawResults.csv'

if [ -f $outputFile ] ; then
  rm $outputFile
fi

IFS=$'\n'; set -f; list=($(<$inputFile))
for item in ${list[@]}
do
  echo $item >> $outputFile
  for mapType in {0..1}
  do
    params=$(echo $item | sed 's/,/ /g')
	echo $params
	command=$(echo './build/PerformanceTest' $mapType $params $seed $ramThreshold)
	echo $command
	result=$(eval $command)
    echo $result >> $outputFile
  done
done

$(./formatResults.sh)
