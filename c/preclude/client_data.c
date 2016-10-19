#include <string.h>
#include <stdlib.h>

#include "client_data.h"
#include "const.h"

client_data* createClientWithData(client_data* head, char* ip, int port, int sockfd) {
	client_data* client =(client_data*)malloc(sizeof(client_data));
	
    strcpy(client->ip, ip);
	strcpy(client->password, BLANK_PASSWORD);
	client->port = port;
	client->sockfd = sockfd;
    client->status = UN_LOG;
	client->next = head;
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
		if (strcmp(node->ip, ip) == 0 && node->port == port) {
			return node;
		}
		node = node->next;
	}
	return NULL;
} 

