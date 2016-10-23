#ifndef BINDER_H
#define BINDER_H

#define BEGIN_PORT 10000 // the minimum port your machine is going to bind

/*
	dest_ip: ip you want you bind with, if NULL, you will bind to localhost;
	dest_port: port you want to bind with, if INVALID_PORT, you will bind to a random available port;
	connection_num: the connection your port is going to listen to;
	result_ip: the ip of your socket, you told it to others
	result_port: the port of your socket, you told it to others
	
	return value: the socket file discriptor
*/
	
extern int bindSocketWithLocal(char* dest_ip, int dest_port, int connection_num, char* result_ip, int* result_port);

#endif