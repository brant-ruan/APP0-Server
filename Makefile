
object1 = Main.o Socket.o Database.o \
		 QRCode.o HTML.o GenPacket.o Interact.o Mobile.o

object2 = Socket.o HTML.o GenPacket.o  QRCode.o Database.o Mobile.o Interact.o webS.o

TARGET = mobS webS

all: $(TARGET)

mobS: $(object1)
	gcc -o mobS $(object1) -lmysqlclient

webS: $(object2)
	gcc -o webS $(object2) -lmysqlclient

Main.o: Main.c

Socket.o: Socket.c

Database.o: Database.c
	gcc -c Database.c -lmysqlclient

QRCode.o: QRCode.c

GenPacket.o: GenPacket.c

Interact.o: Interact.c

HTML.o: HTML.c

Mobile.o: Mobile.c

webS.o: webS.c

.PHONY:
clean:
	-rm $(object1) $(object2)
