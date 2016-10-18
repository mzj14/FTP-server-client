#ifndef CLASSIFIER_H
#define CLASSIFIER_H

/* 
  Recieve: req as the request string from client
  Return : the return int stands for the type of the request(e.g. USER, PASS)
           if int equals 0, then error represents the error type
*/           
extern int getRequestCategory(char* verb, char* parameter, char* error_msg);

#endif