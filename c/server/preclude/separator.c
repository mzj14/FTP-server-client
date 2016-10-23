#include <string.h>
#include <ctype.h>

// left stands for the index of the most left unspace character
// right stands for the index of the most right unspace character + 1
// remove space character in request
void trimRequest(char* req, int* left, int* right) {
	
    *right = strlen(req) - 1;
	*left = 0;
    
    // trim the space from right
    while (*right >= 0 && isspace(req[*right])) {
        req[*right] = '\0';
        (*right)--;
    }
	
	(*right)++;
    
    // trim the space from left
    while (isspace(req[*left])) {
        req[*left] = '\0';
        (*left)++;
    }
    
    return;
	
}

// find first j that meets req[j] == ch in [left, right - 1]
int findFirstChar(char* req, int left, int right, char ch) {
	
	int i;
	for (i = left; i < right; i++) {
		if (req[i] == ch) {
			return i;
		}
	}
	return i;
	
}

// copy req[left, right -1] to dest
void copyString(char* req, int left, int right, char* dest) {
	
    int i, j = 0;
    for (i = left; i < right; i++) {
		dest[j] = req[i];
		j++;
	}
    return;	
	
}

// separate request to verb and parameter
void separateRequest(char* req, char* verb, char* parameter) {
	
    int left, right, middle;
    trimRequest(req, &left, &right);
    
    if (right == 0) {
		return;
	}
    
    middle = findFirstChar(req, left, right, ' ');
	
	copyString(req, left, middle, verb);
	copyString(req, middle + 1, right, parameter);
    
    return;
	
}
