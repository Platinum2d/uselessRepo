CC=gcc
CFLAGS=-Wall -I../../utils/
HEADERS=../../utils/utils.h
LIBS=../../utils/utils.c
BINS=es21 es23 es24 es26 es27 es28

all: $(BINS)
	
clean:
	rm -rf *.o $(BINS)

%: %.c $(HEADERS) $(LIBS)
	$(CC) $(CFLAGS) $(LIBS) $< -o $@
