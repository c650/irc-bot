/*
	parsers.c
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

	packet->realname = strtok(NULL, "@"); // skip over ~
	if (packet->realname != NULL && *packet->realname == '~')
		packet->realname++;

	packet->hostname = strtok(NULL, " ");

	packet->type = strtok(NULL, " ");
	
	packet->channel = strtok(NULL, " ");

	packet->content = strtok(NULL, "\0");

	if ( packet->content != NULL && (!strcmp(packet->type, "PRIVMSG") || !strcmp(packet->type, "NOTICE")))
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

int parse_for_command(IRCPacket* packet,  Command* command) {
	char* start = strchr(packet->content, '@');
	
	if (start == NULL || start + 1 == NULL) {
		return 0;
	}
	start++;

	command->cmd = strtok(start, " \0");
	command->arg = strtok(NULL, "\0");

	command->arg_first = strdup(command->arg);
	char* s = strchr(command->arg_first, ' ');
	if (s) *s = 0x00;
	command->arg_first = realloc(command->arg_first, strlen(command->arg_first) * sizeof(char));

	command->caller = packet->sender;
	command->channel = packet->channel;

	for (int i = 0, n = strlen(command->cmd); i < n; i++) {
		command->cmd[i] = tolower(command->cmd[i]);
	}

	printf("[+] command->cmd = %s\n", command->cmd);
	printf("[+] command->arg = %s\n", command->arg);

	return 1;
}