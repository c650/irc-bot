void format_query(char* query_str, char* result) {

	char *token = strtok(query_str, ",.:;?!-_ ");
	while (token != NULL) {
		strcpy(result + strlen(result), token);
		strcpy(result + strlen(result), "+");

		token = strtok(NULL, ",.:;?!-_ ");
	}

	result[strlen(result) - 1] = 0x00;
}

void responsible_send(int sockfd, char* buf, size_t len, int special_options) {
	send(sockfd, buf, len, special_options);

	printf("Sending\n\t%s\n", buf);

	memset(buf, 0, len * sizeof(char));
}