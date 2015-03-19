serwer:	serwer.c protokol.h klient
	gcc -Wall serwer.c protokol.h -o serwer

klient:	klient.c protokol.h
	gcc -Wall klient.c protokol.h -o klient
