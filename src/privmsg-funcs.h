/*
	privmsg-funcs.h
*/

void slap(IRCSession* session, IRCPacket* packet, char* out, Command* command);
void query(IRCSession* session, IRCPacket* packet, char* out, Command* command, const char* main_url);
void search(IRCSession* session, IRCPacket* packet, char* out, Command* command);
void echo_config(IRCSession* session, IRCPacket* packet, char* out, Command* command, char** echoing);
void kick_user(IRCSession* session, IRCPacket* packet, char* out, Command* command);