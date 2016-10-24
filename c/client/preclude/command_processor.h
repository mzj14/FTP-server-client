#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#define IP "-ip" // ip argument in command
#define PORT "-port"  // the port argument in command
#define ROOT "-root"  // the root argument in command

#define DEFAULT_IP "127.0.0.1" 
#define IP_REGEX "^((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])\\.){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$"
#define DEFAULT_PORT 21  
#define DEFAULT_ROOT "local"

#define FULL_ACCESS S_IRWXU|S_IROTH|S_IWOTH|S_IXOTH

extern void getIPAndPortAndRoot(int argc, char* argv[], char* ip, int* port, char* directory);

#endif