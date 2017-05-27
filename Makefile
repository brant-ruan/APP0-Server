object = Main.o Socket.o Database.o \
		 QRCode.o GenPacket.o Interact.o HTML.o Mobile.o

ts: $(object)
	gcc -o ts $(object) -lmysqlclient

Main.o: Main.c

Socket.o: Socket.c

Database.o: Database.c
	gcc -c Database.c -lmysqlclient

QRCode.o: QRCode.c

GenPacket.o: GenPacket.c

Interact.o: Interact.c

HTML.o: HTML.c

Mobile.o: Mobile.c


.PHONY:
clean:
	-rm $(object)
