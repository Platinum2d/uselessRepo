CC=gcc
CFLAGS=-Wall
BINS=main prova

all: $(BINS)
	
clean:
	rm -rf *.o $(BINS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@
