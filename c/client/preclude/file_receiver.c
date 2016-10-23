#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "const.h"
#include "file_receiver.h"

long int recvFile(int sockfd, char* root_directory, char* file_name) {
	
	char file_path[MAXDATASIZE];
	
    strcpy(file_path, root_directory);
	
	int root_len = strlen(root_directory);
	file_path[root_len] = '/';
	file_path[root_len + 1] = '\0';
	
    strcat(file_path, file_name);
	
	FILE* fp = fopen(file_path, "wb+");
	
	if (fp == NULL) {
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