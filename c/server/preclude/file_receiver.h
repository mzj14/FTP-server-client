#ifndef RECEIVER_H
#define RECEIVER_H

/* 
	sockfd: the socket which we will recv file data from
	root_directory: the directory where file located
*/

extern long int recvFile(int sockfd, char* root_directory, char* file_name);

#endif