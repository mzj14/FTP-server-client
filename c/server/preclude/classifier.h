#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#define PORT_REGEX "^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9]),){5}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$"

/* 
  Recieve: req as the request string from client
  Return : the return int stands for the type of the request(e.g. USER, PASS)
           if int equals 0, then error represents the error type
*/           
extern int getRequestCategory(char* verb, char* parameter, char* error_msg);


extern int checkParamWithRegex(char* parameter, char* pattern);

#endif