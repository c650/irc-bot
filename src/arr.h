/*
	arr.h
	char ** functions
*/

void arr_push_back(char*** arr, const char* s, size_t *arr_len);

char* arr_find(char** arr, char* q, size_t *arr_len);

void arr_free(char** arr, size_t *arr_len);

void arr_remove(char*** arr, char* s, size_t *arr_len);

char* concat_arr(char** arr, const size_t *arr_len);