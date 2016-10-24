#ifndef CONST_H
#define CONST_H

#define MAXDATASIZE 200

#define TOTAL_PORT_NUM 65536
#define BACKLOG 20

#define COMMAND 0
#define DATA 1

#define INVALID_PORT -1
#define INVALID_SOCKFD -1

#define INVALID_TRANS 0
#define PORT_TRANS 1
#define PASV_TRANS 2

#define IGNORE 0
#define SET_PORT 1
#define SET_PASV 2
#define BEGIN_RETR 3
#define BEGIN_STOR 4
#define FILE_SUCC 5
#define FILE_FAIL 6
#define TO_QUIT 7
#define STOP_RETR 8
#define STOP_STOR 9

#define UN_FEASIBLE 0
#define FEASIBLE 1 

#endif