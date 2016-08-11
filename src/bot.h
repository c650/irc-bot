/*
	bot.h
*/

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
#include <curl/curl.h>

#define BUFFER_SIZE 2048
#define IRC_PORT 6667

#include "./irc-datatypes.h"
#include "./helper-functions.h"
#include "./arr.h"
#include "./connect.h"
#include "./parsers.h"
#include "./privmsg-funcs.h"

#include "./irc-datatypes.c"
#include "./helper-functions.c"
#include "./arr.c"
#include "./connect.c"
#include "./parsers.c"
#include "./privmsg-funcs.c"