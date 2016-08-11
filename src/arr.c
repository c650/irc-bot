/*
	arr.c
	char ** functions
*/

void arr_push_back(char*** arr, const char* s, size_t *arr_len) {
	if (*arr == NULL) {
		
		*arr = malloc(sizeof(char*));
		
		*arr[0] = strdup(s);
		*arr_len = 1;

		return;
	}

	printf("*arr = %p\n", *arr);
	*arr = realloc( *arr, (*arr_len + 1) * sizeof(char*) );
	printf("*arr = %p\n", *arr);
	
	(*arr)[(*arr_len)++] = strdup(s);
	printf("*arr[%i] = %s\n", (int)*arr_len-1, (*arr)[(*arr_len)-1]);

}

char* arr_find(char** arr, char* q, size_t *arr_len) {
	if (arr == NULL) {
		return NULL;
	}

	for (int i = 0; i < *arr_len; i++) {
		if ( !strcmp(arr[i], q) ) {
			return arr[i];
		}
	}

	return NULL;
}

void arr_free(char*** arr, size_t *arr_len) {
	
	if (*arr == NULL) {
		return;
	}

	for (int i = 0; i < *arr_len; i++) {
		if (*arr[i]) free(*arr[i]);
	}

	*arr_len = 0;
	
	free(*arr);
}

void arr_remove(char*** arr, char* s, size_t *arr_len) {
	if (*arr == NULL) {
		return;
	}

	char* loc = arr_find(*arr, s, arr_len);

	if (loc == NULL) {
		printf("[*] Was not in %s\n", s);
		return;
	}

	free(loc);

	if (*arr[*arr_len - 1] != NULL) {

		loc = strdup(*arr[*arr_len - 1]);
		free(*arr[*arr_len - 1]);

	}

	(*arr_len)--;

	*arr = realloc(*arr, *arr_len);
}