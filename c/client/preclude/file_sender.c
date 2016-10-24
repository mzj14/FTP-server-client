#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "file_sender.h"
#include "const.h"

long int sendFile(int sockfd, char* root_directory, char* file_name) {
		
    char file_path[MAXDATASIZE];
    strcpy(file_path, root_directory);
	
	int root_len = strlen(root_directory);
	file_path[root_len] = '/';
	file_path[root_len + 1] = '\0';

    strcat(file_path, file_name);
		
	char file_buffer[MAXDATASIZE * 5];
	
	
	FILE* file = fopen(file_path, "rb+");
	fseek(file, 0, SEEK_END);
	long int fileSize = ftell(file);
	rewind(file);
	
	long int pos = 0;
	int readLen;
		
	do {
		readLen = fread(file_buffer, sizeof(char), sizeof(file_buffer), file); 
		if(readLen > 0) 
		{ 
			pos += readLen;
			int send_result = send(sockfd, file_buffer, readLen, 0);
		} 
	} while (pos < fileSize); 
	
	fclose(file);
	
    return readLen;
	
}