#include <string.h>


const char *allowed_path_chars = 
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789"
    ".-_/ ";
const char *allowed_alphanum= 
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";


int is_safe_path(const char *path) {
	if (!path) return 0;
	for (size_t i = 0; path[i];i++) if (!strchr(allowed_path_chars, path[i])) return 0;
	return	(strstr(path, "..") == NULL) &&
		(path[0] != '-') &&
		(path[0] != '/') &&
		(strchr(path, '$') == NULL);
}

int is_buffer_alphanum(const char *buffer) {
	if (!buffer) return 0;
	for (size_t i = 0; buffer[i];i++) if (!strchr(allowed_alphanum, buffer[i])) return 0;
	return 1;
}


