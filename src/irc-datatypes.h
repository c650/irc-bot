#ifndef BOT_IRC_DT_H
#define BOT_IRC_DT_H
/*
	irc-datatypes.h
*/

typedef struct IRCSession {
	
	char	*server,
			*nick,
			*pass;

	int		sockfd,
			port;

	/*
		We are going to exist in several channels now.
		And the bot will now handle several admins.
		And can ignore.
	*/
	char	**channels,
			**admins,
			**ignoring;

	size_t num_channels, num_admins, num_ignoring;

} IRCSession;

typedef struct IRCPacket {

	char	*sender,
			*realname,
			*hostname,
			*type,
			*channel,

			*content;

} IRCPacket;

/*
	Each command can take one argument at this time.
*/
typedef struct Command {
	char	*cmd,
			**argv,
			*caller,
			*channel;
	size_t argc;
} Command;

#ifdef __cplusplus

extern "C" {

#endif

void init_session(IRCSession* session, char *server, char *nick, char *pass, int port, char* first_admin);
void free_session(IRCSession* session);

#ifdef __cplusplus
}
#endif



#endif