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
#include "preclude/file_sender.h"
#include "preclude/file_receiver.h"
#include "preclude/const.h"
#include "preclude/binder.h"
#include "preclude/reply_classifier.h"

#define IP "-ip" // ip argument in command
#define PORT "-port"  // the port argument in command
#define ROOT "-root"  // the root argument in command

#define DEFAULT_IP "127.0.0.1" 
#define IP_REGEX "^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$"
#define DEFAULT_PORT 21  
#define DEFAULT_ROOT "local"

#define FULL_ACCESS S_IRWXU|S_IROTH|S_IWOTH|S_IXOTH

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
	char file_name[MAXDATASIZE]; 
	int data_transmode = INVALID_TRANS;
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
		
		int cat = classifyReply(buf, send_msg);
		
		printf("cat = %d\n", cat);
		
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
		if (cat == FILE_FAIL) {
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
		}
		
		if (cat == BEGIN_RETR) {
			if (data_transmode == PORT_TRANS && data_sockfd != INVALID_SOCKFD) {
				accept(data_sockfd, NULL, NULL);
			}
			
			if (data_transmode == PASV_TRANS && data_sockfd != INVALID_SOCKFD) {
				data_sockfd = connectSocketToDest(data_connect_ip, data_connect_port);
			}
			
			if (recv(sockfd, buf, MAXDATASIZE - 1, 0) == -1) {
				perror("recv error");
				exit(1);
			}
			
			if (classifyReply(buf, send_msg) == FILE_SUCC) {
				recvFile(data_sockfd, root_directory, file_name);
			}
				
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
		}
		
		if (cat == BEGIN_STOR) {
			if (data_transmode == PORT_TRANS && data_sockfd != INVALID_SOCKFD) {
				new_data_sockfd = accept(data_sockfd, NULL, NULL);
			}
			if (data_transmode == PASV_TRANS && data_sockfd != INVALID_SOCKFD) {
				data_sockfd = connectSocketToDest(data_connect_ip, data_connect_port);
			}
			
			getFileParam(send_msg, file_name);
			
			sendFile(data_sockfd, root_directory, file_name);
			
			if (recv(sockfd, buf, MAXDATASIZE - 1, 0) == -1) {
				perror("recv error");
				exit(1);
			}
			
			if (data_sockfd != INVALID_SOCKFD) {
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
			
		}
		
	    memset(send_msg, 0, sizeof send_msg);
		
		// client now get msg from command line
		fgets(send_msg, MAXDATASIZE - 2, stdin);
		modifyTail(send_msg);
		
		if (send(sockfd, send_msg, strlen(send_msg), 0) == -1) {
			perror("send error");
			exit(1);
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
