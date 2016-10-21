#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sender.h"

int sendAll(int s, char* buf, int len) {
	int total = 0;
	int bytesleft = len;
	int n;
	while (total < len) {
		n = send(s, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft = -n;
	}
	send(s, END_STRING, strlen(END_STRING), 0);
	return n == -1 ? -1 : 0; 
}

long int sendFile(int sockfd, char* file_name) {
	int fd = open(file_name, O_RDONLY);  
    off_t offset = 0;  
    struct stat fileStatus;  
    int fstat_ret = fstat(fd, &fileStatus);
	
	ssize_t length = sendfile(sockfd, fd, &offset, fileStatus.st_size);

    return length;	
}