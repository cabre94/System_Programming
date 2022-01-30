#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

int main(){

	const char* str = "Hello World\n";

	int fd = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);

	write(1, str, strlen(str));

	return 0;
}