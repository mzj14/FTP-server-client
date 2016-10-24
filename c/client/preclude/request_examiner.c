#include <string.h>
#include <stdio.h>

#include "const.h"
#include "request_examiner.h"
#include "file_accessor.h"

void getVerb(char* verb, char* request) {
	
	strncpy(verb, request, 4);
	verb[4] = '\0';
	return;
	
}

void getParam(char* msg, char* file_name) {
	
	char* ptr = msg + 5;
	strcpy(file_name, ptr);
	int file_name_len = strlen(file_name);
	if (file_name[file_name_len - 1] == '\n') {
		file_name[file_name_len - 1] = '\0';
	} 
	return;
	
}

int requestExamine(char* root_directory, char* send_msg) {
	
	char verb[5];
	char file_name[30];
	
	getVerb(verb, send_msg);
		
	if (strcmp(verb, "STOR") == 0) {
		getParam(send_msg, file_name);
		if (accessFile(root_directory, file_name, 0) == -1) {
			printf("The file you want to store to the server does not exist.\n");
			return UN_FEASIBLE;
		}
		
		if (accessFile(root_directory, file_name, 4) == -1) {
			printf("You don't have the right to read the file.\n");
			return UN_FEASIBLE;
		}
	}
	
	return FEASIBLE;
	
}