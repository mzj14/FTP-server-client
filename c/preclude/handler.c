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
            client->status = USER_STATUS;
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
            if (client->password[0] == '\0' || strcmp(client->password, parameter) == 0) {
                strcpy(send_msg, USER_PASS_MSG_1);
                strcpy(client->password, parameter);
				client->status = LOG_IN;
            } else {
                strcpy(send_msg, USER_PASS_MSG_2);
            }
        } else {
            strcpy(client->password, parameter);
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
    
    // QUIT command
    if (type == QUIT_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
            strcpy(send_msg, LOG_IN_QUIT_MSG);
            client->status = UN_LOG;
        }
    }
    return;
}