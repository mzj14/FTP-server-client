#ifndef RECEIVER_H
#define RECEIVER_H

#define END_CHAR '$'
/* 
   receive all the character to buf
*/           
extern int recvAll(int s, char* buf, int size);

#endif