#include "./connect.h"

int connect_to_irc(int* sockfd, char* irc_channel, int port) {
	struct sockaddr_in serv_addr;
	struct hostent *server;

	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sockfd < 0) {
		perror("socket could not be opened");
		exit(1);
	}

	server = gethostbyname("irc.freenode.net");
	if (server == NULL) {
		perror("host not found");
		close(*sockfd);
		exit(2);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	bcopy(
		(char*)server->h_addr,
		(char*)&serv_addr.sin_addr.s_addr,
		server->h_length
		);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(IRC_PORT);

	if ( connect(*sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0 ) {
		perror("error connecting...");
		close(*sockfd);
		exit(3);
	}

	return *sockfd;
}
int get_on_channel(int* sockfd, char* channel, char* nick, char* buf, char* password) {

	write_to_socket(sockfd, buf, "\rUSER %s 0 * :%s\r\n", nick, nick);
	write_to_socket(sockfd, buf, "\rNICK %s\r\n", nick);
	write_to_socket(sockfd, buf, "\rPRIVMSG NickServ :identify %s\r\n", password);
	
	write_to_socket(sockfd, buf, "\rJOIN %s\r\n", channel);
	

	return 0;
}
int write_to_socket(int* sockfd, char* buf, char* fmt, ...) {
	memset(buf, 0, BUFFER_SIZE);

	va_list ap;
	va_start(ap,fmt);

	vsnprintf(buf, BUFFER_SIZE, fmt, ap);

	va_end(ap);

	write(*sockfd, buf, strlen(buf));

	return 0;
}