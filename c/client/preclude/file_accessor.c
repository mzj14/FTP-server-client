#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "const.h"
#include "file_accessor.h"

int accessFile(char* root_directory, char* filename, int access_mode) {
	char file_path[MAXDATASIZE];
	strcpy(file_path, root_directory);
	
	int root_len = strlen(root_directory);
	file_path[root_len] = '/';
	file_path[root_len + 1] = '\0';
	
	strcat(file_path, filename);
	
	return access(file_path, access_mode);
}