#include <sys/stat.h> 

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "const.h"
#include "command_processor.h"

void getPortAndRoot(int argc, char* argv[], int* port, char* directory) {
	
	int give_root_flag = 0, give_port_flag = 0;
	
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], PORT) == 0) {
			if (i + 1 < argc) {
				int rv = atoi(argv[i + 1]);
				// if the port number is reasonable, set port according to command argument
				if (0 < rv && rv < TOTAL_PORT_NUM) {
					*port = rv;
					give_port_flag = 1;
				}
			}
		}
		
		if (strcmp(argv[i], ROOT) == 0) {
			if (i + 1 < argc) {
				// if the root directory already exist or can be created, set root directory according to command argument
				if (access(argv[i + 1], 0) == 0 || mkdir(argv[i + 1], FULL_ACCESS) == 0) {
					strcpy(directory, argv[i + 1]);
					give_root_flag = 1;
				}	
			}
		}
	}
	
	// no proper port is given by command
	if (give_port_flag == 0) {
		*port = DEFAULT_PORT;
	}
	
	// no root directory was given by command 
	if (give_root_flag == 0) {
		if (access(DEFAULT_ROOT, 0) == 0 || mkdir(DEFAULT_ROOT, FULL_ACCESS) == 0) {
			// printf("Create the root directory %s.\n", DEFAULT_ROOT);
			strcpy(directory, DEFAULT_ROOT);
		}
	}
	
}