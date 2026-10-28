
/*
 * Kristen Sala
 *
 * ks.h
 * Lib for common C stuff so I don't have to rewrite them
 *
 * - Functions
 *    char **ks_split_string_to_array(char *str, char *delimiter, size_t size);
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
#include <stdlib.h>
#include <wchar.h>
#include <time.h>

char **ks_split_string_to_array(char *str, char *delimiter, size_t size) {
    char **array = malloc(sizeof(char *) * size);
    size_t count = 0;

    char *token = strtok(str, delimiter);
    array[count] = token;
    count++;

    while (token != NULL) {
        token = strtok(NULL, delimiter);
        array[count] = token;
        count++;
    }

    return array;
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

#endif



