#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#define MEMBER_SIZE 30
#define BLANK_PASSWORD "$"
#define FAKE_IP "$"
#define FAKE_SOCKET -1

typedef struct client_data{
	char ip[MEMBER_SIZE];
	int port;
    char password[MEMBER_SIZE];
	int sockfd;
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