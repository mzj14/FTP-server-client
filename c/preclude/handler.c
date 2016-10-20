#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "handler.h"
#include "const.h"

void splitPortParam(char* parameter, char* ip, int* port) {
	printf("111");
	char *ptr = parameter, *ptr_r = ptr;
	char first_num[10], second_num[10];
	
	for (int i = 0; i < 4; i++) {
		ptr = strchr(ptr, ',');
		*ptr = '.';
	}
	*ptr = '\0';
	
	// get the ip
	strcpy(ip, parameter);
	printf("ip = %s", ip);
	// get the port number
	ptr++;
	ptr_r = strchr(ptr, ',');
	*ptr_r = '\0';
	ptr_r++;
	
	strcpy(first_num, ptr);
    printf("first_num = %s\n", first_num);
	
	strcpy(second_num, ptr_r);
	printf("second_num = %s\n", second_num);
	
	*port = atoi(first_num) * 256 + atoi(second_num);
	printf("port = %d", *port);
	
	return;
}

void handleRequest(int type, char* parameter, char* error_msg, char* send_msg, client_data* client) {
    // error command
    if (type == INVALID_COMMAND) {
        strcpy(send_msg, error_msg);
    }
    
    // USER command
    if (type == USER_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_USER_MSG);
            setClientStatus(client, USER_STATUS);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_USER_MSG);
        } else {
            strcpy(send_msg, LOG_IN_USER_MSG);
        }
    }
    
    // PASS command
    if (type == PASS_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_PASS_MSG);
        } else if (client->status == USER_STATUS) {
            if (strcmp(client->password, BLANK_PASSWORD) == 0) {
				// new member
				strcpy(send_msg, USER_PASS_MSG_1);
				setClientStatus(client, LOG_IN);
				setClientPassword(client, parameter);
				appendClientNode(client);
			} else if (strcmp(client->password, parameter) == 0) {
				// old member
				strcpy(send_msg, USER_PASS_MSG_1);
				setClientStatus(client, LOG_IN);
			} else {
				// invalid member
				strcpy(send_msg, USER_PASS_MSG_2);
			}
        } else {
            setClientPassword(client, parameter);
            sprintf(send_msg, LOG_IN_PASS_MSG, parameter);
        }
    }
    
    // SYST command
    if (type == SYST_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
            strcpy(send_msg, LOG_IN_SYST_MSG);
        }
    }
    
    // TYPE command
    if (type == TYPE_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
            strcpy(send_msg, LOG_IN_TYPE_MSG);
        }
    }
    
    // QUIT or ABOR command
    if (type == QUIT_COMMAND || type == ABOR_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
            strcpy(send_msg, LOG_IN_QUIT_MSG);
			setClientStatus(client, OFFLINE);
        }
    }
	
	// PORT command
	if (type == PORT_COMMAND) {
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			splitPortParam(parameter, client->ip[DATA], &client->port[DATA]);
			strcpy(send_msg, LOG_IN_PORT_MSG);
		}	
	}

    return;
}