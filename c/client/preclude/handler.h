#ifndef HANDLER_H
#define HANDLER_H

#include "client_data.h"

/* type£ºthe command type
   parameter: the parameter in the command
   error_msg: store some message if the command has syntax error 
   send_msg: the server's response to this command
   client: a pointer to some client data
*/
   
extern void handleRequest(int type, char* parameter, char* error_msg, char* send_msg, client_data* client);

#endif