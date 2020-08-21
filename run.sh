#!/bin/bash

while : ; do
	cd middle
	./middle.src.x -port 4433 -conf ../eZ.conf -cert ../cert.pem -key ../key.pem
	cd ..
	sleep 10
done &
while : ; do
	cd dndd
	./dndd.src.x -port 2001 -dir www
	cd ..
	sleep 10
done &
while : ; do
	cd middle
	python3 pybind/gensim-server.py
	cd ..
	sleep 10
done &
while : ; do
	cd suwon
	./suwon.src.x -dir www -port 2002
	cd ..
	sleep 10
done &
while : ; do
	cd ez
	./ez.src.x -dir www -port 2003
	cd ..
	sleep 10
done &

while : ; do
	cd adnet
	./adnet.src.x -dir www -port 2004
	cd ..
	sleep 10
done &

while : ; do
	cd adnet
	./ad.src.x -dir www -port 2005 
	cd ..
	sleep 10
done &

