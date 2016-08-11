/*
	bot.c
	the MAIN file.
*/

#include "./bot.h"

int main(int argc, char** argv) {

	if (argc != 5) {
		printf("[!] Usage: %s [server] [nick] [password] [admin]\n", argv[0]);
		exit(4);
	}

	/*
		Streamlining IRC Data into a session.
	*/
	IRCSession *session = malloc(sizeof(IRCSession));
	init_session(session, argv[1], argv[2], argv[3], IRC_PORT, argv[4]);

	char* echoing = NULL;
	int restart = 1, iplookupset = 0, sleeping = 0;

	while(restart) {
		
		restart = 0;

		time_t t;
		srand((unsigned) time(&t));

		connect_to_irc(session);
		log_on(session);

		char *buf, *out;
		buf = malloc(sizeof(char) * BUFFER_SIZE+1);
		out = malloc(sizeof(char) * BUFFER_SIZE+1);

		/* now we let the first admin know we are ready */
		if (session->num_admins < 1) {
			printf("No admin? Exiting...\n");
			exit(5);
		} else {
			write_to_socket(session, out, "\rPRIVMSG %s :Ready for commands.\r\n", session->admins[0]);
		}

		IRCPacket *packet = malloc(sizeof(IRCPacket));
		Command *command = malloc(sizeof(Command));
		
		int n, admin_is_sender = 0, ignore_sender = 0;

		while( (n = read(session->sockfd, buf, BUFFER_SIZE)) && !restart) {

			buf[n] = 0x00;

			printf("\n%s\n", buf);

			if (!strncmp(buf, "PING", 4)) {
				
				buf[1] = 'O';
				write_to_socket(session, out, buf);
				printf("[*] Pong sent!\n");
				continue;

			}

			if (parse_irc_packet(buf, packet) <= 0) {
				continue;
			}

			admin_is_sender = (arr_find(session->admins, packet->sender, &session->num_admins) != NULL);
			ignore_sender = (arr_find(session->ignoring, packet->sender, &session->num_ignoring) != NULL);

			if (packet->content != NULL && !strcmp(packet->type, "PRIVMSG") && !ignore_sender) {

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
					Going to parse for any command.
					First found, actually.
				*/

				if (parse_for_command(packet, command) == 0) {
					continue;
				}

				if ( !strcmp(command->cmd, "@wakeup")) {
					sleeping = 0;
					
					write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION yawned!\001\r\n", packet->channel);
					
					sleep(0.7);

					write_to_socket(session, out, "\rPRIVMSG %s :What'd I miss?\r\n", packet->channel);					
				} else if ( !strcmp(command->cmd, "@sleep") && admin_is_sender) {

					sleeping = 1;

					write_to_socket(session, out, "\rPRIVMSG %s :I'm tired. KTHXBAI\r\n", packet->channel);

				}

				if (sleeping) continue;
				
				if ( !strcmp(command->cmd, "slap") && command->arg) {

					slap(session, packet, out, command);
				
				} else if ( !strcmp(command->cmd, "google") && command->arg) {
					
					google(session, packet, out, command);

				} else if ( !strcmp(command->cmd, "search") && command->arg) {
					
					search(session, packet, out, command);

				} else if ( !strcmp(command->cmd, "urban") && command->arg) {

					urban(session, packet, out, command);

				} else if ( !strcmp(command->cmd, "topic") && command->arg) {

					write_to_socket(session, out, "\rPRIVMSG %s :https://0x00sec.org/t/%d\r\n", packet->channel, atoi(command->arg ? command->arg : "0"));

				} else if ( !strcmp(command->cmd, "iplookup") && command->arg) {
					
					if (iplookupset) {
						char* pos = command->arg;
						pos = strtok(pos, " /;,&|~!?\?+=#@%%\\$>^*()[]{}_<\"\'\r\b`");
						printf("[*] host = %s.\n", pos);

						if (pos != NULL) ip_lookup(pos, out, session, packet);
					} else {
						write_to_socket(session, out, "\rPRIVMSG %s :%s: iplookup is off.\r\n", packet->channel, packet->sender);
					}

				} else if ( !strcmp(command->cmd, "help") ) {

					write_to_socket(session, out, "\rPRIVMSG %s :slap, google, search, urban, topic, iplookup, help, echo [0,1], repeat, wakeup\r\n", packet->sender);
					write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION Just PM'd %s the HELP menu.\001\r\n", packet->channel, packet->sender);
				
				} else if ( !strcmp(command->cmd, "echo") && command->arg) {

					echo_config(session, packet, out, command, &echoing);
					printf("[*] echoing = %s\n", echoing ? echoing : "nobody");

				} else if ( !strcmp(command->cmd, "repeat") && command->arg) {

					/* would NOT want it to repeat itself */
					if (strcmp(packet->sender, session->nick)) {
						write_to_socket(session, out, "\rPRIVMSG %s :\"%s\" -- %s\r\n", packet->channel, command->arg, packet->sender);
					}

				}

				if (admin_is_sender) {

					if (!strcmp(command->cmd, "iplookupset") && command->arg) {

						char *setting = command->arg;
						if (*setting == '1') {
							iplookupset = 1;
						} else {
							iplookupset = 0;
						}
						
						write_to_socket(session, out, "\rPRIVMSG %s :iplookupset = %d\r\n", packet->channel, iplookupset);					
					
					} else if (!strcmp(command->cmd, "join") && command->arg) {
						
						if (arr_find(session->channels, command->arg, &session->num_channels) != NULL)
							continue;
						
						arr_push_back(&session->channels, command->arg, &session->num_channels);

						join_channel(session);
						printf("[*] Joining %s...\n", command->arg);

					} else if (!strcmp(command->cmd, "part") && command->arg) {

						if (arr_find(session->channels, command->arg, &session->num_channels) == NULL)
							continue;

						write_to_socket(session, out, "\rPART %s\r\n", command->arg);
						printf("[*] Parting from %s...\n", command->arg);

						arr_remove(&session->channels, command->arg, &session->num_channels);

					} else if (!strcmp(command->cmd, "nick") && command->arg) {

						if (session->nick != NULL) free(session->nick);
						session->nick = strdup(command->arg);

						write_to_socket(session, out, "\rNICK %s\r\n", session->nick);
						printf("[*] Changing nick to %s...\n", session->nick);

					} else if (!strcmp(command->cmd, "quit")) {
				
						write_to_socket(session, out, "\rQUIT :Quit: %s\r\n", command->arg ? command->arg : "Leaving...");

						sleep(3);
						break;

					} else if (!strcmp(command->cmd, "restart")) {

						write_to_socket(session, out, "\rQUIT :Quit: Restarting\r\n");

						restart = 1;
						break;			

					} else if ( !strcmp(command->cmd, "kick")  && command->arg) {

						write_to_socket(session, out, "\rKICK %s %s\r\n", packet->channel, command->arg);

					} else if ( !strcmp(command->cmd, "ignore") && command->arg) {

						if ( arr_find(session->admins, command->arg, &session->num_admins) ) {
							printf("[*] %s tried to ignore an admin (%s)\n", command->caller, command->arg);
							continue;
						}

						printf("[*] Ignore command triggered.\n");
						arr_push_back(&session->ignoring, command->arg, &session->num_ignoring);
						printf("[*] Now ignoring %s.\n", command->arg);

					} else if ( !strcmp(command->cmd, "unignore") && command->arg) {

						if ( arr_find(session->ignoring, command->arg, &session->num_ignoring) == NULL) {
							continue;
						}

						arr_remove(&session->ignoring, command->arg, &session->num_ignoring);
						printf("[*] No longer ignoring %s.\n", command->arg);

					} else if ( !strcmp(command->cmd, "addadmin") && command->arg) {

						arr_push_back(&session->admins, command->arg, &session->num_admins);
						printf("[*] %s is now an admin.\n", command->arg);

					}
				}
			}

			if (!strcmp(packet->type, "JOIN")) {

				if (strcmp(packet->sender, session->nick) != 0) {

					char* host;
					if ((host = parse_for_host(packet)) != NULL && iplookupset) {
						printf("[*] host = %s\n", host);
						ip_lookup(host, out, session, packet);
					}

				} else {
					write_to_socket(session, out, "\rPRIVMSG %s :Hi everybody!\r\n", packet->channel);
				}
			}

			if (!strcmp(packet->type, "KICK") && strstr(packet->content, session->nick)) {

				printf("[*] Got kicked from %s\n", packet->channel);
				sleep(0.4);

				char* loc = arr_find(session->channels, packet->channel, &session->num_channels);
				
				if (loc == NULL) {
					continue;
				}
				
				// swap if channel kicked from isn't last channel.
				if (loc != session->channels[ session->num_channels - 1 ] ) {
					char* tmp = loc;
					loc = session->channels[ session->num_channels - 1 ];
					session->channels[ session->num_channels - 1 ] = tmp;
				}

				join_channel(session);

			}

			/*
				TODO: Be able to update ignore list if user changes nick.
			*/
			if ( !strcmp(packet->type, "NICK") ) {
				char *find_admin, *find_ignore;

				find_admin = arr_find(session->admins, packet->sender, &session->num_admins);
				find_ignore = arr_find(session->ignoring, packet->sender, &session->num_ignoring);

				if (find_admin) {
					free(find_admin);
					find_admin = strdup(packet->channel);
				}

				if (find_ignore) {
					free(find_ignore);
					find_ignore = strdup(packet->channel);
				}

				printf("[*] %s changed his nick to %s. Updating admin/ignore lists if needed...\n", packet->sender, packet->channel);
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
	
	free_session(session);

	return 0;
}