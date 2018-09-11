CC=gcc
CFLAGS=-Wall -I utils/
HEADERS=utils/utils.h
LIBS=utils/utils.c
BINS=es1 es2 es3 es4 es5 es6 es7

all: $(BINS)
	

clean:
	rm -rf *.o $(BINS)

%: %.c $(HEADERS) $(LIBS)
	$(CC) $(CFLAGS) $(LIBS) $< -o $@
