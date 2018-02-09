
LIBS=-lsporth -lsoundpipe -lsndfile -lao -lm
CC=gcc

all:
	$(CC) sporth_ao.c $(LIBS) -o sporth_ao
	$(CC) sporth_ao_stream.c $(LIBS) -o sporth_ao_stream
clean:
	rm sporth_ao sporth_ao_stream

.PHONY: all clean
