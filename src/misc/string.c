#include "string.h"

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    
    return (unsigned char)*s1 - (unsigned char)*s2;
}

char *strchr(const char *s, int c) {
    char ch = (char)c;

    while (*s) {
        if (*s == ch)
            return (char *)s;
        
        s++;
    }

    if (ch == '\0')
        return (char *)s;
    
    return NULL;
}

char *strtok(char *str, const char *delim) {
    static char *next_token = NULL;

    if (str != NULL)
        next_token = str;

    if (next_token == NULL)
        return NULL;
    
    char *token_start = next_token;

    while (*token_start && strchr(delim, *token_start))
        token_start++;
    
    if (*token_start == '\0') {
        next_token = NULL;

        return NULL;
    }

    char *token_end = token_start;

    while (*token_end && !strchr(delim, *token_end))
        token_end++;

    if (*token_end) {
        *token_end = '\0';
        next_token = token_end + 1;
    } else
        next_token = NULL;

    return token_start;
}