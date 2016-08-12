/*
	privmsg-funcs.c
*/

void slap(IRCSession* session, IRCPacket* packet, char* out, Command* command) {

	int prob = rand() % 10;
	if (prob < 1) {
		
		write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION slapped %s so hard he got kicked from the channel!\001\r\n", packet->channel, command->arg_first);					

		sleep(2);

		if ( !arr_find(session->admins, command->arg_first, &session->num_admins) ) // if slap victim isn't admin.
			write_to_socket(session, out, "\rKICK %s %s\r\n", packet->channel, command->arg_first);

	} else {
		write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION slapped the hell outta %s\001\r\n", packet->channel, command->arg_first);					
	}
}

void google(IRCSession* session, IRCPacket* packet, char* out, Command* command) {

	char* google_url = "http://google.com/#q=";
	char* pos = command->arg;

	char* result = malloc(strlen(pos) * 3);
	memset(result, 0, strlen(pos) * 3);

	if (result == NULL)
		return;

	format_query(pos, result);

	write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, google_url, result);

	free(result);
}

void search(IRCSession* session, IRCPacket* packet, char* out, Command* command) {

	char* search_url = "https://0x00sec.org/search?q=";
	char* pos = command->arg;

	CURL *curl = curl_easy_init();
	if (curl) {
		char* escaped = curl_easy_escape(curl, pos, strlen(pos));
		curl_easy_cleanup(curl);

		write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, search_url, escaped);

		curl_free(escaped);
	} else {
		return;
	}
}

void urban(IRCSession* session, IRCPacket* packet, char* out, Command* command) {

	char* urban_url = "https://www.urbandictionary.com/define.php?term=";
	char* pos = command->arg;

	char* result = calloc(strlen(pos) * 3, sizeof(char));

	if (result == NULL)
		return;

	format_query(pos, result);

	write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, urban_url, result);

	free(result);
}

void echo_config(IRCSession* session, IRCPacket* packet, char* out, Command* command, char** echoing) {

	if (*command->arg_first == '1') {

		if (*echoing != NULL) {
			free(*echoing);
		}

		*echoing = strdup(command->caller);

		write_to_socket(session, out, "\rPRIVMSG %s :Now echoing: %s\r\n", packet->channel, *echoing);					

		return;

	} else {

		write_to_socket(session, out, "\rPRIVMSG %s :No longer echoing: %s\r\n", packet->channel, *echoing);

		free(*echoing);
	}
}