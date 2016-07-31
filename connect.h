#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define IRC_PORT 6667
#define BUFFER_SIZE 2048

int connect_to_irc(int* sockfd, char* irc_channel, int port);
int get_on_channel(int* sockfd, char* channel, char* nick, char* buf, char* password);
int write_to_socket(int* sockfd, char* buf, char* fmt, ...);