CC := gcc
CFLAGS := -g

all: server client

% : %.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean test
clean:
	rm server client

test: server client
	./server
	./client
