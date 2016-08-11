/*
	irc-datatypes.c
*/

void init_session(IRCSession* session, char *server, char *nick, char *pass, int port, char* first_admin) {
	
	if (!session) {
		printf("[*] Error initializing session...\n");
		exit(5);
	}

	session->server = strdup(server);
	session->nick = strdup(nick);
	session->pass = strdup(pass);

	// sockfd to be initialized in main()
	session->port = port;

	session->channels = session->ignoring = NULL;
	session->num_channels = session->num_ignoring = 0;

	session->admins = malloc(sizeof(char*) * 1 /* to make obvious that we have one admin to start*/);
	session->admins[0] = strdup(first_admin);
	session->num_admins = 1;
}

void free_session(IRCSession* session) {
	if (session == NULL)
		return;

	printf("[*] Freeing a session\n");

	free(session->server);
	free(session->nick);
	free(session->pass);

	if (session->channels != NULL) {
		arr_free(&session->channels, &session->num_channels);
	}

	if (session->ignoring != NULL) {
		arr_free(&session->ignoring, &session->num_ignoring);
	}

	if (session->admins != NULL) {
		arr_free(&session->admins, &session->num_admins);
	}

	free(session);
}