#!/bin/bash

while : ; do
	cd sample 
	./bikin.src.x -d Bikin -p 2005
	cd ..
	sleep 10
done &

while : ; do
	cd ez
	./ez.src.x -dir www -port 2002
	cd ..
	sleep 10
done &

while : ; do
	cd dndd
	./dndd.src.x -dir www -port 2003
	cd ..
	sleep 10
done &

while : ; do
	cd suwon
	./suwon.src.x -dir www -port 2001
	cd ..
	sleep 10
done &

while : ; do
	cd sample
	./site.src.x -dir www -port 2004
	cd ..
	sleep 10
done &
