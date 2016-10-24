#include <sys/stat.h> 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>

#include "const.h"
#include "command_processor.h"

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

void getIPAndPortAndRoot(int argc, char* argv[], char* ip, int* port, char* directory) {
	
	int give_ip_flag = 0, give_root_flag = 0, give_port_flag = 0;
	
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], IP) == 0) {
			if (i + 1 < argc) {
				if (checkParamWithRegex(argv[i + 1], IP_REGEX) == 1) {
					strcpy(ip, argv[i + 1]);
					give_ip_flag = 1;
				}
			}
		}
		
		if (strcmp(argv[i], PORT) == 0) {
			if (i + 1 < argc) {
				int rv = atoi(argv[i + 1]);
				if (0 < rv && rv < TOTAL_PORT_NUM) {
					*port = rv;
					give_port_flag = 1;
				}
			}
		}
		
		if (strcmp(argv[i], ROOT) == 0) {
			if (i + 1 < argc) {
				if (access(argv[i + 1], 0) == 0 || mkdir(argv[i + 1], FULL_ACCESS) == 0) {
					strcpy(directory, argv[i + 1]);
					give_root_flag = 1;
				}	
			}
		}
	}
	
	if (give_ip_flag == 0) {
		strcpy(ip, DEFAULT_IP);
	}
	
	if (give_port_flag == 0) {
		*port = DEFAULT_PORT;
	}
	
	if (give_root_flag == 0) {
		if (access(DEFAULT_ROOT, 0) == 0 || mkdir(DEFAULT_ROOT, FULL_ACCESS) == 0) {
		    strcpy(directory, DEFAULT_ROOT);
		}
	}
	
}