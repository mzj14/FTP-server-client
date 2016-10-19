/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>

#include "preclude/modify.h"
#include "preclude/sender.h"
#include "preclude/receiver.h"

#define IP_ADDRESS "10.0.2.15" // the ip address client will be connecting to

#define PORT "9000" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

int main(int argc, char *argv[])
{
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  char send_msg[MAXDATASIZE];
  /*
  if (argc != 2) {
      fprintf(stderr,"usage: client hostname\n");
      exit(1);
  }
  */
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
    
    // client now get msg from command line
    fgets(send_msg, MAXDATASIZE - 2, stdin);
    
    modifyTail(send_msg);
    
    // printf("The command is %s.", send_msg);
    
    // printf("len is %lu", strlen(send_msg));
    
	if (sendAll(sockfd, send_msg, strlen(send_msg)) == -1) {
	  perror("send");
	}
  }
  
  close(sockfd);

  return 0;
}
