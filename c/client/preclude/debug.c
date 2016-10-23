#include "debug.h"
#include <stdio.h>
#include <string.h>

void f_write(char* str) {
	FILE* fp;
	fp = fopen("test1.txt", "a+");
	if (fp == NULL) {
		printf("fail to open test.txt");
		return;
	}
	fwrite(str, 1, strlen(str), fp);
	fclose(fp);
	return;
}