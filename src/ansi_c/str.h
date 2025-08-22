#ifndef STR_H
#define STR_H

#include <stddef.h>

/* Duplicate the given string. */
char *str_dup(const char *s);

/* Remove leading whitespace from the given string, returning its new length. */
size_t str_lstrip(char *s);

/* Remove trailing whitespace from the given string, returning its new length. */
size_t str_rstrip(char *s);

/* Remove leading and trailing whitespace from the given string, returning its
 * new length. */
size_t str_strip(char *s);

#endif /* STR_H */
