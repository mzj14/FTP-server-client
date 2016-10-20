#ifndef CONST_H
#define CONST_H

#define COMMAND_NUM 7
#define KEY_WORD_SIZE 4

#define WELCOME_MESSAGE "220 Anonymous FTP server ready.\r\n"
#define WELCOME_MESSAGE_SIZE 33

#define OFFLINE -1
#define UN_LOG 0
#define USER_STATUS 1
#define LOG_IN 2

#define INVALID_COMMAND 0
#define USER_COMMAND 1
#define PASS_COMMAND 2
#define SYST_COMMAND 3
#define TYPE_COMMAND 4
#define QUIT_COMMAND 5
#define ABOR_COMMAND 6
#define PORT_COMMAND 7
#define RETR_COMMAND 8

#define USER_VERB "USER"
#define PASS_VERB "PASS"
#define SYST_VERB "SYST"
#define TYPE_VERB "TYPE"
#define QUIT_VERB "QUIT"
#define ABOR_VERB "ABOR"
#define PORT_VERB "PORT"
#define RETR_VERB "RETR"

#define UN_LOG_USER_MSG "331 Guest login ok, send your complete e-mail address as a password.\r\n"
#define UN_LOG_PASS_MSG "530 Please provide a user name before you set a password.\r\n"
#define UN_LOG_COMMON_MSG "530 You are not a user to use FTP command, please provide a user name first.\r\n"

#define USER_USER_MSG "331 User name change to anonymous.\r\n"
#define USER_PASS_MSG_1 "230-Welcome to Ma Zijun's FTP service.\r\n230-We make no guarantees, explicit or im\
plicit, about the contents of this site.\r\n230-Use at your own risk.\r\n230 Guest login ok, access restrictions apply.\r\n"
#define USER_PASS_MSG_2 "530 Password validation failed.\r\n"
#define USER_COMMON_MSG "530 You haven't log in, please set your password to log in.\r\n"

#define LOG_IN_USER_MSG USER_USER_MSG
#define LOG_IN_PASS_MSG "230 Password change to %s.\r\n"
#define LOG_IN_SYST_MSG "215 UNIX Type: L8\r\n"
#define LOG_IN_TYPE_MSG "200 Type set to I.\r\n"
#define LOG_IN_QUIT_MSG "221-Thank you for using the FTP service.\r\n221 Goodbye.\r\n"
#define LOG_IN_PORT_MSG "200 FTP receive new data IP and port of client.\r\n"
#endif