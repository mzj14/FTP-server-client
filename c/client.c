#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA "Hello, world of socket"

int main(int argc, char* argv[])
{
  /* Variables */
  int sock;
  struct sockaddr_in client;
  struct hostent *hp;
  // int mysock;
  char buff[1024];
  // int rval;
  
  
  
  /* Create socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("Failed to create socket");
    exit(1);
  }
  
  client.sin_family = AF_INET;
  hp = gethostbyname(argv[1]);
  
  if (hp == 0)
  {
    perror("Gethostbyname failed");
    close(sock);
    exit(1);
  }
  
  memcpy(&client.sin_addr, hp->h_addr, hp->h_length);
  printf("hp->h_addr : %d", client.sin_addr.s_addr);
  client.sin_port = htons(5000);
  
  
  /* Call bind */
  if (connect(sock, (struct sockaddr *) &client, sizeof(client)) < 0)
  {
    perror("Connect failed");
    close(sock);
    exit(1);
  }
  
  if (send(sock, DATA, sizeof(DATA), 0) < 0)
  {
    perror("Send failed");
    close(sock);
    exit(1);
  }
 
  printf("Sent %s\n", DATA);
  close(sock);
 
  return 0;
}