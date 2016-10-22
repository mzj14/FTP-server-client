#include <string.h>
#include <stdlib.h>

#include "client_data.h"
#include "const.h"
#include "debug.h"

client_data* createClientWithData(client_data* head, char* ip, int port, int sockfd, char* root_directory) {
	client_data* client =(client_data*)malloc(sizeof(client_data));
	
    strcpy(client->ip[COMMAND], ip);
	strcpy(client->ip[DATA], INVALID_IP);
	strcpy(client->password, BLANK_PASSWORD);
	client->port[COMMAND] = port;
	client->port[DATA] = INVALID_PORT;
	client->sockfd[COMMAND] = sockfd;
	client->sockfd[DATA] = INVALID_SOCKFD;
    client->status = UN_LOG;
	client->next = head;
	strcpy(client->root_directory, root_directory);
	client->transmode = INVALID_TRANS;
    return client;
}

void setClientPassword(client_data* client, char* password) {
	strcpy(client->password, password);
	return;
}

void setClientStatus(client_data* client, int status) {
	client->status = status;
	return;
}

void appendClientNode(client_data* client) {
	client_data* head = client->next;
    if (head) {
		client->next = head->next;
	}
	head = client;
	return;
}

void deleteAllClient(client_data* head) {
	client_data* next_node;
    while (head) {
		next_node = head->next;
		free(head);
		head = next_node;
	}	
	return;
}

client_data* searchClientWithIP(client_data* head, char* ip, int port) {
	client_data* node = head;
	while (node) {
	if (strcmp(node->ip[COMMAND], ip) == 0 && node->port[COMMAND] == port) {
			return node;
		}
		node = node->next;
	}
	return NULL;
} 

