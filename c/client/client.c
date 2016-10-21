/*
** client.c -- a stream socket client demo
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

#define IP_ADDRESS "10.0.2.15" // the ip address client will be connecting to

#define PORT 9000 // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

#define BACKLOG 10 

int main(int argc, char *argv[])
{
  int sockfd;  
  char buf[MAXDATASIZE];
  // struct addrinfo hints, *servinfo, *p;
  // int rv;
  // char s[INET6_ADDRSTRLEN];
  char send_msg[MAXDATASIZE], verb[MAXDATASIZE], parameter[MAXDATASIZE], error_msg[MAXDATASIZE];
  char data_connect_ip[MAXDATASIZE];
  int data_connect_port = INVALID_PORT;
  int data_sockfd = INVALID_SOCKFD;
  int new_data_sockfd = INVALID_SOCKFD;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size = sizeof(struct sockaddr_in);
  /*
  if (argc != 2) {
      fprintf(stderr,"usage: client hostname\n");
      exit(1);
  }
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(IP_ADDRESS, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  } else {
	printf("The server socket IP is %s\n", inet_ntoa(((struct sockaddr_in*)p->ai_addr)->sin_addr));
    printf("The server socket port is %d\n", ntohs(((struct sockaddr_in*)p->ai_addr)->sin_port));  
  }

  freeaddrinfo(servinfo); // all done with this structure
  */
  
  sockfd = connectSocketToDest(IP_ADDRESS, PORT);
  
  
  while (1) {
    // printf("loop begin again\n");
    memset(buf, 0, sizeof buf);
    // printf("Begin from the socket.\n");
    // if there is no message sent from the client, the process will hang on the recv()

    // printf("Finish reading from the socket.\n");
    if (recvAll(sockfd, buf, MAXDATASIZE - 1) == -1) {
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
			data_sockfd = bindSocketWithLocal(data_connect_ip, data_connect_port, BACKLOG);
	}
    
    // client now get msg from command line
    fgets(send_msg, MAXDATASIZE - 2, stdin);
    modifyTail(send_msg);
    
	separateRequest(send_msg, verb, parameter);
	int cat = getRequestCategory(verb, parameter, error_msg);
	
	if (sendAll(sockfd, send_msg, strlen(send_msg)) == -1) {
	    perror("send");
	} else {
		printf("jjjj");
		printf("cat = %d\n", cat);
		printf("data_sockfd = %d\n", data_sockfd);
		
		if (cat == RETR_COMMAND && data_sockfd != INVALID_SOCKFD) {
			printf("I am going to accept.\n");
			new_data_sockfd = accept(data_sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if (new_data_sockfd == -1) {
				perror("accept data connection error");
			} else {
				recvFile(new_data_sockfd, "maybe.txt");
				close(data_sockfd);
				data_sockfd = INVALID_SOCKFD;
			}
		}
	}
  }
  
  close(sockfd);

  return 0;
}
