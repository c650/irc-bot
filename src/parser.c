int parse_irc_packet(char* buf, IRCPacket* packet) {

	if (strstr(buf, "!") == NULL || strstr(buf, "@") == NULL) {
		return 0;
	}

	/* Get rid of \r\n */
	int n = strlen(buf);
	buf[n-1] = buf[n-2] = '\0';

	printf("[*] Analyzing %s\n", buf);

	packet->sender = strtok(buf, "!")+1; // gets user between : and !

	packet->realname = strtok(NULL, "@")+1; // skip over ~

	packet->hostname = strtok(NULL, " ");

	packet->type = strtok(NULL, " ");
	
	packet->channel = strtok(NULL, " ");

	packet->content = strtok(NULL, "\0");

	if ( packet->content != NULL && !strcmp(packet->type, "PRIVMSG") )
		packet->content++;

	if (!strcmp(packet->type, "NICK"))
		packet->channel++; // skip : in NICK messages

	printf("[+] packet->sender = %s\n", packet->sender);
	printf("[+] packet->realname = %s\n", packet->realname);
	printf("[+] packet->hostname = %s\n", packet->hostname);
	printf("[+] packet->type = %s\n", packet->type);
	printf("[+] packet->channel = %s\n", packet->channel);
	printf("[+] packet->content = %s\n", packet->content);

	return 1;

}