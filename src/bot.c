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
#include "./connect.c"
#include "./helper-functions.c"
#include "./parser.c"

int main(int argc, char** argv) {

	if (argc != 5) {
		printf("Usage: %s [server] [channel] [nick] [password]\n", argv[0]);
		exit(4);
	}

	/*
		Streamlining IRC Data into a session.
	*/
	IRCSession *session = malloc(sizeof(IRCSession));
	session->server = argv[1];
	session->channel = argv[2];
	session->nick = argv[3];
	session->pass = argv[4];
	session->port = IRC_PORT;

	const char* admin = "oaktree";
	char* echoing = NULL;

	connect_to_irc(session);
	log_on(session);

	sleep(15);
	
	join_channel(session);

	char *buf, *out;
	buf = malloc(sizeof(char) * BUFFER_SIZE+1);
	out = malloc(sizeof(char) * BUFFER_SIZE+1);

	IRCPacket *packet = malloc(sizeof(IRCPacket));
	
	int n;
	while( (n = read(session->sockfd, buf, BUFFER_SIZE)) ) {

		buf[n] = 0;
		printf("%s\n", buf);

		if (!strncmp(buf, "PING", 4)) {
			
			buf[1] = 'O';
			write_to_socket(session, out, buf);
			printf("Pong sent!\n");

		} else {

			if (parse_irc_packet(buf, packet) <= 0) {
				continue;
			}

			if (packet->content != NULL && !strcmp(packet->type, "PRIVMSG")) {
				if (strstr(packet->content, "@slap ") != NULL) {

					char* pos = strstr(packet->content, "@slap ") + 6;

					write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION slapped the hell outta %s\001\r\n", packet->channel, pos);
				
				} else if (strstr(packet->content, "@google ") != NULL) {
					
					char* google_url = "http://google.com/#q=";
					char* pos = strstr(packet->content, "@google ") + 8;

					char* result = malloc(strlen(pos) * 3);
					memset(result, 0, strlen(pos) * 3);

					if (result == NULL)
						continue;

					format_query(pos, result);

					write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, google_url, result);

					free(result);

				} else if (strstr(packet->content, "@search ") != NULL) {
					
					char* search_url = "https://0x00sec.org/search?q=";
					char* pos = strstr(packet->content, "@search ") + 8;

					CURL *curl = curl_easy_init();
					if (curl) {
						char* escaped = curl_easy_escape(curl, pos, strlen(pos));
						curl_easy_cleanup(curl);

						write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, search_url, escaped);

						curl_free(escaped);
					} else {
						continue;
					}

				} else if (strstr(packet->content, "@iplookup ") != NULL) {
					
					char* pos = strstr(packet->content, "@iplookup ") + 10;
					pos = strtok(pos, " /");
					printf("[*] host = %s.\n", pos);

					ip_lookup(pos, out, session);

				} else if (strstr(packet->content, "@help") != NULL) {

					write_to_socket(session, out, "\rPRIVMSG %s :%s: slap, google, search, iplookup, help, echo [0,1]\r\n", packet->channel, packet->sender);
				
				} else if (strstr(packet->content, "@quit") && !strcmp(packet->sender, admin)) {
				
					char* message = strstr(packet->content, "@quit") + 5;
					if (*message != '\0') message++;

					write_to_socket(session, out, "\rQUIT :Quit: %s\r\n", message);
					break;

				} else if (strstr(packet->content, "@echo ") != NULL) {

					char* command = strstr(packet->content, "@echo ") + 6;
					if (*command == '1') {

						if (echoing != NULL) {
							free(echoing);
						}

						echoing = malloc( strlen(packet->sender) + 1 );
						strcpy(echoing, packet->sender);

						write_to_socket(session, out, "\rPRIVMSG %s :Now echoing: %s\r\n", packet->channel, echoing);					

						continue;

					} else {

						write_to_socket(session, out, "\rPRIVMSG %s :No longer echoing: %s\r\n", packet->channel, echoing);

						free(echoing);
						echoing = NULL;
					}

				}

				if (echoing != NULL && !strcmp(packet->sender, echoing)) {

					write_to_socket(session, out, "\rPRIVMSG %s :%s\r\n", packet->channel, packet->content);					

				}
			}

			if (!strcmp(packet->type, "JOIN")) {

				if (strcmp(packet->sender, session->nick) != 0) {

					write_to_socket(session, out, "\rPRIVMSG %s :Hi there, %s!\r\n", packet->channel, packet->sender);

					char* host;
					if ((host = parse_for_host(packet)) != NULL) {
						printf("[*] host = %s\n", host);
						ip_lookup(host, out, session);
					}

				} else {
					write_to_socket(session, out, "\rPRIVMSG %s :Hi everybody!\r\n", packet->channel);
				}
			}
		}

		memset(buf, 0, BUFFER_SIZE);
	}

	printf("press enter to exit");
	scanf("%c",buf);

	if (echoing != NULL)
		free(echoing);

	free(buf);
	free(out);

	free(packet);
	close(session->sockfd);
	free(session);

	return 0;
}