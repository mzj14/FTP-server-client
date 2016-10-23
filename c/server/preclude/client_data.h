#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#define MEMBER_SIZE 30
#define CONNECTION_TYPE_NUM 2
#define MAX_PATH_LEN 100

typedef struct client_data {
	
	char ip[CONNECTION_TYPE_NUM][MEMBER_SIZE]; // client ip for command connection and data connection
	
	int port[CONNECTION_TYPE_NUM]; // client port for command connection and data connection
	
    char password[MEMBER_SIZE];
	
	int sockfd[CONNECTION_TYPE_NUM]; // server socket for two kind connection
	
	int status; // client status : OFFLINE, UN_LOG, USER_STATUS, LOG_IN
	
	struct client_data* next;
	
	char root_directory[MAX_PATH_LEN]; // directory for client upload file
	
	int transmode; // file transmode: PORT or PASV
	
} client_data;

// initialize client data
extern client_data* createClientWithData(client_data* head, char* ip, int port, int sockfd, char* root_directory);

// add client node to the head of the link pointed by parameter client
extern void addClientNode(client_data* client);

// delete the link pointed by client
extern void deleteAllClient(client_data* head);

// search the link for client based on ip and port
extern client_data* searchClientWithIP(client_data* head, char* ip, int port);

#endif