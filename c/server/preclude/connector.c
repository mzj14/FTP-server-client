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

#include "debug.h"
#include "connector.h"
#include "const.h"

int connectSocketToDest(char* dest_ip, int dest_port) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char send_msg[MAXDATASIZE];
	
	printf("The dest socket IP is %s\n", dest_ip);
    printf("The dest socket port is %d\n", dest_port);  
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	char dest_port_str[10];
	sprintf(dest_port_str, "%d", dest_port);
    
    printf("1\n");
	
	if ((rv = getaddrinfo(dest_ip, dest_port_str, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}
    printf("2\n");
	
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
	printf("3\n");
	
    freeaddrinfo(servinfo); // all done with this structure
	
	printf("4\n");
	if (p == NULL) {
		printf("5\n");
		fprintf(stderr, "failed to connect to dest.\n");
		return -1;
	} else {
		printf("6\n");
		return sockfd;
	}
}