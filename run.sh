#!/bin/bash

while : ; do
	cd middle
	./https.src.x
	cd ..
	sleep 10
done &
while : ; do
	cd dndd
	./dndd.dndd.x
	cd ..
	sleep 10
done &
while : ; do
	cd middle
	python3 pybind/gensim-server.py
	cd ..
	sleep 10
done &
