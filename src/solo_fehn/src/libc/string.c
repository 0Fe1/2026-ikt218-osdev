/*
 * string.c - tiny libc string helpers
 *
 * For now we only need strlen. We will grow this file as later
 * assignments demand more.
 */

#include <libc/string.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}
