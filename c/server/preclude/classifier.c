#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdio.h>

#include "const.h"
#include "classifier.h"

const char* KEY_VERB[COMMAND_NUM] = { USER_VERB, PASS_VERB, SYST_VERB, TYPE_VERB, QUIT_VERB, ABOR_VERB, PORT_VERB, RETR_VERB, PASV_VERB, STOR_VERB };

int ifAllUpperCase(char* str) {
	
	int i = 0;
    while (str[i] != '\0') {
		if (isupper(str[i]) == 0) {
			return 0;
		}
		i++;
	}
	return 1;
	
}

int getCommandType(char* verb) {
	
	int i = 0;
	for (i = 0; i < COMMAND_NUM; i++) {
		if (strcmp(verb, KEY_VERB[i]) == 0) {
			return i + 1;
		}
	}
	return 0;
	
}

// check if parameter meets the pattern in regular expression
// return 1 if meet, otherwise return 0-9
int checkParamWithRegex(char* parameter, char* pattern) {
    int status;
    regex_t reg;
    
    regcomp(&reg, pattern, REG_EXTENDED);
    status = regexec(&reg, parameter, 0, NULL, 0);
    regfree(&reg);
    
    if (status == REG_NOMATCH) {
        return 0;
    } else if (status == 0) {
        return 1;
    }
}

// check if the parameter of the command contains syntax error. If there is syntax error, we got error_msg
// if syntax correct, we return 1
int properParam(char* parameter, int type, char* error_msg) {
	
    // the USER command
    if (type == USER_COMMAND) {
        if (checkParamWithRegex(parameter, "^$") == 1) {
            stpcpy(error_msg, "500 User name is needed\r\n");
            return 0;
        }
        if (checkParamWithRegex(parameter, "^\\w+$") == 0) {
            stpcpy(error_msg, "503 User name should only contains: _ a-z A-Z 0-9\r\n");
            return 0;
        }
        if (checkParamWithRegex(parameter, "^anonymous$") == 0) {
            stpcpy(error_msg, "504 FTP does not support users other than anonymous\r\n");
            return 0;
        };
    }
    
    // the PASS command
    if (type == PASS_COMMAND) {
        if (checkParamWithRegex(parameter, "^$") == 1) {
            stpcpy(error_msg, "500 Password is needed\r\n");
            return 0;
        }
    }
    
    // the SYST, QUIT, ABOR, PASV command
    if (type == SYST_COMMAND || type == QUIT_COMMAND || type == ABOR_COMMAND || type == PASV_COMMAND) {
        if (checkParamWithRegex(parameter, "^$") == 0) {
            stpcpy(error_msg, "500 The command should not have parameter\r\n");
            return 0;
        }
    }
    
	// the TYPE command
    if (type == TYPE_COMMAND) {
        if (checkParamWithRegex(parameter, "^$") == 1) {
            stpcpy(error_msg, "500 Type parameter is needed\r\n");
            return 0;
        }
        if (checkParamWithRegex(parameter, "^I$") == 0) {
            stpcpy(error_msg, "504 FTP does not support type other I\r\n");
            return 0;
        }
    }
	
	// the PORT command
    if (type == PORT_COMMAND) {
		if (checkParamWithRegex(parameter, "^$") == 1) {
            stpcpy(error_msg, "500 Port parameter is needed\r\n");
            return 0;
        }
		if (checkParamWithRegex(parameter, PORT_REGEX) == 0) {
			stpcpy(error_msg, "500 Port format is wrong\r\n");
            return 0;
		}
	}

    // the RETR and STOR command.
	if (type == RETR_COMMAND || type == STOR_COMMAND) {
		if (checkParamWithRegex(parameter, "^$") == 1) {
            stpcpy(error_msg, "500 Command parameter is needed\r\n");
            return 0;
        }
	}
	
    return 1;
	
}

int getRequestCategory(char* verb, char* parameter, char* error_msg) {
	
    // only space in command
    if (verb[0] == '\0') {
        strcpy(error_msg, "500 Command contains only space character\r\n");
        return 0;
    }
    
    // check if there are space within the command
	if (isspace(parameter[0])) {
		strcpy(error_msg, "500 More than one space between the verb and parameter\r\n");
        return 0;
	}
    
    // lowercase character in verb
    if (ifAllUpperCase(verb) == 0) {
        // parsing error
        strcpy(error_msg, "500 Verb has nonuppercase character\r\n");
        return 0;
    }
    
    int commandType = getCommandType(verb);
	
    // illegal command
    if (commandType == 0) {
        // not support the verb
        strcpy(error_msg, "503 FTP does not support the verb\r\n");
    }
    
	// improper parameter
	if (properParam(parameter, commandType, error_msg) == 0) {
        return 0;
	}
    
	return commandType;
	
}