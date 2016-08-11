/*
	connect.c
*/
int connect_to_irc(IRCSession* session) {
	struct sockaddr_in serv_addr;
	struct hostent *server;

	session->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (session->sockfd < 0) {
		perror("socket could not be opened");
		exit(1);
	}

	server = gethostbyname(session->server);
	if (server == NULL) {
		perror("host not found");
		close(session->sockfd);
		exit(2);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	bcopy(
		(char*)server->h_addr,
		(char*)&serv_addr.sin_addr.s_addr,
		server->h_length
		);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(session->port);

	if ( connect(session->sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0 ) {
		perror("error connecting...");
		close(session->sockfd);
		exit(3);
	}
	return session->sockfd;
}

void log_on(IRCSession* session) {

	char buf[BUFFER_SIZE+1];

	write_to_socket(session, buf, "\rUSER %s 0 * :%s\r\n", session->nick, session->nick);
	write_to_socket(session, buf, "\rNICK %s\r\n", session->nick);
	write_to_socket(session, buf, "\rPRIVMSG NickServ :identify %s\r\n", session->pass);

}

void join_channel(IRCSession* session) {
	if (session->num_channels < 1) {
		printf("[*] No channel to join.\n");
		return;
	}

	char buf[BUFFER_SIZE+1];

	write_to_socket(session, buf, "\rJOIN %s\r\n", session->channels[ session->num_channels - 1 ] );
}