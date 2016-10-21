#ifndef HANDLER_H
#define HANDLER_H

#include "client_data.h"

extern void handleRequest(int type, char* parameter, char* error_msg, char* send_msg, client_data* client);

extern void splitPortParam(char* parameter, char* ip, int* port);

#endif