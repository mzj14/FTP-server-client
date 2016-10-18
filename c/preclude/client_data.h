#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#define PASSWORD_SIZE 30

typedef struct {
    int status;
    char password[PASSWORD_SIZE];
} client_data;

extern void initClientData(client_data* client);

#endif