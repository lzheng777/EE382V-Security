all: rsa

rsa: rsa.c accelerator.c
	gcc -g -std=c11 -o $@ $^

clean:
	rm rsa