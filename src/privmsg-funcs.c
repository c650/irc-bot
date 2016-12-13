/*
	privmsg-funcs.c
*/

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>

#include "./include/privmsg-funcs.h"
#include "./include/helper-functions.h"
#include "./include/irc-datatypes.h"
#include "./include/arr.h"

void slap(IRCSession* session, IRCPacket* packet, char* out, Command* command) {
	if (command->argc <= 0 || !strcmp(session->nick, command->argv[0]))
		return;

	int prob = rand() % 10;
	if (prob < 1) {

		write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION slapped %s so hard he got kicked from the channel!\001\r\n", packet->channel, command->argv[0]);

		sleep(2);

		if ( !arr_find(session->admins, command->argv[0], &session->num_admins) ) // if slap victim isn't admin.
			write_to_socket(session, out, "\rKICK %s %s\r\n", packet->channel, command->argv[0]);

	} else {
		write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION slapped the hell outta %s\001\r\n", packet->channel, command->argv[0]);
	}
}

void query(IRCSession* session, IRCPacket* packet, char* out, Command* command, const char* main_url) {

	char* concatted = concat_arr(command->argv, &command->argc);

	if (!concatted) return;

	char* result = calloc(strlen(concatted) * 3, sizeof(char));
	if (!result) {
		free(concatted);
		return;
	}

	format_query(concatted, result);

	write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, main_url, result);

	free(concatted);

	free(result);
}

void search(IRCSession* session, IRCPacket* packet, char* out, Command* command) {

	/*
		Must fix this function, adapt to argc/argv
	*/
	char* search_url = "https://0x00sec.org/search?q=";
	char* concatted = concat_arr(command->argv, &command->argc);

	CURL *curl = curl_easy_init();
	if (curl) {
		char* escaped = curl_easy_escape(curl, concatted, strlen(concatted));
		curl_easy_cleanup(curl);

		write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, search_url, escaped);

		curl_free(escaped);
	}

	free(concatted);
}

void echo_config(IRCSession* session, IRCPacket* packet, char* out, Command* command, char** echoing) {
	printf("[0] gets here!!\n");
	if (command->argv[0][0] == '1') {

		if (*echoing != NULL) {
			free(*echoing);
		}

		(*echoing) = strdup(packet->sender);
		printf("and here??\n");

		write_to_socket(session, out, "\rPRIVMSG %s :Now echoing: %s\r\n", packet->channel, *echoing);

	} else {

		write_to_socket(session, out, "\rPRIVMSG %s :No longer echoing: %s\r\n", packet->channel, *echoing);

		if (*echoing != NULL) {
			free(*echoing);
			*echoing = NULL;
		}
	}
	printf("[*] gets here!!\n");
}
