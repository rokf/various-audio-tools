
LDLIBS=-lsporth -lsoundpipe -lsndfile -lao -lm

all: sporth_ao sporth_ao_stream

sporth_ao: sporth_ao.c
sporth_ao_stream: sporth_ao_stream.c

clean:
	rm sporth_ao sporth_ao_stream

.PHONY: all clean
