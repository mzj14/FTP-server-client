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

#include "preclude/command_processor.h"
#include "preclude/binder.h"
#include "preclude/const.h"
#include "preclude/separator.h"
#include "preclude/classifier.h"
#include "preclude/handler.h"
#include "preclude/client_data.h"

// handle every client connection with data pointed by clientPtr in one thread
void* handleConnection(void* clientPtr);
 
int main(int argc, char* argv[])
{	
	/************************************ get command port and root directory from command ************************************/
	int command_port;
	char root_directory[MAXDATASIZE];
	
	getPortAndRoot(argc, argv, &command_port, root_directory);
	
	

  
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
		
		
		// if we can not find the client_ip from the client list, create a new client
		if ((new_client = searchClientWithIP(client_head, client_ip, client_port)) == NULL) {
			new_client = createClientWithData(client_head, client_ip, client_port, new_fd, root_directory);
		}
		
		pthread_t sniffer_thread;
		
		if (pthread_create(&sniffer_thread, NULL, handleConnection, (void*)new_client) < 0)
        {
            perror("could not create thread");
            return 1;
        }

    }
    
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
	
	
	while(1) {
		memset(buf, 0, sizeof buf);
		memset(verb, 0, sizeof verb);
		memset(parameter, 0, sizeof parameter);
		memset(error_msg, 0, sizeof error_msg);
		memset(send_msg, 0, sizeof send_msg);
		
		// if there is no message sent from the client, the process will hang on the recv()
		receive_result = recv(client->sockfd[COMMAND], buf, MAXDATASIZE - 1, 0);
		
		if (receive_result == -1) {
			perror("recv error");
			exit(1);
		}
		
		separateRequest(buf, verb, parameter);
  
		cat = getRequestCategory(verb, parameter, error_msg);
		
		handleRequest(cat, parameter, error_msg, send_msg, client);

		// send the response to client
		if (send(client->sockfd[COMMAND], send_msg, strlen(send_msg), 0) == -1) {
			perror("send");
		}
 
		// if the client quit
		if (client->status == OFFLINE) {
			// close the socket for command transport
			close(client->sockfd[COMMAND]);
			// if the client never login
			if (strcmp(client->password, BLANK_PASSWORD) == 0) {
				free(client);
			}
			break;
		}
	}
	
}
