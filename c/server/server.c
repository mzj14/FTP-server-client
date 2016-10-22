/*
** server.c -- a stream socket server
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "preclude/const.h"
#include "preclude/separator.h"
#include "preclude/classifier.h"
#include "preclude/handler.h"
#include "preclude/sender.h"
#include "preclude/receiver.h"
#include "preclude/client_data.h"
#include "preclude/binder.h"
#include "preclude/debug.h"

#define PORT "-port"  // the port argument in command
#define ROOT "-root"  // the root argument in command

#define TOTAL_PORT_NUM 65536
#define DEFAULT_PORT 21
#define DEFAULT_ROOT "/tmp"

#define FULL_ACCESS S_IRWXU|S_IROTH|S_IWOTH|S_IXOTH   // full right to the created folder

#define BACKLOG 10  // how many pending connections queue will hold


// get the port and root directory from command
void getPortAndRoot(int argc, char* argv[], int* port, char* directory);


// handle every client connection with data pointed by clientPtr
void* handleConnection(void* clientPtr);

char print_str[MAXDATASIZE];

FILE *fp = NULL;
 
int main(int argc, char* argv[])
{	
	/************************************ get command port and root directory from command ************************************/
	int command_port;
	char root_directory[MAXDATASIZE];
	getPortAndRoot(argc, argv, &command_port, root_directory);
	
	printf("port = %d\n", command_port);
	printf("root_directory = %s\n", root_directory);
	
	
	

  
    /************************************ Select a proper socket for server to use******************************/
	
    int sockfd = bindSocketWithLocal(NULL, command_port, BACKLOG, NULL, NULL);
	
	
    /**************** Loop, accept client connection, assign every incoming connection to a thread ***************************/
  
    struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size = sizeof(struct sockaddr_in);
	int new_fd, client_port;  // new connection on new_fd
	client_data *new_client, *client_head = NULL;
	char* client_ip = NULL;
	
    // if there is no connection income, the process will stop the while loop
    while(1) {
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    
		if (new_fd == -1) {
		  perror("accept");
		  break;
		}
	
		client_ip = inet_ntoa(((struct sockaddr_in*)&their_addr)->sin_addr);
		client_port = ntohs(((struct sockaddr_in*)&their_addr)->sin_port);
		
	    sprintf(print_str, "The incoming client socket IP is %s\n", client_ip);
		f_write(print_str);
		
        sprintf(print_str, "The incoming client socket port is %d\n", client_port);
		f_write(print_str);
		
		sprintf(print_str, "Use socket No.%d to play with it.\n", new_fd);
		f_write(print_str);
		
		if ((new_client = searchClientWithIP(client_head, client_ip, client_port)) == NULL) {
			new_client = createClientWithData(client_head, client_ip, client_port, new_fd, root_directory);
		}
		
		pthread_t sniffer_thread;
		
		if (pthread_create(&sniffer_thread, NULL, handleConnection, (void*)new_client) < 0)
        {
            perror("could not create thread");
            return 1;
        }
		pthread_join(sniffer_thread, NULL);
		printf("mark 2");
    }
    
	printf("mark 3");
	deleteAllClient(client_head);
    client_head = NULL;
	
    return 0;
}
	
/**************************** handle the connection within each thread ******************************/

void* handleConnection(void* clientPtr) {
	client_data* client = (client_data*)clientPtr;
	
	char buf[MAXDATASIZE],        // request received from the connection 
		 verb[MAXDATASIZE],       // verb in the request
		 parameter[MAXDATASIZE],  // parameters in the request
		 error_msg[MAXDATASIZE],  // the error message of syntax error 
		 send_msg[MAXDATASIZE];   // the reply message to the client
	
	int cat,                      // type of the request
		receive_result;           // result of receiving data from stream
		
	// send welcome message
	if (send(client->sockfd[COMMAND], WELCOME_MESSAGE, strlen(WELCOME_MESSAGE), 0) == -1) {
		perror("send");
	}
	
	f_write(WELCOME_MESSAGE);
	
	
	while(1) {
		// printf("000");
		memset(buf, 0, sizeof buf);
		memset(verb, 0, sizeof verb);
		memset(parameter, 0, sizeof parameter);
		memset(error_msg, 0, sizeof error_msg);
		memset(send_msg, 0, sizeof send_msg);
  
        f_write("1111");
		
		// if there is no message sent from the client, the process will hang on the recv()
		receive_result = recv(client->sockfd[COMMAND], buf, MAXDATASIZE - 1, 0);
		
		if (receive_result == -1) {
			f_write("wrong");
			perror("recv error");
			exit(1);
		}
  
		printf("%s", buf);
  
		f_write(buf);
		
		separateRequest(buf, verb, parameter);
  
		printf("Verb:%s\n", verb);
		printf("Parameter:%s\n", parameter);
  
		cat = getRequestCategory(verb, parameter, error_msg);
		printf("cat = %d\n", cat);
		
		handleRequest(cat, parameter, error_msg, send_msg, client);
  
		
		sprintf(print_str, "send_msg = %s\n", send_msg);
		f_write(print_str);
		
	    sprintf(print_str, "client.ip[COMMAND] = %s\n", client->ip[COMMAND]);
		f_write(print_str);
		sprintf(print_str, "client.ip[DATA] = %s\n", client->ip[DATA]);
		f_write(print_str);
		sprintf(print_str, "client.port[COMMAND] = %d\n", client->port[COMMAND]);
		f_write(print_str);
		sprintf(print_str, "client.port[DATA] = %d\n", client->port[DATA]);
		f_write(print_str);
		sprintf(print_str, "client.socket[COMMAND] = %d\n", client->sockfd[COMMAND]);
		f_write(print_str);
		
		sprintf(print_str, "client.socket[DATA] = %d\n", client->sockfd[DATA]);
		f_write(print_str);
		printf("client.status = %d\n", client->status);
		printf("client.password = %s\n", client->password);
		printf("client.root_directory = %s\n", client->root_directory);
		
		
		sprintf(print_str, "client.transmode = %d\n", client->transmode);
		f_write(print_str);

		if (send(client->sockfd[COMMAND], send_msg, strlen(send_msg), 0) == -1) {
			perror("send");
		}
	
 
		if (client->status == OFFLINE) {
			close(client->sockfd[COMMAND]);
			printf("Socket No.%d is closed by client at %s:%d\n", client->sockfd[COMMAND], client->ip[COMMAND], client->port[COMMAND]);
			if (strcmp(client->password, BLANK_PASSWORD) == 0) {
				free(client);
			}
			printf("mark1");
			break;
		}
	}
}

void getPortAndRoot(int argc, char* argv[], int* port, char* directory) {
	int give_root_flag = 0, give_port_flag = 0;
	
	for (int i = 0; i < argc; i++) {
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
