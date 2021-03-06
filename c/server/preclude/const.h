#ifndef CONST_H
#define CONST_H

#define MAXDATASIZE 200

#define KEY_WORD_SIZE 4

#define WELCOME_MESSAGE "220 Anonymous FTP server ready.\r\n"

#define OFFLINE -1
#define UN_LOG 0
#define USER_STATUS 1
#define LOG_IN 2

#define TOTAL_PORT_NUM 65536
#define BACKLOG 20
#define INVALID_PORT -1

#define BLANK_PASSWORD "$"

#define COMMAND 0
#define DATA 1

#define INVALID_IP "$"
#define INVALID_PORT -1
#define INVALID_SOCKFD -1

#define INVALID_TRANS 0
#define PORT_TRANS 1
#define PASV_TRANS 2

#define INVALID_COMMAND 0
#define USER_COMMAND 1
#define PASS_COMMAND 2
#define SYST_COMMAND 3
#define TYPE_COMMAND 4
#define QUIT_COMMAND 5
#define ABOR_COMMAND 6
#define PORT_COMMAND 7
#define RETR_COMMAND 8
#define PASV_COMMAND 9
#define STOR_COMMAND 10

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
#define LOG_IN_PORT_MSG "200 PORT command successful.\r\n"

#define LOG_IN_MARK_MSG "150 Opening BINARY mode data connection for %s\r\n"
 
#define LOG_IN_RETR_MSG_1 "425 No TCP connection shown by PORT or PASV.\r\n"
#define LOG_IN_RETR_MSG_2 "550 File not exist.\r\n"
#define LOG_IN_RETR_MSG_3 "550 No right to read the file.\r\n"
#define LOG_IN_RETR_MSG_4 "425 Can not establish tcp connection.\r\n"
#define LOG_IN_RETR_MSG_5 "426 The established tcp connection is broken.\r\n"
#define LOG_IN_RETR_MSG_6 "226 Transfer complete (%ld bytes).\r\n"

#define LOG_IN_PASV_MSG "227 =%s\r\n"

#define LOG_IN_STOR_MSG_1 LOG_IN_RETR_MSG_1
#define LOG_IN_STOR_MSG_2 LOG_IN_RETR_MSG_4
#define LOG_IN_STOR_MSG_3 "550 File already exist.\r\n"
#define LOG_IN_STOR_MSG_4 "426 The established tcp connection is broken.\r\n"
#define LOG_IN_STOR_MSG_5 LOG_IN_RETR_MSG_6
 
#endif