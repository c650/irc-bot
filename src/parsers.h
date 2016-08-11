/*
	parsers.h
*/

int parse_irc_packet(char* buf, IRCPacket* packet);

int parse_for_command(IRCPacket* packet,  Command* command);