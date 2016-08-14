#ifndef BOT_PRIVMSG_H
#define BOT_PRIVMSG_H

/*
	privmsg-funcs.h
*/
#include "./irc-datatypes.h"

void slap(IRCSession* session, IRCPacket* packet, char* out, Command* command);
void query(IRCSession* session, IRCPacket* packet, char* out, Command* command, const char* main_url);
void search(IRCSession* session, IRCPacket* packet, char* out, Command* command);
void echo_config(IRCSession* session, IRCPacket* packet, char* out, Command* command, char** echoing);
void kick_user(IRCSession* session, IRCPacket* packet, char* out, Command* command);

#endif