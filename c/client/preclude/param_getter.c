#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "param_getter.h"

// split portParam to ip and port 
void splitPortParam(char* parameter, char* ip, int* port) {
	
	char *ptr = parameter, *ptr_r = ptr;
	char first_num[10], second_num[10];
	
	for (int i = 0; i < 4; i++) {
		ptr = strchr(ptr, ',');
		*ptr = '.';
	}
	*ptr = '\0';
	
	// get the ip
	strcpy(ip, parameter);
	
	// get the port number
	ptr++;
	ptr_r = strchr(ptr, ',');
	*ptr_r = '\0';
	ptr_r++;
	
	strcpy(first_num, ptr); 
	
	strcpy(second_num, ptr_r);
	
	*port = atoi(first_num) * 256 + atoi(second_num);

	return;
	
}

void getPortParamFrom(char* msg, char* ip, int* port) {
	
	// find first comma
	char ip_port[30];
	char* ptr = strchr(msg, ',');
	ptr--;
	while (isdigit(*ptr)) {
		ptr--;
	}
	ptr++;
	strcpy(ip_port, ptr);
	int ip_port_len = strlen(ip_port);
	ip_port[ip_port_len - 1] = '\0';
	ip_port[ip_port_len - 2] = '\0';
	splitPortParam(ip_port, ip, port);
	
}

void getFileParam(char* msg, char* file_name) {
	
	char* ptr = msg + 5;
	strcpy(file_name, ptr);
	int name_len = strlen(file_name);
	file_name[name_len - 1] = '\0';
	file_name[name_len - 2] = '\0';
	return;
	
}