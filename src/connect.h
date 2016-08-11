/*
	connect.h
*/
int connect_to_irc(IRCSession* session);
void log_on(IRCSession* session);
void join_channel(IRCSession* session);
void write_to_socket(IRCSession* session, char* buf, char* fmt, ...);