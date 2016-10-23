#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>

#include "const.h"
#include "reply_classifier.h"

int getStatusCode(char* reply) {
	
	char dig[4];
	strncpy(dig, reply, 4);
	
	return atoi(dig);
	
}

void getPreviousVerb(char* verb, char* request) {
	
	strncpy(verb, request, 4);
	return;
	
}

int classifyReply(char* reply, char* previous_request) {
	
	char previous_verb[5];

	int reply_status = getStatusCode(reply);
	getPreviousVerb(previous_verb, previous_request);
	
	if (reply_status == 200 && strcmp(previous_verb, "PORT") == 0) {
		return SET_PORT;
	}

	if (reply_status == 227 && strcmp(previous_verb, "PASV") == 0) {
		return SET_PASV;
	}
	
	if (reply_status == 150 && strcmp(previous_verb, "RETR") == 0) {
		return BEGIN_RETR;
	}
	
	if (reply_status == 150 && strcmp(previous_verb, "STOR") == 0) {
		return BEGIN_STOR;
	}
	
	if (strcmp(previous_verb, "RETR") == 0 || strcmp(previous_verb, "STOR") == 0) {
		if (reply_status == 226) {
			return FILE_SUCC;
		} else {
			return FILE_FAIL;
		}
	}
	
	if (reply_status == 221 && strcmp(previous_verb, "QUIT") == 0) {
		return TO_QUIT;
	}
	
	return IGNORE;
	
}

void getFileParam(char* msg, char* file_name) {
	char* ptr = msg + 4;
	strcpy(file_name, ptr);
	int name_len = strlen(file_name);
	file_name[name_len - 1] = '\0';
	file_name[name_len - 2] = '\0';
	return;
}

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

// check if parameter meets the pattern in regular expression
// return 1 if meet, otherwise return 0-9
int checkParamWithRegex(char* parameter, char* pattern) {
    int status;
    regex_t reg;
    
    regcomp(&reg, pattern, REG_EXTENDED);
    status = regexec(&reg, parameter, 0, NULL, 0);
    regfree(&reg);
    
    if (status == REG_NOMATCH) {
        return 0;
    } else if (status == 0) {
        return 1;
    }
}
