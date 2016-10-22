#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "debug.h"
#include "const.h"
#include "receiver.h"

int recvAll(int s, char* buf, int size) {
	int total = 0; // the number of bytes we receive
	int bytesleft = size; // the number of bytes unused in buffer
	int n;
	do {
		n = recv(s, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft = -n;
	} while (buf[total - 1] != END_CHAR);
	
    if (n != -1) {
		buf[total - 1] = '\0';
	}
	return n == -1 ? -1 : 0; 
}

long int recvFile(int sockfd, char* root_directory, char* file_name) {
	char file_path[MAXDATASIZE];
	int has_file_content = 0;
	
    strcpy(file_path, root_directory);
	
	printf("file_path = %s\n", file_path);
	
	int root_len = strlen(root_directory);
	file_path[root_len] = '/';
	file_path[root_len + 1] = '\0';

	printf("file_path = %s\n", file_path);
	
    strcat(file_path, file_name);

    printf("file_path = %s\n", file_path);
	
	FILE* fp = fopen(file_path, "wb+");
	
	if (fp == NULL) {
		fprintf(stderr, "Can not open file locally !");
        return -1;
	}
	
	char buff[MAXDATASIZE];
    ssize_t length;
	long int file_size = 0;
	
    while((length = recv(sockfd, buff, sizeof(buff), 0)) > 0) {
        fwrite(buff, 1, length, fp);
		file_size += length;
    }
	
	fclose(fp);
   
	return file_size;
}