#ifndef PARAM_GETTER_H
#define PARAM_GETTER_H

/* get file name from message */
extern void getFileParam(char* msg, char* file_name);

/* get ip and port from message */
extern void getPortParamFrom(char* msg, char* ip, int* port);

#endif