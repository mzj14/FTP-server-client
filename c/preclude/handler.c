#include <stdio.h>
#include <string.h>
#include "handler.h"
#include "const.h"

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
    return;
}