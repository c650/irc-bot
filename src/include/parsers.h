#ifndef BOT_PARSERS_H
#define BOT_PARSERS_H
/*
	parsers.h
*/
#include "./irc-datatypes.h"

#ifdef __cplusplus

extern "C" {

#endif

int parse_irc_packet(char* buf, IRCPacket* packet);

int parse_for_command(IRCPacket* packet,  Command* command);

#ifdef __cplusplus
}
#endif

#endif
