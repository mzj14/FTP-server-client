#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "debug.h"
#include "sender.h"
#include "const.h"

int sendAll(int s, char* buf, int len) {
	int total = 0;
	int bytesleft = len;
	int n;
	while (total < len) {
		n = send(s, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft = -n;
	}
	// send(s, END_STRING, strlen(END_STRING), 0);
	return n == -1 ? -1 : 0; 
}

long int sendFile(int sockfd, char* root_directory, char* file_name) {
    char file_path[MAXDATASIZE];
    strcpy(file_path, root_directory);
	
	int root_len = strlen(root_directory);
	file_path[root_len] = '/';
	file_path[root_len + 1] = '\0';

    strcat(file_path, file_name);
	
	f_write(file_path);
	// char file_buffer[MAXDATASIZE * 5];
	
	FILE* file = fopen(file_path, "r+b");
	fseek(file, 0, SEEK_END);
	long int fileSize = ftell(file);
	rewind(file);
	
	long int pos = 0;
	int readLen;
	
	char file_buffer[100000];
	readLen = fread(file_buffer, sizeof(char), sizeof(file_buffer), file); 
	sendAll(sockfd, file_buffer, readLen);
	
	/*
		readLen = fread(file_buffer, sizeof(char), sizeof(file_buffer), file); 
		if(readLen > 0) 
		{ 
			pos += readLen;
			send(sockfd, file_buffer, readLen, 0);
		} 
	} while (pos < fileSize); 
	*/
	/*
	int fd = open(file_path, O_RDONLY);  
    off_t offset = 0;  
    struct stat fileStatus;  
    int fstat_ret = fstat(fd, &fileStatus);
	
	ssize_t length = sendfile(sockfd, fd, &offset, fileStatus.st_size);
    */
	
    return readLen;	
}