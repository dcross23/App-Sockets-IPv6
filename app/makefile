CC = gcc
CFLAGS = 
LIBS =

PROGS = sender receiver

all: ${PROGS}

sender: sender.o sender.h
	${CC} ${CFLAGS} -o $@ sender.o ${LIBS}

receiver: receiver.o receiver.h
	${CC} ${CFLAGS} -o $@ receiver.o ${LIBS}

clean:
	rm *.o ${PROGS}

