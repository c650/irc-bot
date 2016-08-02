/*
:oaktree!~purple@unaffiliated/oaktree347 PRIVMSG ##0x00sec
 :it will make it HELLA easier to parse
*/


int parse_irc_packet(char* buf, IRCPacket* packet) {

	if (strstr(buf, "!") == NULL || strstr(buf, "@") == NULL) {
		return 0;
	}

	printf("Analyzing %s\n", buf);

	int n = strlen(buf);
	buf[n-1] = buf[n-2] = '\0';

	// for (int i = 0, n = strlen(buf); i < n; i++) {
	// 	printf("%i ", (int)buf[i]);
	// }
	// printf("\n");

	packet->sender = strtok(buf, "!")+1; // gets user between : and !
	printf("\tpacket->sender = %s\n", packet->sender);

	packet->realname = strtok(NULL, "@")+1; // skip over ~
	printf("\tpacket->realname = %s\n", packet->realname);

	packet->hostname = strtok(NULL, " ");
	printf("\tpacket->hostname = %s\n", packet->hostname);

	packet->type = strtok(NULL, " ");
	printf("\tpacket->type = %s\n", packet->type);

	packet->channel = strtok(NULL, " ");
	printf("\tpacket->channel = %s\n", packet->channel);

	packet->content = strtok(NULL, "\0");
	if (packet->content != NULL)
		packet->content++;
	
	printf("\tpacket->content = %s\n", packet->content);
	return 1;

}