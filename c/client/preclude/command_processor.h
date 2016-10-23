#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#define PORT "-port"  // the port argument in command
#define ROOT "-root"  // the root argument in command

#define DEFAULT_PORT 21
#define DEFAULT_ROOT "/tmp"

#define FULL_ACCESS S_IRWXU|S_IROTH|S_IWOTH|S_IXOTH   // full right to the created folder

extern void getPortAndRoot(int argc, char* argv[], int* port, char* directory);

#endif