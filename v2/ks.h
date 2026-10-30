
/*
 * Kristen Sala
 *
 * ks.h
 * Lib for common C stuff so I don't have to rewrite them
 *
 * - Functions
 *    size_t ks_split_string_to_array(char ***buffer, size_t *size, char *str, char *delim);
 *    char *ks_current_date(chat *buffer, size_t buffer_size, char *format);
 *    wchar_t *ks_char_to_wide(char *input);
 *    char *ks_clone_string(const char *str);
 *    char *ks_string_to_lower(const chat *str);
 *
 */

#ifndef KS_H
#define KS_H

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <time.h>

size_t ks_split_string_to_array(char ***buffer, size_t *size, char *str, char *delim) {
    size_t len = 0;

    if (*size == 0) {
        return 0;
    }

    char *token = strtok(str, delim);
    if (token == NULL) {
        return 0;
    }

    (*buffer)[len] = token;
    len++;

    while ((token = strtok(NULL, delim)) != NULL) {
        if (len == *size) {
            if (*size > SIZE_MAX / sizeof **buffer / 2) {
                return 0;
            }

            size_t new_size = *size * 2;
            char **tmp = realloc(*buffer, sizeof(**buffer) * new_size);
            if (tmp == NULL) {
                puts("Realloc failed");
                return 0;
            }

            *buffer = tmp;
            *size = new_size;
        }

        (*buffer)[len++] = token;
    }

    return len;
}

char *ks_current_date(char *buffer, size_t buffer_size, char *format) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    if (format == NULL) {
        strftime(buffer, buffer_size, "%d-%m-%Y %H:%M:%S", t);
    } else {
        strftime(buffer, buffer_size, format, t);
    }
    return buffer;
}

wchar_t *ks_char_to_wide(char *input) {
    size_t capacity;
    wchar_t *dst;

    if (input == NULL)
        return NULL;

    capacity = strlen(input) + 1;
    if (capacity > (size_t)-1 / sizeof(wchar_t))
        return NULL;

    dst = malloc(capacity * sizeof *dst);
    if (dst == NULL)
        return NULL;

    if (mbstowcs(dst, input, capacity) == (size_t)-1) {
        free(dst);
        return NULL;
    }

    return dst;
}


/*
 * Allocates memory. Pair with free
 */
static char *ks_clone_string(const char *str) {
    size_t len = strlen(str); // strlen() does not include '\0'
    char *copy = malloc(len + 1);

    if (copy == NULL) {
        return NULL;
    }

    strcpy(copy, str); // str does include '\0'
    return copy;
}

static wchar_t *ks_clone_wstring(const wchar_t *str) {
    if (str == NULL) {
        return NULL;
    }

    size_t len = wcslen(str);
    wchar_t *copy = malloc((len + 1) * sizeof(*copy));

    if (copy == NULL) {
        return NULL;
    }

    wcscpy(copy, str); /* Includes the terminating L'\0'. */
    return copy;
}

/*
 * Allocates memory. Pair with free.
 * Does not modify the input and returns a cloned (lowercase) string
 */
static char *ks_string_to_lower(const char *str) {
    char *str_clone = ks_clone_string(str);

    for (size_t i = 0; str_clone[i] != '\0'; ++i) {
        str_clone[i] = (char)tolower((unsigned char)str_clone[i]);
    }

    return str_clone;
}

static wchar_t *ks_wide_to_lower(const wchar_t *str) {
    wchar_t *str_clone = ks_clone_wstring(str);

    for (wchar_t *p = str_clone; *p != L'\0'; ++p) {
        *p = (wchar_t)towlower(*p);
    }


    return str_clone;
}

#endif



