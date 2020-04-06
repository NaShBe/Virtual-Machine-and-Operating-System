CC = gcc
EXENAME = vmos
OUTNAME = context.txt
CCFLAGS = -o $(EXENAME) -g -O0
CFLAGS = -I include -c -g -O0
OBJDEP = architecture.o dispatcher.o handler.o harddrive.o loader.o parse.o process.o scheduler.o vmosmain.o
HEADERDEP = include/architecture.h include/dispatcher.h include/handler.h include/harddrive.h include/loader.h include/parse.h include/process.h include/scheduler.h include/vmos.h

vmos: $(OBJDEP)
	$(CC) $(OBJDEP) $(CCFLAGS)

architecture.o: src/architecture.c
	$(CC) src/architecture.c $(CFLAGS)

dispatcher.o: src/dispatcher.c
	$(CC) src/dispatcher.c $(CFLAGS)

handler.o: src/handler.c
	$(CC) src/handler.c $(CFLAGS)

harddrive.o: src/harddrive.c
	$(CC) src/harddrive.c $(CFLAGS)

loader.o: src/loader.c
	$(CC) src/loader.c $(CFLAGS)

parse.o: src/parse.c
	$(CC) src/parse.c $(CFLAGS)

process.o: src/process.c
	$(CC) src/process.c $(CFLAGS)

scheduler.o: src/scheduler.c
	$(CC) src/scheduler.c $(CFLAGS)

vmosmain.o: src/vmosmain.c
	$(CC) src/vmosmain.c $(CFLAGS)

clean:
	rm $(OBJDEP) $(EXENAME) $(OUTNAME)