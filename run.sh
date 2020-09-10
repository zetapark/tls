#!/bin/bash

while : ; do
	cd lib 
	./middle.tls.x -port 4433 
	cd ..
	sleep 10
done &
while : ; do
	cd biz
	./biz.src.x -port 2001
	cd ..
	sleep 10
done &
while : ; do
	cd middle
	python3 pybind/gensim-server.py
	cd ..
	sleep 10
done &
