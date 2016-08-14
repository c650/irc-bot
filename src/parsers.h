#ifndef BOT_PARSERS_H
#define BOT_PARSERS_H
/*
	parsers.h
*/
#include "./irc-datatypes.h"

int parse_irc_packet(char* buf, IRCPacket* packet);

int parse_for_command(IRCPacket* packet,  Command* command);

#endif