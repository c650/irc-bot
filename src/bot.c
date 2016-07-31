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
		exit(1);
	}

	char *server, *channel, *nick, *password;
	server = argv[1];
	channel = argv[2];
	nick = argv[3];
	password = argv[4];

	int sockfd;
	connect_to_irc(&sockfd, 6667, server);

	char buf[BUFFER_SIZE+1]; char out[BUFFER_SIZE+1];
	int n;

	get_on_channel(&sockfd, channel, nick, out, password);
	
	while( (n = read(sockfd, buf, BUFFER_SIZE)) ) {
		if (n <= 0) continue;

		buf[n] = 0;

		if (strncmp(buf, "PING", 4)) {
			printf("We got PING'd\n");
			
			send(sockfd, out, strlen(out), 0);
		}

		if (strstr(buf, "@slap") != NULL) {
			out[0] = 0;

			char* pos = strstr(buf, "@slap ") + 6;
			sprintf(out, "\rPRIVMSG %s :\001ACTION slapped the hell outta %s\001\r\n", channel, pos);

			responsible_send(sockfd, out, strlen(out), 0);
		}

		if (strstr(buf, "@google ") != NULL) {
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
		}

		if (strstr(buf, "@search ") != NULL) {
			char* search_url = "https://0x00sec.org/search?q=";
			char* pos = strstr(buf, "@search ") + 8;
			pos[strlen(pos)-2] = 0;

			CURL *curl = curl_easy_init();
			if (curl) {
				char* escaped = curl_easy_escape(curl, pos, strlen(pos));
				curl_easy_cleanup(curl);

				sprintf(out, "\rPRIVMSG %s :%s%s\r\n", channel, search_url, escaped);
				responsible_send(sockfd, out, strlen(out), 0);

				printf("Got here\n");

				curl_free(escaped);
			}
		}

		printf("%s\n", buf);
		memset(buf, 0, BUFFER_SIZE);
	}

	printf("press enter to exit");
	scanf("%c",buf);

	close(sockfd);

	return 0;
}