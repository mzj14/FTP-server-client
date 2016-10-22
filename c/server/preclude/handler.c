#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "handler.h"
#include "const.h"
#include "connector.h"
#include "sender.h"
#include "binder.h"
#include "receiver.h"
#include "debug.h"

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

void splitPortParam(char* parameter, char* ip, int* port) {
	// printf("111");
	char *ptr = parameter, *ptr_r = ptr;
	char first_num[10], second_num[10];
	
	for (int i = 0; i < 4; i++) {
		ptr = strchr(ptr, ',');
		*ptr = '.';
	}
	*ptr = '\0';
	
	// get the ip
	strcpy(ip, parameter);
	// printf("ip = %s", ip);
	
	// get the port number
	ptr++;
	ptr_r = strchr(ptr, ',');
	*ptr_r = '\0';
	ptr_r++;
	
	strcpy(first_num, ptr);
    // printf("first_num = %s\n", first_num);
	
	strcpy(second_num, ptr_r);
	// printf("second_num = %s\n", second_num);
	
	*port = atoi(first_num) * 256 + atoi(second_num);
	// printf("port = %d", *port);
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
            strcpy(send_msg, UN_LOG_USER_MSG);
            setClientStatus(client, USER_STATUS);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_USER_MSG);
        } else {
            strcpy(send_msg, LOG_IN_USER_MSG);
        }
    }
    
    // PASS command
    if (type == PASS_COMMAND) {
        if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_PASS_MSG);
        } else if (client->status == USER_STATUS) {
            if (strcmp(client->password, BLANK_PASSWORD) == 0) {
				// new member
				strcpy(send_msg, USER_PASS_MSG_1);
				setClientStatus(client, LOG_IN);
				setClientPassword(client, parameter);
				appendClientNode(client);
			} else if (strcmp(client->password, parameter) == 0) {
				// old member
				strcpy(send_msg, USER_PASS_MSG_1);
				setClientStatus(client, LOG_IN);
			} else {
				// invalid member
				strcpy(send_msg, USER_PASS_MSG_2);
			}
        } else {
            setClientPassword(client, parameter);
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
			setClientStatus(client, OFFLINE);
        }
    }
	
	// PORT command
	if (type == PORT_COMMAND) {
		f_write("I am in the port command.\n");
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			if (client->sockfd[DATA] != INVALID_SOCKFD) {
				close(client->sockfd[DATA]);
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				client->sockfd[DATA] = INVALID_SOCKFD;
			}
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
			if (client->sockfd[DATA] != INVALID_SOCKFD) {
				close(client->sockfd[DATA]);
				strcpy(client->ip[DATA], INVALID_IP);
				client->port[DATA] = INVALID_PORT;
				client->sockfd[DATA] = INVALID_SOCKFD;
			}

			do {
				client->sockfd[DATA] = bindSocketWithLocal(NULL, INVALID_PORT, 10, client->ip[DATA], &client->port[DATA]);
			} while (client->sockfd[DATA] == INVALID_SOCKFD);
			
			client->transmode = PASV_TRANS;
			
			char portParam[MAXDATASIZE];
			combinePortParam(portParam, client->ip[DATA], client->port[DATA]);
			
			sprintf(send_msg, LOG_IN_PASV_MSG, portParam);
		}
	}
	
	// RETR command
	if (type == RETR_COMMAND) {
		f_write("I am in the port command.\n");
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			char sss[100];
			sprintf(sss, "client.transmode = %d\n", client->transmode);
		    f_write(sss);
			if (client->transmode == INVALID_TRANS) {
				// server does not receive data port from client
				strcpy(send_msg, LOG_IN_RETR_MSG_1);
			} else {			
				int	data_sockfd;
				if (client->transmode == PORT_TRANS) {
					data_sockfd = connectSocketToDest(client->ip[DATA], client->port[DATA]);
				} else {
					data_sockfd = accept(client->sockfd[DATA], NULL, NULL);
				}
				
				/*
				f_write11(client->ip[DATA]);
				char str_11[10];
				sprintf(str_11, "%d\n", client->port[DATA]);
				
				f_write11(str_11);
				sprintf(str_11, "data_sockfd = %d\n", data_sockfd);
				f_write11(str_11);
				*/
				
				if (data_sockfd == INVALID_SOCKFD) {
					strcpy(send_msg, LOG_IN_RETR_MSG_4);
				} else {
					char file_path[MAXDATASIZE];
					strcpy(file_path, client->root_directory);
	
					int root_len = strlen(client->root_directory);
					file_path[root_len] = '/';
					file_path[root_len + 1] = '\0';

					strcat(file_path, parameter);
					
					printf("file_path = %s\n", file_path);
					
					if ((access(file_path, 0)) == -1) {
						printf("cant find the file");
						// can not find the file
						strcpy(send_msg, LOG_IN_RETR_MSG_2);
					} else {
						if ((access(file_path, 4)) == -1) {
							// can not read the file
							printf("cant read the file");
							strcpy(send_msg, LOG_IN_RETR_MSG_3);
						} else {		
							// send file
							long int file_len = sendFile(data_sockfd, client->root_directory, parameter);
							f_write("Begin to send file.\n");
							if (file_len == -1) {
								// send file failed
								strcpy(send_msg, LOG_IN_RETR_MSG_5);
							} else {
								// send file succeed
								f_write("I am here1111!");
								// sprintf(send_msg, LOG_IN_RETR_MSG_6, file_len);
								strcpy(send_msg, LOG_IN_RETR_MSG_7);
							}
						}
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
	}
	
	// STOR command
	if (type == STOR_COMMAND) {
		if (client->status == UN_LOG) {
            strcpy(send_msg, UN_LOG_COMMON_MSG);
        } else if (client->status == USER_STATUS) {
            strcpy(send_msg, USER_COMMON_MSG);
        } else {
			if (client->transmode == INVALID_TRANS) {
				// server does not receive data port from client
				strcpy(send_msg, LOG_IN_STOR_MSG_1);
			} else {			
				int	data_sockfd;
				if (client->transmode == PORT_TRANS) {
					data_sockfd = connectSocketToDest(client->ip[DATA], client->port[DATA]);
				} else {
					data_sockfd = accept(client->sockfd[DATA], NULL, NULL);
				}
				if (data_sockfd == INVALID_SOCKFD) {
					strcpy(send_msg, LOG_IN_STOR_MSG_2);
				} else {
					char file_path[MAXDATASIZE];
					strcpy(file_path, client->root_directory);
	
					int root_len = strlen(client->root_directory);
					file_path[root_len] = '/';
					file_path[root_len + 1] = '\0';

					strcat(file_path, parameter);
					
					printf("file_path = %s\n", file_path);
					
					// has got a a file with the same name
					if ((access(file_path, 0)) != -1) {
						strcpy(send_msg, LOG_IN_STOR_MSG_3);
					} else {
						// recv file
						long int file_len = recvFile(data_sockfd, client->root_directory, parameter);
						if (file_len == -1) {
						// send file failed
							strcpy(send_msg, LOG_IN_STOR_MSG_4);
						} else {
							// send file succeed
							f_write("I am here1111!");
							sprintf(send_msg, LOG_IN_STOR_MSG_5, file_len); 
						}
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
	}
	
    return;
}