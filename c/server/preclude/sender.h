#ifndef SENDER_H
#define SENDER_H

#define END_STRING "$"
/* 
   send all the character in buf
*/           
extern int sendAll(int s, char* buf, int len);

extern long int sendFile(int sockfd, char* file_name);

#endif