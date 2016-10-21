#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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

#include "binder.h"
#include "client_data.h"

void getLocalIp(char* result_ip) {
	char hname[128];
	struct hostent *hent;
	gethostname(hname, sizeof(hname));
	hent = gethostbyname(hname);
	strcpy(result_ip, inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0])));
}

int bindSocketWithLocal(char* dest_ip, int dest_port, int connection_num, char* result_ip, int* result_port) {
    int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv, yes = 1;
	static int random_num = 10000;
	
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // restrict socket to IPV4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	char dest_port_str[10];
	
	// bind to specific port
	if (dest_port != INVALID_PORT) {
		sprintf(dest_port_str, "%d", dest_port);
	} else {
		// printf("r = %d", random_num);
		// sprintf(dest_port_str, "%d", random_num);
		// random_num++;
		sprintf(dest_port_str, "%d", random_num);
		random_num = random_num % 65536 + 1;
	}
	
    if ((rv = getaddrinfo(dest_ip, dest_port_str, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            perror("local: socket");
            continue;
        }

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
		    perror("setsockopt");
		    return -1;
		}

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
           close(sockfd);
           perror("local: bind");
           continue;
        }
        break;
    }

	
    if (p == NULL)  {
        fprintf(stderr, "local: failed to bind\n");
        return -1;
    }
	
	// we need to know which ip and port did the socket bind
	if (dest_port == INVALID_PORT) {
		getLocalIp(result_ip);
		*result_port = ntohs(((struct sockaddr_in*)p->ai_addr)->sin_port);
	}
	
	freeaddrinfo(servinfo); // all done with this structure
  
    if (listen(sockfd, connection_num) == -1) {
        perror("listen");
        return -1;
    }

    printf("local: waiting for connections...\n");
	return sockfd;
}