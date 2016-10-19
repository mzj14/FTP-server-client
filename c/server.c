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

int main(void)
{
  int sockfd, new_fd, numbytes;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv; 
  char buf[MAXDATASIZE], verb[MAXDATASIZE], parameter[MAXDATASIZE], error_msg[MAXDATASIZE], send_msg[MAXDATASIZE];
  client_data client_node;
  initClientData(&client_node);

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

  // if there is no connection income, the process will stop the while loop
  while(1) {  // main accept() loop
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    
    if (new_fd == -1) {
      perror("accept");
      continue;
    }
	
	printf("The client socket IP is %s\n", inet_ntoa(((struct sockaddr_in*)&their_addr)->sin_addr));
    printf("The client socket port is %d\n", ntohs(((struct sockaddr_in*)&their_addr)->sin_port));
	
    client_node.status = UN_LOG;
    
    if (sendAll(new_fd, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE)) == -1) {
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
      if (recvAll(new_fd, buf, MAXDATASIZE - 1) == -1) {
        perror("recv error");
        exit(1);
      }
      
      printf("%s", buf);
      
      separateRequest(buf, verb, parameter);
      
      printf("Verb:%s\n", verb);
      printf("Parameter:%s\n", parameter);
      
	  int cat = getRequestCategory(verb, parameter, error_msg);
      
      handleRequest(cat, parameter, error_msg, send_msg, &client_node);
      
      printf("cat = %d\n", cat);
      printf("send_msg = %s\n", send_msg);
	  
	  printf("client.status = %d", client_node.status);
	  printf("client.password = %s", client_node.password);
      
      if (sendAll(new_fd, send_msg, strlen(send_msg)) == -1) {
        perror("send");
      }
    }
  }
  return 0;
}
