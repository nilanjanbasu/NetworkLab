stopwait:network receiver

network: Network.o
	gcc -o network Network.o

receiver: Reciever.o
	gcc -g -o receiver Reciever.o	

