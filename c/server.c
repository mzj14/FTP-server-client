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

#define PORT "9000"  // the port users will be connecting to

#define BACKLOG 10  // how many pending connections queue will hold

#define MAXDATASIZE 100

void sigchld_handler(int s)
{
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while(waitpid(-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
  hints.ai_family = AF_UNSPEC;
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

  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
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
    
    inet_ntop(their_addr.ss_family,
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s);
      
    printf("server: got connection from %s\n", s);
    
    client_node.status = UN_LOG;
    // printf("-1");
    
    if (send(new_fd, WELCOME_MESSAGE, strlen(WELCOME_MESSAGE), 0) == -1) {
      perror("send");
    }
    /*    
    else {
      printf("Welcome message has been successfully send out");  
    }
    */
    // printf("sizeof buf = %lu", sizeof buf);
    
    while(1) {
      // printf("000");
      memset(buf, 0, sizeof buf);
      memset(verb, 0, sizeof verb);
      memset(parameter, 0, sizeof parameter);
      memset(error_msg, 0, sizeof error_msg);
      memset(send_msg, 0, sizeof send_msg);
      
      // if there is no message sent from the client, the process will hang on the recv()
      numbytes = recv(new_fd, buf, MAXDATASIZE - 1, 0);
      // printf("numbytes = %d\n", numbytes);
      // printf("111");
      // buf[numbytes] = '\0';
      if (numbytes == -1) {
        perror("recv error");
        exit(1);
      }
      
      
      printf("Receive message:%s\n", buf);
      
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
