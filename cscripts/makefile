CC=gcc
CFLAGS=-Wall
BINS=provalseek

all: $(BINS)
	
clean:
	rm -rf *.o $(BINS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@
