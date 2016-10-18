#include <string.h>
#include "modify.h"

void modifyTail(char* msg) {
    int msglen = strlen(msg);
    msg[msglen - 1] = '\r';
    msg[msglen] = '\n';
    msg[msglen + 1] = '\0';
}