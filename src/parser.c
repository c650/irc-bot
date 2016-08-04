/*
:oaktree!~purple@unaffiliated/oaktree347 PRIVMSG ##0x00sec
 :it will make it HELLA easier to parse
*/


int parse_irc_packet(char* buf, IRCPacket* packet) {

	if (strstr(buf, "!") == NULL || strstr(buf, "@") == NULL) {
		return 0;
	}

	/* Get rid of \r\n */
	int n = strlen(buf);
	buf[n-1] = buf[n-2] = '\0';

	printf("[*] Analyzing %s\n", buf);

	packet->sender = strtok(buf, "!")+1; // gets user between : and !
	printf("[+] packet->sender = %s\n", packet->sender);

	packet->realname = strtok(NULL, "@")+1; // skip over ~
	printf("[+] packet->realname = %s\n", packet->realname);

	packet->hostname = strtok(NULL, " ");
	printf("[+] packet->hostname = %s\n", packet->hostname);

	packet->type = strtok(NULL, " ");
	printf("[+] packet->type = %s\n", packet->type);

	packet->channel = strtok(NULL, " ");
	printf("[+] packet->channel = %s\n", packet->channel);

	packet->content = strtok(NULL, "\0");
	if (packet->content != NULL && !strcmp(packet->type, "PRIVMSG"))
		packet->content++;
	
	printf("[+] packet->content = %s\n", packet->content);

	return 1;

}