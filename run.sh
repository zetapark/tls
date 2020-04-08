#!/bin/bash

while : ; do
	./https.src.x
	sleep 10
done &
while : ; do
	./dndd.dndd.x
	sleep 10
done &
while : ; do
	python3 pybind/gensim-server.py
	sleep 10
done &
