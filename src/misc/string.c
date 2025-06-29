#include "string.h"

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- > 0) {
        if (*s1 != *s2) return (unsigned char)*s1 - (unsigned char)*s2;
        
        if (*s1 == '\0') break;
        
        s1++;
        s2++;
    }

    return 0;
}

char *strcpy(char *dest, const char *src) {
    char *original = dest;

    while ((*dest++ = *src++) != '\0');

    return original;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;

    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    while (i < n) {
        dest[i] = '\0';
        i++;
    }

    return dest;
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

char *strrchr(const char *s, int c) {
    const char *last = 0;
    char ch = (char)c;

    while (*s) {
        if (*s == ch)
            last = s;
        
        s++;
    }

    if (ch == '\0')
        return (char *)s;
    
    return (char *)last;
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

char *strcat(char *dest, const char *src) {
    char *ptr = dest;

    while (*ptr != '\0')
        ptr++;


    while (*src != '\0') {
        *ptr = *src;
        ptr++;
        src++;
    }

    *ptr = '\0';

    return dest;
}

char *strncat(char *dest, const char *src, unsigned int n) {
    char *d = dest;

    while (*d) d++;

    while (n-- && *src)
        *d++ = *src++;

    *d = '\0';

    return dest;
}

size_t strlen(const char *str) {
    size_t length = 0;

    while (str[length] != '\0')
        length++;

    return length;
}

char *strdup(const char *s) {
    if (s == NULL)
        return NULL;

    size_t len = strlen(s) + 1;
    char *copy = kmalloc(len, 0);

    if (copy == NULL)
        return NULL;

    memcpy(copy, s, len);

    return copy;
}


char *strndup(const char *src, size_t n) {
    size_t len = 0;

    while (len < n && src[len] != '\0')
        len++;

    char *dup = (char *)kmalloc(len + 1, 0);

    if (!dup)
        return NULL;

    for (size_t i = 0; i < len; i++)
        dup[i] = src[i];

    dup[len] = '\0';

    return dup;
}