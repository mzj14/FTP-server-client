#include <sys/types.h>
#include <sys/socket.h>

#include "receiver.h"

int recvAll(int s, char* buf, int size) {
	int total = 0; // the number of bytes we receive
	int bytesleft = size; // the number of bytes unused in buffer
	int n;
	do {
		n = recv(s, buf + total, bytesleft, 0);
		if (n == -1) {
			break;
		}
		total += n;
		bytesleft = -n;
	} while (buf[total - 1] != END_CHAR);
	
    if (n != -1) {
		buf[total - 1] = '\0';
	}
	return n == -1 ? -1 : 0; 
}