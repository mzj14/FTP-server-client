#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "const.h"
#include "binder.h"

// get local machine's ip and store it in result_ip
void getLocalIp(char* result_ip) {
	
	struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

	// choose the second one as the result_ip
	int second = 0;
	
    while (ifAddrStruct!=NULL) {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, result_ip, INET_ADDRSTRLEN);
			second++;
			if (second == 2) {
				break;
			}	
        }
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
    return;
	
}

int bindSocketWithLocal(char* dest_ip, int dest_port, int connection_num, char* result_ip, int* result_port) {
	
    int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv, yes = 1;
	static int random_num = BEGIN_PORT;  // random_port will loop between BEGIN_PORT and TOTAL_PORT_NUM - 1
	
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // restrict socket to IPV4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	char dest_port_str[10];
	
	if (dest_port != INVALID_PORT) {
		// bind to specific port
		sprintf(dest_port_str, "%d", dest_port);
	} else {
		// bind to random_port
		sprintf(dest_port_str, "%d", random_num);
		if (random_num == TOTAL_PORT_NUM - 1) {
			random_num == BEGIN_PORT;
		}
	}
	
    if ((rv = getaddrinfo(dest_ip, dest_port_str, &hints, &servinfo)) != 0) {
		// fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            // perror("local: socket");
            continue;
        }

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
		    // perror("setsockopt");
		    return -1;
		}

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
           close(sockfd);
           // perror("local: bind");
           continue;
        }
        break;
    }

	
    if (p == NULL)  {
        // fprintf(stderr, "local: failed to bind\n");
        return -1;
    }
	
	// we need to know which ip and port the socket binded
	if (dest_port == INVALID_PORT) {
		getLocalIp(result_ip);
		*result_port = ntohs(((struct sockaddr_in*)p->ai_addr)->sin_port);
	}
	
	freeaddrinfo(servinfo); // all done with this structure
  
    if (listen(sockfd, connection_num) == -1) {
        // perror("listen");
        return -1;
    }

	return sockfd;
	
}