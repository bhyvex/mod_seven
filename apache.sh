#!/bin/bash

interval=1

t=0

output=log-cpu-mem_apache.txt

echo "t(s) %cpu mem(kb)" > $output

while true; do
   psres=`ps aux | grep apache2 | grep -v pts| awk '{ totMemoria += $6; totCpu+=$3; procs += 1} END {print totCpu," ",totMemoria}'| tail`;
   echo "$t $psres" >> $output; sleep $interval;
   ((t++))
done
