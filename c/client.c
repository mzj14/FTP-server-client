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

#include <arpa/inet.h>

#define PORT "9000" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  char *msg;

  if (argc != 2) {
      fprintf(stderr,"usage: client hostname\n");
      exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
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
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
      s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  while (1) {
    printf("loop begin again\n");
    memset(buf, 0, sizeof buf);
    printf("Begin reading from the socket.\n");
    // if there is no message sent from the client, the process will hang on the recv()
    numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
    printf("Finish reading from the socket.\n");
    if (numbytes == -1) {
      perror("recv error");
      exit(1);
    }
    buf[numbytes] = '\0';
    printf("The recieve message is %s\n", buf);
    
    if (numbytes > 0) {
      if (buf[0] == '2') {
        if (send(sockfd, "USER", 4, 0) == -1) {
          perror("send");
        } else {
          printf("Successfully send the USER message.\n");
        }
      }

      if (buf[0] == '3') {
        if (send(sockfd, "Password", 4, 0) == -1) {
          perror("send");
        } else {
          printf("Successfully send the password message.\n");
        }
      }
    }
    
    getchar();
  }

  close(sockfd);

  return 0;
}
