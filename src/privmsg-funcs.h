/*
	privmsg-funcs.h
*/

void slap(IRCSession* session, IRCPacket* packet, char* out, Command* command);
void google(IRCSession* session, IRCPacket* packet, char* out, Command* command);
void search(IRCSession* session, IRCPacket* packet, char* out, Command* command);
void echo_config(IRCSession* session, IRCPacket* packet, char* out, Command* command, char** echoing);
void kick_user(IRCSession* session, IRCPacket* packet, char* out, Command* command);