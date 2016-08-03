void slap(IRCSession* session, IRCPacket* packet, char* out, const char* admin);
void google(IRCSession* session, IRCPacket* packet, char* out);
void search(IRCSession* session, IRCPacket* packet, char* out);
void echo_config(IRCSession* session, IRCPacket* packet, char* out, char** echoing);
void kick_user(IRCSession* session, IRCPacket* packet, char* out);

void slap(IRCSession* session, IRCPacket* packet, char* out, const char* admin) {

	char* pos = strstr(packet->content, "@slap ") + 6;

	int prob = rand() % 10;
	if (prob < 1) {
		
		char *tmp = strchr(pos, ' ');
		
		int was_null = 1;
		if (tmp != NULL) {
			*tmp = '\0';
			was_null = 0;
		}
		
		write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION slapped %s so hard he got kicked from the channel!\001\r\n", packet->channel, pos);					

		sleep(2);

		if (strcmp(admin, pos))
			write_to_socket(session, out, "\rKICK %s %s\r\n", packet->channel, pos);

		if (!was_null)
			*tmp = ' ';
	} else {
		write_to_socket(session, out, "\rPRIVMSG %s :\001ACTION slapped the hell outta %s\001\r\n", packet->channel, pos);					
	}
}

void google(IRCSession* session, IRCPacket* packet, char* out) {

	char* google_url = "http://google.com/#q=";
	char* pos = strstr(packet->content, "@google ") + 8;

	char* result = malloc(strlen(pos) * 3);
	memset(result, 0, strlen(pos) * 3);

	if (result == NULL)
		return;

	format_query(pos, result);

	write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, google_url, result);

	free(result);
}

void search(IRCSession* session, IRCPacket* packet, char* out) {

	char* search_url = "https://0x00sec.org/search?q=";
	char* pos = strstr(packet->content, "@search ") + 8;

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

void urban(IRCSession* session, IRCPacket* packet, char* out) {

	char* urban_url = "https://www.urbandictionary.com/define.php?term=";
	char* pos = strstr(packet->content, "@urban ") + 7;

	char* result = malloc(strlen(pos) * 3);
	memset(result, 0, strlen(pos) * 3);

	if (result == NULL)
		return;

	format_query(pos, result);

	write_to_socket(session, out, "\rPRIVMSG %s :%s%s\r\n", packet->channel, urban_url, result);

	free(result);
}

void echo_config(IRCSession* session, IRCPacket* packet, char* out, char** echoing) {

	char* command = strstr(packet->content, "@echo ") + 6;
	if (*command == '1') {

		if (*echoing != NULL) {
			free(*echoing);
		}

		*echoing = malloc( strlen(packet->sender) + 1 );
		strcpy(*echoing, packet->sender);

		write_to_socket(session, out, "\rPRIVMSG %s :Now echoing: %s\r\n", packet->channel, *echoing);					

		return;

	} else {

		write_to_socket(session, out, "\rPRIVMSG %s :No longer echoing: %s\r\n", packet->channel, *echoing);

		free(*echoing);
		*echoing = NULL;
	}
}

void kick_user(IRCSession* session, IRCPacket* packet, char* out) {

	char* pos = strstr(packet->content, "@kick ") + 6;
		
	char *tmp = strchr(pos, ' ');
	
	int was_null = 1;
	if (tmp != NULL) {
		*tmp = '\0';
		was_null = 0;
	}

	write_to_socket(session, out, "\rKICK %s %s\r\n", packet->channel, pos);

	if (!was_null)
		*tmp = ' ';
}