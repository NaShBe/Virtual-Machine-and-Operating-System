CC = gcc
CCFLAGS = -o vmos
CFLAGS = -I include -c
OBJDEP = architecture.o dispatcher.o handler.o harddrive.o loader.o parse.o process.o scheduler.o vmosmain.o
HEADERDEP = architecture.h dispatcher.h handler.h harddrive.h loader.h parse.h process.h scheduler.h vmos.h

vmos: $(OBJDEP)
	$(CC) $(OBJDEP) $(CCFLAGS)

architecture.o:
	$(CC) src/architecture.c $(CFLAGS)

dispatcher.o:
	$(CC) src/dispatcher.c $(CFLAGS)

handler.o:
	$(CC) src/handler.c $(CFLAGS)

harddrive.o:
	$(CC) src/harddrive.c $(CFLAGS)

loader.o:
	$(CC) src/loader.c $(CFLAGS)

parse.o:
	$(CC) src/parse.c $(CFLAGS)

process.o:
	$(CC) src/process.c $(CFLAGS)

scheduler.o:
	$(CC) src/scheduler.c $(CFLAGS)

vmosmain.o:
	$(CC) src/vmosmain.c $(CFLAGS)