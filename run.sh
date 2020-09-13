while : ; do
	cd lib
	./middle.tls.x
	cd ..
	sleep 10
done &
while : ; do
	cd adnet
	./misc.src.x -dir misc -port 2003
	cd ..
	sleep 10
done &
while : ; do
	cd adnet
	./ad.src.x -dir ad -port 2002
	cd ..
	sleep 10
done &
while : ; do
	cd adnet
	./adnet.src.x -dir www -port 2004 -data localhost
	cd ..
	sleep 10
done &
while : ; do
	cd biz
	./biz.src.x -dir www -port 2001 -data localhost
	cd ..
	sleep 10
done &

while : ; do
	cd middle
	./gensim-server.py
	cd ..
	sleep 10
done &

while : ; do
	cd adnet
	./adnet2.src.x -dir Bikin -port 2005 -data localhost
	cd ..
	sleep 10
done &
