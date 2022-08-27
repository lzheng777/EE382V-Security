CC := gcc
CFLAGS := -std=c11 -g

all: server client

% : %.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm server client

