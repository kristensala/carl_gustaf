/*
 * Kristen Sala
 *
 * tinkers.h
 * Lib for common C stuff so I don't have to rewrite them
 *
 * - Functions
 *    char *clone_string(const char *str);
 *    char *string_to_lower(const chat *str);
 *
 */

#ifndef TINKERS_H
#define TINKERS_H

#include <string.h>
#include <assert.h>

/*
 * Allocates memory. Pair with free
 */
static inline char *clone_string(const char *str) {
    assert(str != NULL);

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
static inline char *string_to_lower(const char *str) {
    char *str_clone = clone_string(str);
    assert(str_clone != NULL);

    for (size_t i = 0; str_clone[i] != '\0'; ++i) {
        str_clone[i] = (char)tolower((unsigned char)str_clone[i]);
    }

    return str_clone;
}

#endif



