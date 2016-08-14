/*
	helper-functions.h
*/

void write_to_socket(IRCSession* session, char* buf, char* fmt, ...);

void format_query(char* query_str, char* result);

char* parse_for_host(IRCPacket* packet);

void ip_lookup(char* host, char* out, IRCSession* session, IRCPacket* packet);

void send_args(char** argv, const size_t *argc, IRCSession* session, char* buf);