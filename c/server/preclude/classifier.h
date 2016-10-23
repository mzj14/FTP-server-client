#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#define COMMAND_NUM 10

#define USER_VERB "USER"
#define PASS_VERB "PASS"
#define SYST_VERB "SYST"
#define TYPE_VERB "TYPE"
#define QUIT_VERB "QUIT"
#define ABOR_VERB "ABOR"
#define PORT_VERB "PORT"
#define RETR_VERB "RETR"
#define PASV_VERB "PASV"
#define STOR_VERB "STOR"

#define PORT_REGEX "^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9]),){5}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$"

/* 
  Recieve: req as the request string from client
  Return : the return int stands for the type of the request(e.g. USER, PASS)
           if int equals 0, then error represents the error type
*/           
extern int getRequestCategory(char* verb, char* parameter, char* error_msg);

#endif