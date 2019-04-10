all: sequentialPi threadPi mutexPi atomicPi falseSharingPi trueAndFalseSharingPi barriersPi

sequentialPi: sequentialPi.c
	icc -o sequentialPi sequentialPi.c -O3 -lpthread

threadPi: threadPi.c
	icc -o threadPi threadPi.c -O3 -lpthread

mutexPi: mutexPi.c
	icc -o mutexPi mutexPi.c -O3 -lpthread

atomicPi: atomicPi.cpp
	icc -o atomicPi atomicPi.c -O3 -lpthread -std=c++11

falseSharingPi: falseSharingPi.c
	icc -o falseSharingPi falseSharingPi.c -O3 -lpthread

trueAndFalseSharingPi: trueAndFalseSharingPi.c
	icc -o trueAndFalseSharingPi trueAndFalseSharingPi.c -O3 -lpthread

barriersPi: barriersPi.c
	icc -o barriersPi barriersPi.c -O3 -lpthread