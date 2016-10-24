#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <ctype.h>

#include "const.h"
#include "reply_classifier.h"

int getStatusCode(char* reply) {
	
	char dig[4];
	strncpy(dig, reply, 4);
	
	return atoi(dig);
	
}

void getPreviousVerb(char* verb, char* request) {
	
	strncpy(verb, request, 4);
	return;
	
}

int singleLine(char* reply) {
	
	char* ptr = strchr(reply, '\n');
	ptr++;
	if (*ptr == '\0') {
		return 1;
	} else {
		return 0;
	}
	
}

int classifyReply(char* reply, char* previous_request) {
	
	char previous_verb[5];

	int reply_status = getStatusCode(reply);
	getPreviousVerb(previous_verb, previous_request);
	
	if (reply_status == 200 && strcmp(previous_verb, "PORT") == 0) {
		// we need to bind local soket
		return SET_PORT;
	}

	if (reply_status == 227 && strcmp(previous_verb, "PASV") == 0) {
		// we need to store the server ip and port
		return SET_PASV;
	}
	
	if (reply_status == 150 && strcmp(previous_verb, "RETR") == 0) {
		if (singleLine(reply)) {
			// we can now safely build connection and get the file.
			return BEGIN_RETR;
		} else {
			// we should stop getting the file
			return STOP_RETR; 
		}	
	}
	
	if (reply_status == 150 && strcmp(previous_verb, "STOR") == 0) {
		if (singleLine(reply)) {
			// we can now safely build connection and store the file.
			return BEGIN_STOR;
		} else {
			// we should stop sending the file
			return STOP_STOR; 
		}
	}
	
	if (strcmp(previous_verb, "RETR") == 0 || strcmp(previous_verb, "STOR") == 0) {
		if (reply_status == 226) {
			// file successfully sent
			return FILE_SUCC;
		} else {
			// something is wrong with the file itself
			return FILE_FAIL;
		}
	}
	
	if (reply_status == 221 && (strcmp(previous_verb, "QUIT") == 0 || strcmp(previous_verb, "ABOR") == 0)) {
		// we need to cutdown connection from the server and exit from the program
		return TO_QUIT;
	}
	
	// just ignore the reply
	return IGNORE;
	
}




