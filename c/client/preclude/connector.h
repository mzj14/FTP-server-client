#ifndef CONNECTOR_H
#define CONNECTOR_H

/* 
	dest_ip: the other side ip which will be connected to
	dest_port: the other side port which will be connected to
	return value: the socket file discriptor
*/
	
extern int connectSocketToDest(char* dest_ip, int dest_port);

#endif