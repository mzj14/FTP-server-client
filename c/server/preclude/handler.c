#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "handler.h"
#include "const.h"
#include "connector.h"
#include "file_sender.h"
#include "binder.h"
#include "file_receiver.h"
#include "file_accessor.h"

// combine ip and port to command format string to portParam
void combinePortParam(char* portParam, char* ip, int port) {
	
	strcpy(portParam, ip);
	
	char* ptr = portParam;
	
	for (int i = 0; i < 3; i++) {
		ptr = strchr(ptr, '.');
		*ptr = ',';
	}
	
	int big_dig = port / 256;
	int small_dig = port % 256;
	
	char dig[5];
	sprintf(dig, ",%d", big_dig);
	strcat(portParam, dig);
	
	sprintf(dig, ",%d", small_dig);
	strcat(portParam, dig);
	
	return;
	
}

// split portParam to ip and port 
void splitPortParam(char* parameter, char* ip, int* port) {
	
	char *ptr = parameter, *ptr_r = ptr;
	char first_num[10], second_num[10];
	
	for (int i = 0; i < 4; i++) {
		ptr = strchr(ptr, ',');
		*ptr = '.';
	}
	*ptr = '\0';
	
	// get the ip
	strcpy(ip, parameter);
	
	// get the port number
	ptr++;
	ptr_r = strchr(ptr, ',');
	*ptr_r = '\0';
	ptr_r++;
	
	strcpy(first_num, ptr); 
	
	strcpy(second_num, ptr_r);
	
	*port = atoi(first_num) * 256 + atoi(second_num);

	return;
	
}


void handleRequest(int type, char* parameter, char* error_msg, char* send_msg, client_data* client) {
    // error command
    if (type == INVALID_COMMAND) {
        strcpy(send_msg, error_msg);
    }
    
    // USER command
    if (type == USER_COMMAND) {
        if (client->status == UN_LOG) {
			// change client status
            strcpy(send_msg, UN_LOG_USER_MSG);
            client->status = USER_STATUS;
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_USER_MSG);
        } else {
			// reset client user name
            strcpy(send_msg, LOG_IN_USER_MSG);
        }
    }
    
    // PASS command
    if (type == PASS_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_PASS_MSG);
        } else if (client->status == USER_STATUS) {
            if (strcmp(client->password, BLANK_PASSWORD) == 0) {
				// the client is a new member
				strcpy(send_msg, USER_PASS_MSG_1);
				client->status = LOG_IN;
				strcpy(client->password, parameter);
				addClientNode(client);
			} else if (strcmp(client->password, parameter) == 0) {
				// the client is an old member
				strcpy(send_msg, USER_PASS_MSG_1);
				client->status = LOG_IN;
			} else {
				// the client is an invalid member
				strcpy(send_msg, USER_PASS_MSG_2);
			}
        } else {
			// reset client password
            strcpy(client->password, parameter);
            sprintf(send_msg, LOG_IN_PASS_MSG, parameter);
        }
    }
    
    // SYST command
    if (type == SYST_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
            strcpy(send_msg, LOG_IN_SYST_MSG);
        }
    }
    
    // TYPE command
    if (type == TYPE_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
            strcpy(send_msg, LOG_IN_TYPE_MSG);
        }
    }
    
    // QUIT or ABOR command
    if (type == QUIT_COMMAND || type == ABOR_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
            strcpy(send_msg, LOG_IN_QUIT_MSG);
			// change client status
			client->status = OFFLINE;
        }
    }
	
	// PORT command
	if (type == PORT_COMMAND) {
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			// the previous data connection socket should be closed
			if (client->sockfd[DATA] != INVALID_SOCKFD) {
				close(client->sockfd[DATA]);
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				client->sockfd[DATA] = INVALID_SOCKFD;
			}
			// store new client ip and port for data connection
			splitPortParam(parameter, client->ip[DATA], &client->port[DATA]);
			client->transmode = PORT_TRANS;
			strcpy(send_msg, LOG_IN_PORT_MSG);
		}	
	}

	// PASV command
	if (type == PASV_COMMAND) {
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			// the previous data connection socket should be closed
			if (client->sockfd[DATA] != INVALID_SOCKFD) {
				close(client->sockfd[DATA]);
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				client->sockfd[DATA] = INVALID_SOCKFD;
			}

			// bind an available local socket
			do {
				client->sockfd[DATA] = bindSocketWithLocal(NULL, INVALID_PORT, BACKLOG, client->ip[DATA], &client->port[DATA]);
			} while (client->sockfd[DATA] == INVALID_SOCKFD);
			
			client->transmode = PASV_TRANS;
			
			// combine data socket ip and port to string
			char portParam[MAXDATASIZE];
			combinePortParam(portParam, client->ip[DATA], client->port[DATA]);
			
			sprintf(send_msg, LOG_IN_PASV_MSG, portParam);
		}
	}
	
	// RETR command
	if (type == RETR_COMMAND) {
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			// can not find the file
			if ((accessFile(client->root_directory, parameter, 0)) == -1) {
				strcpy(send_msg, LOG_IN_RETR_MSG_2);
				client->transmode = INVALID_TRANS;
				if (client->sockfd[DATA] != INVALID_SOCKFD) {
					close(client->sockfd[DATA]);
					client->sockfd[DATA] = INVALID_SOCKFD;	
				}
				// abandon the client data ip and port
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				return;
			}
			
			// can not read the file
			if ((accessFile(client->root_directory, parameter, 4)) == -1) {
				strcpy(send_msg, LOG_IN_RETR_MSG_3);
				if (client->sockfd[DATA] != INVALID_SOCKFD) {
					close(client->sockfd[DATA]);
					client->sockfd[DATA] = INVALID_SOCKFD;	
				}
				// abandon the client data ip and port
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				return;
			} 
			
			// send the mark response
			sprintf(send_msg, LOG_IN_MARK_MSG, parameter);
			if (send(client->sockfd[COMMAND], send_msg, strlen(send_msg), 0) == -1) {
				perror("send");
			}
			
			// server does not receive data port from client 
			if (client->transmode == INVALID_TRANS) {
				strcpy(send_msg, LOG_IN_RETR_MSG_1);
				if (client->sockfd[DATA] != INVALID_SOCKFD) {
					close(client->sockfd[DATA]);
					client->sockfd[DATA] = INVALID_SOCKFD;	
				}
				// abandon the client data ip and port
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				return;
			}
			
			int	data_sockfd;
			if (client->transmode == PORT_TRANS) {
				data_sockfd = connectSocketToDest(client->ip[DATA], client->port[DATA]);
			} else {
				data_sockfd = accept(client->sockfd[DATA], NULL, NULL);
			}
					
			if (data_sockfd == INVALID_SOCKFD) {
				strcpy(send_msg, LOG_IN_RETR_MSG_4);
			} else {
				// send file
				long int file_len = sendFile(data_sockfd, client->root_directory, parameter);
			
				if (file_len == -1) {
					// send file failed
					strcpy(send_msg, LOG_IN_RETR_MSG_5);
				} else {
					// send file succeed
					sprintf(send_msg, LOG_IN_RETR_MSG_6, file_len);
				}
			
				// close data socket
				close(data_sockfd);
				client->sockfd[DATA] = INVALID_SOCKFD;		
			}
				
			client->transmode = INVALID_TRANS;
			
			// abandon the client data ip and port
			strcpy(client->ip[DATA], INVALID_IP);
			client->port[DATA] = INVALID_PORT;
		}
	}
	
	// STOR command
	if (type == STOR_COMMAND) {
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			// has got a a file with the same name
			if ((accessFile(client->root_directory, parameter, 0)) != -1) {
				strcpy(send_msg, LOG_IN_STOR_MSG_3);
				if (client->sockfd[DATA] != INVALID_SOCKFD) {
					close(client->sockfd[DATA]);
					client->sockfd[DATA] = INVALID_SOCKFD;	
				}
				// abandon the client data ip and port
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				return;
			}
			
			// send the mark response
			sprintf(send_msg, LOG_IN_MARK_MSG, parameter);
			if (send(client->sockfd[COMMAND], send_msg, strlen(send_msg), 0) == -1) {
				perror("send");
			}
			
			if (client->transmode == INVALID_TRANS) {
				// server does not receive data port from client
				strcpy(send_msg, LOG_IN_STOR_MSG_1);
				if (client->sockfd[DATA] != INVALID_SOCKFD) {
					close(client->sockfd[DATA]);
					client->sockfd[DATA] = INVALID_SOCKFD;	
				}
				// abandon the client data ip and port
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				return;
			}
			
			int	data_sockfd;
			if (client->transmode == PORT_TRANS) {
				data_sockfd = connectSocketToDest(client->ip[DATA], client->port[DATA]);
			} else {
				data_sockfd = accept(client->sockfd[DATA], NULL, NULL);
			}
			
			if (data_sockfd == INVALID_SOCKFD) {
				strcpy(send_msg, LOG_IN_STOR_MSG_2);
			} else {
				// recv file
				long int file_len = recvFile(data_sockfd, client->root_directory, parameter);
				if (file_len == -1) {
					// send file failed
					strcpy(send_msg, LOG_IN_STOR_MSG_4);
				} else {
					// send file succeed
					sprintf(send_msg, LOG_IN_STOR_MSG_5, file_len); 
				}

				close(data_sockfd);
				client->sockfd[DATA] = INVALID_SOCKFD;
			}
			
			client->transmode = INVALID_TRANS;
			
			// abandon the client data ip and port
			strcpy(client->ip[DATA], INVALID_IP);
			client->port[DATA] = INVALID_PORT;
		}
	}
	
    return;
}