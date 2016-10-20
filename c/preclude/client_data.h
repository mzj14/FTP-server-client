#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#define MEMBER_SIZE 30
#define BLANK_PASSWORD "$"

#define COMMAND 0
#define DATA 1
#define CONNECTION_TYPE_NUM 2

#define INVALID_PORT -1
#define INVALID_SOCKFD -1

typedef struct client_data {
	char ip[MEMBER_SIZE];
	int port[CONNECTION_TYPE_NUM]; // client port for command connection and data connection
    char password[MEMBER_SIZE];
	int sockfd[CONNECTION_TYPE_NUM]; // server socket for two kind connection
	int status;
	struct client_data* next;
} client_data;

extern client_data* createClientWithData(client_data* head, char* ip, int port, int sockfd);

extern void setClientPassword(client_data* client, char* password);

extern void setClientStatus(client_data* client, int status);

extern void appendClientNode(client_data* client);

extern void deleteAllClient(client_data* head);

extern client_data* searchClientWithIP(client_data* head, char* ip, int port);

#endif