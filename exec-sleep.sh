#!/bin/bash

cmd=$1
t=$2

while true; do
  $1 &
  pid=$!
  echo "pid: $pid"
  sleep $t
  kill -6 $pid
done
