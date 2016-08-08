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

const static char* swears[] = {"fuck", "cunt", "bitch"};
const static int swears_len = 3;

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
	session->nick = strdup(argv[3]); // so ik it's been alloced
	session->pass = argv[4];
	session->port = IRC_PORT;

	const char* admin = "oaktree";
	char* echoing = NULL;
	int restart = 1, iplookupset = 0, sleeping = 0;

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
		
		int n, cussed_out = 0, admin_is_sender = 0;
		while( (n = read(session->sockfd, buf, BUFFER_SIZE)) && !restart) {

			buf[n] = 0;
			printf("\n%s\n", buf);

			if (!strncmp(buf, "PING", 4)) {
				
				buf[1] = 'O';
				write_to_socket(session, out, buf);
				printf("[*] Pong sent!\n");

			} else {

				if (parse_irc_packet(buf, packet) <= 0) {
					continue;
				}

				if (packet->content != NULL && !strcmp(packet->type, "PRIVMSG")) {

					/*
						Must run echo check first.
					*/
					if (echoing != NULL && !strcmp(packet->sender, echoing)) {
						write_to_socket(session, out, "\rPRIVMSG %s :%s\r\n", packet->channel, packet->content);					
					}

					/*
						No need for the name prompt (e.g., "pinetree: ") in a PM. Otherwise...
						Only speak when spoken to, and don't execute commands said by yourself.

					*/
					if (strcmp(packet->channel, session->nick) && (strlen(packet->content) < strlen(session->nick)
						|| strncmp(packet->content, session->nick, strlen(session->nick))
						|| !strcmp(session->nick, packet->sender))) {
						continue;
					}

					/*
						If we are in a PM prepare to send things back, unlike when
						in a channel.
					*/
					if (!strcmp(packet->channel, session->nick)) {
						packet->channel = packet->sender;
					}

					/*
						Check if bot was cussed out.
					*/
					for (int i = 0; i < swears_len; i++) {
						if (strstr(packet->content, swears[i])) {
							cussed_out = 1;
						}
					}
					/*
						Act IF bot was cussed out.
					*/
					if (cussed_out) {
						write_to_socket(session, out, "\rPRIVMSG %s :%s you lil' %s!\r\n", packet->channel, packet->sender, swears[rand() % swears_len]);
						cussed_out = 0;
					}

					admin_is_sender = !strcmp(admin, packet->sender);

					if ( strstr(packet->content, "@wakeup")) {
						sleeping = 0;
						
						write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION yawned!\001\r\n", packet->channel);
						
						sleep(0.7);

						write_to_socket(session, out, "\rPRIVMSG %s :What'd I miss?\r\n", packet->channel);					
					} else if ( strstr(packet->content, "@sleep") && admin_is_sender) {

						sleeping = 1;

						write_to_socket(session, out, "\rPRIVMSG %s :I'm tired. KTHXBAI\r\n", packet->channel);

					}

					if (sleeping) continue;
					
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

					} else if ( strstr(packet->content, "@iplookup ")) {
						
						if (iplookupset) {
							char* pos = strstr(packet->content, "@iplookup ") + 10;
							pos = strtok(pos, " /;,&|~!?\?+=#@%%\\$>^*()[]{}_<\"\'\r\b`");
							printf("[*] host = %s.\n", pos);

							if (pos != NULL) ip_lookup(pos, out, session);
						} else {
							write_to_socket(session, out, "\rPRIVMSG %s :%s: iplookup is off.\r\n", packet->channel, packet->sender);
						}

					} else if ( strstr(packet->content, "@help") ) {

						write_to_socket(session, out, "\rPRIVMSG %s :slap, google, search, urban, topic, iplookup, help, echo [0,1], repeat, wakeup\r\n", packet->sender);
						write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION Just PM'd %s the HELP menu.\001\r\n", packet->channel, packet->sender);
					
					} else if (strstr(packet->content, "@quit") && admin_is_sender) {
					
						char* message = strstr(packet->content, "@quit") + 5;
						if (*message != '\0') message++;

						write_to_socket(session, out, "\rQUIT :Quit: %s\r\n", message);
						sleep(3);
						break;

					} else if (strstr(packet->content, "@restart") && admin_is_sender) {

						write_to_socket(session, out, "\rPRIVMSG %s :Restarting...\r\n", packet->channel);
						write_to_socket(session, out, "\rQUIT :Quit: Restarting\r\n");

						restart = 1;
						break;				

					} else if ( strstr(packet->content, "@echo ") ) {

						echo_config(session, packet, out, &echoing);
						printf("[*] echoing = %s\n", echoing ? echoing : "nobody");

					} else if ( strstr(packet->content, "@kick ") ) {

						if (admin_is_sender)
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
						if (*setting == '1' && admin_is_sender ) {
							iplookupset = 1;
						} else {
							iplookupset = 0;
						}
						
						write_to_socket(session, out, "\rPRIVMSG %s :iplookupset = %d\r\n", packet->channel, iplookupset);					
					} else if (strstr(packet->content, "@join ") && admin_is_sender) {

						char* chan = strstr(packet->content, "@join ") + 6;
						
						char* cutoff = strchr(chan, ' ');
												
						int was_null = 0;
						if (cutoff != NULL) {
							*cutoff = '\0';
						} else {
							was_null = 1;
						}

						char* orig_chan = session->channel;
						session->channel = chan;

						join_channel(session);
						printf("[*] Joining %s...\n", chan);

						session->channel = orig_chan;

						if (!was_null)
							*cutoff = ' ';

					} else if (strstr(packet->content, "@part ") && admin_is_sender) {

						char* chan = strstr(packet->content, "@part ") + 6;
						
						char* cutoff = strchr(chan, ' ');
												
						int was_null = 0;
						if (cutoff != NULL) {
							*cutoff = '\0';
						} else {
							was_null = 1;
						}

						write_to_socket(session, out, "\rPART %s\r\n", chan);
						printf("[*] Parting from %s...\n", chan);

						if (!was_null)
							*cutoff = ' ';

					} else if (strstr(packet->content, "@nick ") && admin_is_sender) {

						char* new_nick = strstr(packet->content, "@nick ") + 6;

						char* cutoff = strchr(new_nick, ' ');

						int was_null = 0;
						if (cutoff != NULL) {
							*cutoff = '\0';
						} else {
							was_null = 1;
						}

						if (session->nick != NULL) free(session->nick);
						session->nick = strdup(new_nick);

						write_to_socket(session, out, "\rNICK %s\r\n", session->nick);
						printf("[*] Changing nick to %s...\n", session->nick);

						if (!was_null)
							*cutoff = ' ';
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

				if (!strcmp(packet->type, "KICK") && strstr(packet->content, session->nick)) {

					printf("[*] Got kicked from %s\n", packet->channel);
					sleep(0.4);

					char* tmp = session->channel;

					session->channel = packet->channel; // in case you're in several channels

					join_channel(session);

					session->channel = tmp;

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
	
	free(session->nick);
	free(session);

	return 0;
}