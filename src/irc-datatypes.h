typedef struct IRCSession {
	
	char	*server,
			*channel,
			*nick,
			*pass;

	int		sockfd,
			port;

} IRCSession;

typedef struct IRCPacket {

	char	*sender,
			*realname,
			*hostname,
			*type,
			*channel,

			*content;

} IRCPacket;