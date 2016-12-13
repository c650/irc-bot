/*
	arr.c
	char ** functions
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "./include/arr.h"

void arr_push_back(char*** arr, const char* s, size_t *arr_len) {
	if (*arr == NULL) {

		*arr = malloc(sizeof(char*));

		(*arr)[0] = strdup(s);
		*arr_len = 1;

		return;
	}

	/*
		Let's ensure that we get more memory!
	*/
	char** tmp;
	if ( (tmp = realloc( *arr, (*arr_len + 1) * sizeof(char*) )) == NULL) {
		perror("arr_push_back couldnt get more memory.");
	}

	*arr = tmp;

	(*arr)[(*arr_len)++] = strdup(s);
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

	for (size_t i = 0; i < (*arr_len); i++) {

		printf("Deleting arr[%i] at %p, value %s\n", (int)i, (*arr)[i], (*arr)[i]);

		if ( (*arr)[i] != NULL) {
			free( (*arr)[i] );
		}
	}

	*arr_len = 0;

	free( (*arr) );
	*arr = NULL;

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

	/*
		Let's make sure we can shrink the array.
	*/
	char **tmp;
	if ( ( tmp = realloc( *arr, --(*arr_len) ) ) == NULL) {
		perror("arr_remove couldn't shrink the array.");
	}

	*arr = tmp;
}

char* concat_arr(char** arr, const size_t *arr_len) {
	size_t tot_len = 0;
	int i;

	for (i = 0; i < *arr_len; i++)
		tot_len += strlen(arr[i]);

	tot_len += *arr_len - 1; // alloc for spaces.

	char* concatted = malloc(sizeof(char) * tot_len);
	if (!concatted) {
		printf("[*] Failed to allocate.\n");
		return NULL;
	}

	concatted[0] = 0x00;

	for (i = 0; i < *arr_len; i++) {
		strcpy(concatted+strlen(concatted), arr[i]);

		if (i + 1 != *arr_len)
			strcpy(concatted+strlen(concatted), " ");

		//printf("concatted currently = %s\n", concatted);
	}

	return concatted;
}
