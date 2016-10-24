/*
** client.c -- a stream socket client
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>

#include "preclude/command_processor.h"
#include "preclude/connector.h"
#include "preclude/modify.h"
#include "preclude/file_sender.h"
#include "preclude/file_receiver.h"
#include "preclude/const.h"
#include "preclude/binder.h"
#include "preclude/reply_classifier.h"
#include "preclude/request_examiner.h"
#include "preclude/param_getter.h"

int main(int argc, char *argv[])
{
    int sockfd; // the socket file descriptor for command connection
	
    char buf[MAXDATASIZE]; // the reply from client
	
    char send_msg[MAXDATASIZE]; // the message to send as request
	
    char data_connect_ip[MAXDATASIZE]; // the relevant ip for data connectin
	
    int data_connect_port = INVALID_PORT; // the relevant port for data connection
	
    int data_sockfd = INVALID_SOCKFD; // the relevant socket for data connection
	
    int new_data_sockfd = INVALID_SOCKFD; // the new data socket descriptor created by accept 
	
	char file_name[MAXDATASIZE]; // the name of the file to be send or stor
	
	int data_transmode = INVALID_TRANS; // related to pasv or port
	
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size = sizeof(struct sockaddr_in);
    
	memset(send_msg, 0, sizeof send_msg);
	memset(data_connect_ip, 0, sizeof data_connect_ip);
	memset(file_name, 0, sizeof file_name);
  
    /************************************ get server ip, port and root directory from command ************************************/
    char server_ip[MAXDATASIZE], root_directory[MAXDATASIZE];
    int server_port;
	
    getIPAndPortAndRoot(argc, argv, server_ip, &server_port, root_directory);
	
    sockfd = connectSocketToDest(server_ip, server_port);
	
	while (1) {
		memset(buf, 0, sizeof buf);
		
		if (recv(sockfd, buf, MAXDATASIZE - 1, 0) == -1) {
			perror("recv error");
			exit(1);
		}
		
		printf("%s", buf);
		
		int cat = classifyReply(buf, send_msg);
		
		if (cat == TO_QUIT) {
			break;
		}
		
		if (cat == SET_PORT) {			
			getPortParamFrom(send_msg, data_connect_ip, &data_connect_port);
			
			// remove the previous data socket
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
			
			// bind a socket to local address and port
			data_transmode = PORT_TRANS;
			data_sockfd = bindSocketWithLocal(data_connect_ip, data_connect_port, BACKLOG, NULL, NULL);
		}
		
		if (cat == SET_PASV) {
			getPortParamFrom(buf, data_connect_ip, &data_connect_port);
			
			// remove the previous data socket
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}

			data_transmode = PASV_TRANS;
		}
		
		// server could not send the file
		// client could not stor the file
		// connection can't establish
		if (cat == FILE_FAIL || cat == STOP_RETR || cat == STOP_STOR) {
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
			memset(data_connect_ip, 0, sizeof data_connect_ip);
			data_connect_port = INVALID_PORT;
		}
		
		if (cat == BEGIN_RETR) {
			if (data_transmode == PORT_TRANS && data_sockfd != INVALID_SOCKFD) {
				new_data_sockfd = accept(data_sockfd, NULL, NULL);
			}
			
			if (data_transmode == PASV_TRANS) {
				data_sockfd = connectSocketToDest(data_connect_ip, data_connect_port);
			}
			
			memset(buf, 0, sizeof buf);
			
			if (recv(sockfd, buf, MAXDATASIZE - 1, 0) == -1) {
				perror("recv error");
				exit(1);
			}
			
			if (classifyReply(buf, send_msg) == FILE_SUCC) {
				getFileParam(send_msg, file_name);
				if (data_transmode == PORT_TRANS) {
					recvFile(new_data_sockfd, root_directory, file_name);
				} else {
					recvFile(data_sockfd, root_directory, file_name);
				}
			}
				
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
			
			if (new_data_sockfd != INVALID_SOCKFD) {
				close(new_data_sockfd);
				new_data_sockfd = INVALID_SOCKFD;
			}
			
			printf("%s", buf);
			
			memset(data_connect_ip, 0, sizeof data_connect_ip);
			data_connect_port = INVALID_PORT;
		}
		
		if (cat == BEGIN_STOR) {
			if (data_transmode == PORT_TRANS && data_sockfd != INVALID_SOCKFD) {
				new_data_sockfd = accept(data_sockfd, NULL, NULL);
			}
			if (data_transmode == PASV_TRANS) {
				data_sockfd = connectSocketToDest(data_connect_ip, data_connect_port);
			}
			
			getFileParam(send_msg, file_name);
			
			if (data_transmode == PORT_TRANS) {
				sendFile(new_data_sockfd, root_directory, file_name);
			} else {
				sendFile(data_sockfd, root_directory, file_name);
			}
			
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
			
			if (new_data_sockfd != INVALID_SOCKFD) {
				close(new_data_sockfd);
				new_data_sockfd = INVALID_SOCKFD;
			}
			
			memset(buf, 0, sizeof buf);
			
			if (recv(sockfd, buf, MAXDATASIZE - 1, 0) == -1) {
				perror("recv error");
				exit(1);
			}
			
			printf("%s", buf);
			
			memset(data_connect_ip, 0, sizeof data_connect_ip);
			data_connect_port = INVALID_PORT;
			
		}
		
		do {
			memset(send_msg, 0, sizeof send_msg);
			// client now get msg from command line
			fgets(send_msg, MAXDATASIZE - 2, stdin);
		} while(requestExamine(root_directory, send_msg) == UN_FEASIBLE);
	   
		modifyTail(send_msg);
		
		if (send(sockfd, send_msg, strlen(send_msg), 0) == -1) {
			perror("send error");
			exit(1);
		}
	}
  
	close(sockfd);

	return 0;
}


