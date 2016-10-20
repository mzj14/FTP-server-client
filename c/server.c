/*
** server.c -- a stream socket server demo
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

#include "preclude/const.h"
#include "preclude/separator.h"
#include "preclude/classifier.h"
#include "preclude/handler.h"
#include "preclude/sender.h"
#include "preclude/receiver.h"
#include "preclude/client_data.h"

#define PORT "9000"  // the port users will be connecting to

#define BACKLOG 10  // how many pending connections queue will hold

#define MAXDATASIZE 100

void* handleConnection(void* clientPtr);

int main(void)
{
	int sockfd;
  
    /************************************ Select a proper socket for server to use******************************/

	struct addrinfo hints, *servinfo, *p;
	int rv, yes = 1;
	
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // restrict socket to IPV4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
		    perror("setsockopt");
		    exit(1);
		}

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
           close(sockfd);
           perror("server: bind");
           continue;
        }
        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    } else {
	    printf("The server socket IP is %s\n", inet_ntoa(((struct sockaddr_in*)p->ai_addr)->sin_addr));
        printf("The server socket port is %d\n", ntohs(((struct sockaddr_in*)p->ai_addr)->sin_port)); 	
    }
  
    freeaddrinfo(servinfo); // all done with this structure
  
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    
	
	
	
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
	    printf("The incoming client socket IP is %s\n", client_ip);
        printf("The incoming client socket port is %d\n", client_port);
		printf("Use socket No.%d to play with it.\n", new_fd);
		
		if ((new_client = searchClientWithIP(client_head, client_ip, client_port)) == NULL) {
			new_client = createClientWithData(client_head, client_ip, client_port, new_fd);
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
	// deleteAllClient(client_head);
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
	if (sendAll(client->sockfd[COMMAND], WELCOME_MESSAGE, strlen(WELCOME_MESSAGE)) == -1) {
		perror("send");
	}
	
	while(1) {
		// printf("000");
		memset(buf, 0, sizeof buf);
		memset(verb, 0, sizeof verb);
		memset(parameter, 0, sizeof parameter);
		memset(error_msg, 0, sizeof error_msg);
		memset(send_msg, 0, sizeof send_msg);
  
		// if there is no message sent from the client, the process will hang on the recv()
		receive_result = recvAll(client->sockfd[COMMAND], buf, MAXDATASIZE - 1);
		if (receive_result == -1) {
			perror("recv error");
			exit(1);
		}
  
		printf("%s", buf);
  
		separateRequest(buf, verb, parameter);
  
		printf("Verb:%s\n", verb);
		printf("Parameter:%s\n", parameter);
  
		cat = getRequestCategory(verb, parameter, error_msg);
  
		handleRequest(cat, parameter, error_msg, send_msg, client);
  
		printf("cat = %d\n", cat);
		printf("send_msg = %s\n", send_msg);
  
        printf("client.ip = %s\n", client->ip);
		printf("client.port = %d\n", client->port[COMMAND]);
		printf("client.socket = %d\n", client->sockfd[COMMAND]);
		printf("client.status = %d\n", client->status);
		printf("client.password = %s\n", client->password);
  
		if (sendAll(client->sockfd[COMMAND], send_msg, strlen(send_msg)) == -1) {
			perror("send");
		}
		
		if (client->status == OFFLINE) {
			close(client->sockfd[COMMAND]);
			printf("Socket No.%d is closed by client at %s:%d\n", client->sockfd[COMMAND], client->ip, client->port[COMMAND]);
			/*
			if (strcmp(client->password, BLANK_PASSWORD) == 0) {
				free(client);
			}
			*/
			printf("mark1");
			break;
		}
	}
}
