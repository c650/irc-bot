void format_query(char* query_str, char* result) {

	char *token = strtok(query_str, ",.:;?!-_ \n\r");
	while (token != NULL) {
		strcpy(result + strlen(result), token);
		strcpy(result + strlen(result), "+");

		token = strtok(NULL, ",.:;?!-_ ");
	}

	result[strlen(result) - 1] = '\0';
}

char* parse_for_host(IRCPacket* packet) {

	char* pos = packet->hostname;

	if (strstr(pos, "/") != NULL && strstr(pos, ".") == NULL) {
		return NULL;
	} else {
		return pos;
	}
}

void ip_lookup(char* host, char* out, IRCSession* session) {
	char sbuf[256];
	sprintf(sbuf, "./geoip.rb %s", host);

	printf("Running %s\n", sbuf);
	FILE* fp = popen(sbuf, "r");

	while (fgets(sbuf, 256, fp) != NULL) {
		if (sbuf[0] == 0x00) continue;

		write_to_socket(session, out, "\rPRIVMSG %s :%s", session->channel, sbuf);
	}

	pclose(fp);
}