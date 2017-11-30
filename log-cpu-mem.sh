#!/bin/bash

seven &

pid=$!

interval=1

t=0

output=log-cpu-mem.txt

echo "t(s) %cpu mem (kb)" > $output

while true; do
  psres=`ps o pcpu,rsz -p $pid | tail -n1`;
  echo "$t $psres" >> $output; sleep $interval;
  ((t++))
done

kill -6 $pid

