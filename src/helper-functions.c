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
char* parse_for_host(char* buf, char* pos) {

	pos = strtok(NULL, "@");
	pos = strtok(NULL, " ");	
	
	char* pos1 = malloc(strlen(pos)+1);
	strcpy(pos1, pos);

	if (strstr(pos, "/") != NULL && strstr(pos, ".") == NULL) {
		free(pos1);
		return NULL;
	} else {
		return pos1;
	}
}

void ip_lookup(char* pos, int sockfd, char* out, char* channel) {
	char sbuf[256];
	sprintf(sbuf, "./geoip.rb %s", pos);

	printf("\rRunning %s\n", sbuf);
	FILE* fp = popen(sbuf, "r");

	while (fgets(sbuf, 256, fp) != NULL) {
		//if (sbuf[0] == 0x00) continue;

		//pos = strstr(sbuf, "Location");
		sprintf(out, "\rPRIVMSG %s :%s", channel, sbuf);
		responsible_send(sockfd, out, strlen(out), 0);
	}

	pclose(fp);
}