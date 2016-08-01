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

#include "./connect.h"
#include "./connect.c"
#include "./helper-functions.c"

int main(int argc, char** argv) {

	if (argc != 5) {
		printf("Usage: %s [server] [channel] [nick] [password]\n", argv[0]);
		exit(4);
	}

	char *server, *channel, *nick, *password;
	server = argv[1];
	channel = argv[2];
	nick = argv[3];
	password = argv[4];

	int sockfd;
	connect_to_irc(&sockfd, IRC_PORT, server);

	char buf[BUFFER_SIZE+1]; char out[BUFFER_SIZE+1];
	int n;

	get_on_channel(&sockfd, channel, nick, out, password);
	
	while( (n = read(sockfd, buf, BUFFER_SIZE)) ) {

		buf[n] = 0;
		printf("%s\n", buf);

		if (strncmp(buf, "PING", 4)) {
			//printf("We got PING'd\n");
			
			strcpy(out, "PONG :pingis\r\n");

			responsible_send(sockfd, out, strlen(out), 0);
		}

		if (strstr(buf, "@slap") != NULL) {
			out[0] = 0;

			char* pos = strstr(buf, "@slap ") + 6;
			sprintf(out, "\rPRIVMSG %s :\001ACTION slapped the hell outta %s\001\r\n", channel, pos);

			responsible_send(sockfd, out, strlen(out), 0);
		} else if (strstr(buf, "@google ") != NULL) {
			char* google_url = "http://google.com/#q=";
			char* pos = strstr(buf, "@google ") + 8;

			char* result = malloc(strlen(pos));
			memset(result, 0, strlen(pos));

			if (result == NULL)
				continue;

			format_query(pos, result);

			sprintf(out, "\rPRIVMSG %s :%s%s\r\n", channel, google_url, result);

			responsible_send(sockfd, out, strlen(out), 0);

			free(result);
		} else if (strstr(buf, "@search ") != NULL) {
			char* search_url = "https://0x00sec.org/search?q=";
			char* pos = strstr(buf, "@search ") + 8;
			pos[strlen(pos)-2] = 0;

			CURL *curl = curl_easy_init();
			if (curl) {
				char* escaped = curl_easy_escape(curl, pos, strlen(pos));
				curl_easy_cleanup(curl);

				sprintf(out, "\rPRIVMSG %s :%s%s\r\n", channel, search_url, escaped);
				responsible_send(sockfd, out, strlen(out), 0);

				curl_free(escaped);
			}
		} else if (strstr(buf, "@iplookup ") != NULL) {
			char* pos = strstr(buf, "@iplookup ") + 10;
			pos = strtok(pos, " /\r\n");
			printf("pos = %s.\n", pos);

			ip_lookup(pos, sockfd, out, channel);

		} else if (strstr(strtok(buf+1, ":"), "JOIN ") != NULL) {
			char* pos = strtok(buf, "!") + 1;

			if (strcmp(pos, nick) != 0) {
				sprintf(out, "\rPRIVMSG %s :Hi there, %s!\r\n", channel, pos);

				char* pos1;
				if ((pos1 = parse_for_host(buf, pos)) != NULL) {
					printf("pos = %s\n", pos1);
					ip_lookup(pos1, sockfd, out, channel);
					
					free(pos1);
				}
			}
			else
				sprintf(out, "\rPRIVMSG %s :Hi everybody!\r\n", channel);


			responsible_send(sockfd, out, strlen(out), 0);
		}

		memset(buf, 0, BUFFER_SIZE);
	}

	printf("press enter to exit");
	scanf("%c",buf);

	close(sockfd);

	return 0;
}