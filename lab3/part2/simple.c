#include "stdio.h"
#include "stdlib.h"

#define BUF_SIZE 256

int main(){
	FILE* fptr = fopen("/home/class/labs-sec/lab2/simple_example/data/simple.txt", "w");
	fputs("Hello World!\n", fptr);
	fclose(fptr);

	fptr = fopen("/home/class/labs-sec/lab2/simple_example/data/secret.txt", "r");
	char buf[BUF_SIZE];
	fgets(buf, BUF_SIZE, fptr);
	printf("%s\n", buf);
	fclose(fptr);
	return 0;
}
