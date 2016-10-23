#ifndef SENDER_H
#define SENDER_H

/* 
	sockfd: the socket which we will put file data into
	root_directory: the directory where file located
*/

extern long int sendFile(int sockfd, char* root_directory, char* file_name);

#endif