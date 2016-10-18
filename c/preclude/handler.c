#include <stdio.h>
#include <string.h>
#include "handler.h"

void handleRequest(int type, char* parameter, char* error_msg, char* send_msg) {
    // error command
    if (type == 0) {
        strcpy(send_msg, error_msg);
        return;
    }
    
    // SYST command
    if (type == 3) {
        strcpy(send_msg, "215 UNIX Type: L8\r\n");
        return;
    }
    
    // TYPE command
    if (type == 4) {
        strcpy(send_msg, "200 Type set to I.\r\n");
        return;
    }
    
    // QUIT command
    if (type == 5) {
        strcpy(send_msg, "221-Thank you for using the FTP service.\r\n221 Goodbye.\r\n");
        return;
    }
    strcpy(send_msg, "Got the reply?\r\n");
    return;
}