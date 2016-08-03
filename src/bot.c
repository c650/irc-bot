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
#include "./helper-functions.c"
#include "./connect.c"
#include "./parser.c"
#include "./privmsg-funcs.c"

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
	int restart = 1, iplookupset = 0;

	while(restart) {
		
		restart = 0;

		time_t t;
		srand((unsigned) time(&t));

		connect_to_irc(session);
		log_on(session);

		sleep(15);
		
		join_channel(session);

		char *buf, *out;
		buf = malloc(sizeof(char) * BUFFER_SIZE+1);
		out = malloc(sizeof(char) * BUFFER_SIZE+1);

		IRCPacket *packet = malloc(sizeof(IRCPacket));
		
		int n;
		while( (n = read(session->sockfd, buf, BUFFER_SIZE)) && !restart) {

			buf[n] = 0;
			printf("%s\n", buf);

			if (!strncmp(buf, "PING", 4)) {
				
				buf[1] = 'O';
				write_to_socket(session, out, buf);
				printf("[*] Pong sent!\n");

			} else {

				if (parse_irc_packet(buf, packet) <= 0) {
					continue;
				}

				if (packet->content != NULL && !strcmp(packet->type, "PRIVMSG")) {
					
					if ( strstr(packet->content, "@slap ") ) {

						slap(session, packet, out, admin);
					
					} else if ( strstr(packet->content, "@google ") ) {
						
						google(session, packet, out);

					} else if ( strstr(packet->content, "@search ") ) {
						
						search(session, packet, out);

					} else if ( strstr(packet->content, "@urban ") ) {

						urban(session, packet, out);

					} else if ( strstr(packet->content, "@topic ") ) {

						write_to_socket(session, out, "\rPRIVMSG %s :https://0x00sec.org/t/%d\r\n", packet->channel, atoi(strstr(packet->content, "@topic ") + 7));

					} else if ( strstr(packet->content, "@iplookup ") && iplookupset) {
						
						char* pos = strstr(packet->content, "@iplookup ") + 10;
						pos = strtok(pos, " /;,&|~!?\?+=#@%%\\$>^*()[]{}_<\"\'\r\b`");
						printf("[*] host = %s.\n", pos);

						if (pos != NULL) ip_lookup(pos, out, session);

					} else if ( strstr(packet->content, "@help") ) {

						write_to_socket(session, out, "\rPRIVMSG %s :%s: slap, google, search, urban, topic, iplookup, help, echo [0,1], repeat\r\n", packet->channel, packet->sender);
					
					} else if (strstr(packet->content, "@quit") && !strcmp(packet->sender, admin)) {
					
						char* message = strstr(packet->content, "@quit") + 5;
						if (*message != '\0') message++;

						write_to_socket(session, out, "\rQUIT :Quit: %s\r\n", message);
						break;

					} else if (strstr(packet->content, "@restart") && !strcmp(packet->sender, admin)) {

						write_to_socket(session, out, "\rPRIVMSG %s :Restarting...\r\n", packet->channel);
						write_to_socket(session, out, "\rQUIT :Quit: Restarting\r\n");

						restart = 1;
						break;				

					} else if ( strstr(packet->content, "@echo ") ) {

						echo_config(session, packet, out, &echoing);
						printf("[*] echoing = %s\n", echoing);

					} else if ( strstr(packet->content, "@kick ") ) {

						if (!strcmp(packet->sender, admin))
							kick_user(session, packet, out);
						else
							write_to_socket(session, out, "\rPRIVMSG %s :Silly %s! Only %s can do that!\r\n", packet->channel, packet->sender, admin);

					} else if ( strstr(packet->content, "@repeat ") ) {

						/* would NOT want it to repeat itself */
						if (strcmp(packet->sender, session->nick)) {
							write_to_socket(session, out, "\rPRIVMSG %s :\"\033[0;31m%s\033[0;39m\" -- %s\r\n", packet->channel, strstr(packet->content, "@repeat ") + 8, packet->sender);
						}

					} else if (strstr(packet->content, "@iplookupset ") ) {

						char *setting = strstr(packet->content, "@iplookupset ") + 13;
						if (*setting == '1' && !strcmp(packet->sender, admin) ) {
							iplookupset = 1;
						} else {
							iplookupset = 0;
						}
						
						write_to_socket(session, out, "\rPRIVMSG %s :iplookupset = %d\r\n", packet->channel, iplookupset);					
					}

					if (echoing != NULL && !strcmp(packet->sender, echoing)) {

						write_to_socket(session, out, "\rPRIVMSG %s :%s\r\n", packet->channel, packet->content);					

					}
				}

				if (!strcmp(packet->type, "JOIN")) {

					if (strcmp(packet->sender, session->nick) != 0) {

						write_to_socket(session, out, "\rPRIVMSG %s :Hi there, %s!\r\n", packet->channel, packet->sender);

						char* host;
						if ((host = parse_for_host(packet)) != NULL && iplookupset) {
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

		if (echoing != NULL)
			free(echoing);

		free(buf);
		free(out);

		free(packet);
		close(session->sockfd);

		if (restart) {
			printf("[*] Restarting...\n\r");
			for (int i = 0; i < 4; i++) {
				printf(" ### ");
				sleep(0.5);
			}
			printf("\n");
		}
	}
	
	free(session);

	return 0;
}