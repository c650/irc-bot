#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <stdarg.h>
#include <unistd.h>

#include "./helper-functions.h"
#include "./irc-datatypes.h"
#include "./arr.h"
#include "./bot.h"
/*
	helper-functions.c
*/

void write_to_socket(IRCSession* session, char* buf, char* fmt, ...) {
	
	memset(buf, 0, BUFFER_SIZE);

	va_list ap;
	va_start(ap,fmt);

	vsnprintf(buf, BUFFER_SIZE, fmt, ap);

	va_end(ap);

	write(session->sockfd, buf, strlen(buf));

	printf("[*] Sent:    %s", buf+1);
}

void format_query(char* query_str, char* result) {

	char* escaped;
	CURL *curl = curl_easy_init();

	char *token = strtok(query_str, ",;?!-_ \n\r");
	while (token != NULL) {

		escaped = curl_easy_escape(curl, token, strlen(token));

		strcpy(result + strlen(result), escaped);
		strcpy(result + strlen(result), "+");

		token = strtok(NULL, ",.:;?!-_ ");
		
		curl_free(escaped);
	}

	result[strlen(result) - 1] = '\0';

	curl_easy_cleanup(curl);
}

char* parse_for_host(IRCPacket* packet) {

	char* pos = packet->hostname;

	if (strstr(pos, "/") != NULL && strstr(pos, ".") == NULL) {
		return NULL;
	} else {
		return pos;
	}
}

void ip_lookup(char* host, char* out, IRCSession* session, IRCPacket* packet) {
	char sbuf[256];
	sprintf(sbuf, "./geoip.rb %s", host);

	printf("[*] Running %s\n", sbuf);
	FILE* fp = popen(sbuf, "r");

	while (fgets(sbuf, 256, fp) != NULL) {
		if (sbuf[0] == 0x00) continue;

		write_to_socket(session, out, "\rPRIVMSG %s :%s", packet->channel, sbuf);
	}

	pclose(fp);
}

void send_args(char** argv, const size_t *argc, IRCSession* session, char* buf) {
	for (int i = 0; i < *argc-1; i++) {
		write_to_socket(session, buf, "%s ", argv[i]);
	}
	write_to_socket(session, buf, "%s", argv[*argc - 1]);
}