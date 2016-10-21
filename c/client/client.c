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

#include "preclude/connector.h"
#include "preclude/modify.h"
#include "preclude/sender.h"
#include "preclude/receiver.h"
#include "preclude/const.h"
#include "preclude/handler.h"
#include "preclude/separator.h"
#include "preclude/client_data.h"
#include "preclude/binder.h"
#include "preclude/classifier.h"

#define IP "-ip" // ip argument in command
#define PORT "-port"  // the port argument in command
#define ROOT "-root"  // the root argument in command

#define TOTAL_PORT_NUM 65536
#define DEFAULT_IP "127.0.0.1" 
#define IP_REGEX "^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$"
#define DEFAULT_PORT 21  
#define DEFAULT_ROOT "local"

#define FULL_ACCESS S_IRWXU|S_IROTH|S_IWOTH|S_IXOTH

#define BACKLOG 10 // how many pending connections queue will hold

// get the port and root directory from command

void getIPAndPortAndRoot(int argc, char* argv[], char* ip, int* port, char* directory);

int main(int argc, char *argv[])
{
    int sockfd;  
    char buf[MAXDATASIZE];
    char send_msg[MAXDATASIZE], copy_msg[MAXDATASIZE], verb[MAXDATASIZE], parameter[MAXDATASIZE], error_msg[MAXDATASIZE];
    char data_connect_ip[MAXDATASIZE];
    int data_connect_port = INVALID_PORT;
    int data_sockfd = INVALID_SOCKFD;
    int new_data_sockfd = INVALID_SOCKFD;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size = sizeof(struct sockaddr_in);
    
  
    /************************************ get server ip, port and root directory from command ************************************/
    char server_ip[MAXDATASIZE], root_directory[MAXDATASIZE];
    int server_port;
	
    getIPAndPortAndRoot(argc, argv, server_ip, &server_port, root_directory);
    
	printf("ip = %s\n", server_ip);
	printf("port = %d\n", server_port);
	printf("root_directory = %s\n", root_directory);
	
    sockfd = connectSocketToDest(server_ip, server_port);
    printf("sockfd = %d\n", sockfd);
  
	while (1) {
		memset(buf, 0, sizeof buf);
		if (recv(sockfd, buf, MAXDATASIZE - 1, 0) == -1) {
			perror("recv error");
			exit(1);
		}
	
		printf("%s", buf);
	
	
		if (strcmp(buf, LOG_IN_PORT_MSG) == 0) {
			// bind a socket to local address and port
			printf("Begin to bind socket.");
			memset(verb, 0, sizeof verb);
			memset(parameter, 0, sizeof parameter);
			
			separateRequest(send_msg, verb, parameter);
			splitPortParam(parameter, data_connect_ip, &data_connect_port);
			
			// remove the previous data socket
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
			}	
			data_sockfd = bindSocketWithLocal(data_connect_ip, data_connect_port, BACKLOG, NULL, NULL);
		}
		
		if (startsWith(buf, "227")) {
			// store the server data and ip address to connect.
			// need to add transmode to client
		}
    
	    memset(send_msg, 0, sizeof send_msg);
		memset(copy_msg, 0, sizeof copy_msg);
		memset(verb, 0, sizeof verb);
		memset(parameter, 0, sizeof parameter);
		
		// client now get msg from command line
		fgets(send_msg, MAXDATASIZE - 2, stdin);
		modifyTail(send_msg);
		
		printf("send_msg = %s\n", send_msg);
		strcpy(copy_msg, send_msg);
		printf("copy_msg = %s\n", copy_msg);
		
		separateRequest(copy_msg, verb, parameter);
		int cat = getRequestCategory(verb, parameter, error_msg);
		printf("parameter = %s\n", parameter);
		
		if (sendAll(sockfd, send_msg, strlen(send_msg)) == -1) {
			perror("send");
		} else {
			printf("cat = %d\n", cat);
			printf("data_sockfd = %d\n", data_sockfd);
			
			if (cat == RETR_COMMAND && data_sockfd != INVALID_SOCKFD) {
				printf("I am going to accept.\n");
				new_data_sockfd = accept(data_sockfd, (struct sockaddr *)&their_addr, &sin_size);
				if (new_data_sockfd == -1) {
					perror("accept data connection error");
				} else {			
					recvFile(new_data_sockfd, root_directory, parameter);
					close(data_sockfd);
					data_sockfd = INVALID_SOCKFD;
				}
			}
		}
	}
  
	close(sockfd);

	return 0;
}

void getIPAndPortAndRoot(int argc, char* argv[], char* ip, int* port, char* directory) {
	int give_ip_flag = 0, give_root_flag = 0, give_port_flag = 0;
	
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], IP) == 0) {
			if (i + 1 < argc) {
				if (checkParamWithRegex(argv[i + 1], IP_REGEX) == 1) {
					strcpy(ip, argv[i + 1]);
					give_ip_flag = 1;
				}
			}
		}
		
		if (strcmp(argv[i], PORT) == 0) {
			if (i + 1 < argc) {
				int rv = atoi(argv[i + 1]);
				if (0 < rv && rv < TOTAL_PORT_NUM) {
					*port = rv;
					give_port_flag = 1;
				}
			}
		}
		
		if (strcmp(argv[i], ROOT) == 0) {
			if (i + 1 < argc) {
				if (access(argv[i + 1], 0) == 0 || mkdir(argv[i + 1], FULL_ACCESS) == 0) {
					printf("Create the root directory %s.\n", argv[i + 1]);
					strcpy(directory, argv[i + 1]);
					give_root_flag = 1;
				}	
			}
		}
	}
	
	if (give_ip_flag == 0) {
		strcpy(ip, DEFAULT_IP);
	}
	
	if (give_port_flag == 0) {
		*port = DEFAULT_PORT;
	}
	
	if (give_root_flag == 0) {
		if (access(DEFAULT_ROOT, 0) == 0 || mkdir(DEFAULT_ROOT, FULL_ACCESS) == 0) {
			printf("Create the root directory %s.\n", DEFAULT_ROOT);
		    strcpy(directory, DEFAULT_ROOT);
		}
	}
}
