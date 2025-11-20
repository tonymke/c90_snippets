#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

char *str_dup(const char *s)
{
	char *dup;
	size_t cap, len;

	assert(s != NULL);

	len = strlen(s);
	if (len >= SIZE_MAX) {
		return NULL;
	}
	cap = len + 1;

	dup = malloc(cap);
	if (dup == NULL) {
		return NULL;
	}

	return strcpy(dup, s);
}

size_t str_lstrip(char *s)
{
	char *cur, *first_nonspace;

	assert(s != NULL);

	/* Edge cases: we can tell from the first char there isn't whitespace to
	 * strip. */
	if (*s == '\0') {
		return 0;
	}
	if (!isspace(*s)) {
		return strlen(s);
	}

	first_nonspace = NULL;
	cur = s;
	while (*cur != '\0') {
		if (first_nonspace == NULL && !isspace(*cur)) {
			first_nonspace = cur;
		}
		cur++;
	}

	if (first_nonspace == NULL) {
		/* String is all whitespace. */
		*s = '\0';
		return 0;
	}

	memmove(s, first_nonspace, cur - first_nonspace + 1);
	return cur - first_nonspace;
}

size_t str_rstrip(char *s)
{
	char *cur, *first_trailing_space;

	assert(s != NULL);

	first_trailing_space = NULL;
	cur = s;

	while (*cur != '\0') {
		if (isspace(*cur)) {
			if (first_trailing_space == NULL) {
				first_trailing_space = cur;
			}
		} else {
			first_trailing_space = NULL;
		}

		cur++;
	}

	if (first_trailing_space == NULL) {
		return cur - s;
	}

	*first_trailing_space = '\0';
	return first_trailing_space - s;
}

size_t str_strip(char *s)
{
	char *cur, *term, *lstrip_up_to, *rstrip_from;

	assert(s != NULL);

	if (*s == '\0') {
		return 0;
	}

	lstrip_up_to = NULL;
	rstrip_from = NULL;
	cur = s;
	while (*cur != '\0') {
		if (isspace(*cur)) {
			/* If we haven't seen a space yet, this may be the start
			 * of the trailing whitespace. */
			if (rstrip_from == NULL) {
				rstrip_from = cur;
			}
		} else {
			/* If we're looking at a non-space, we were not in
			 * trailing whitespace. */
			rstrip_from = NULL;

			/* Record the first nonspace character we see to lstrip
			 * later. */
			if (lstrip_up_to == NULL) {
				lstrip_up_to = cur;
			}
		}

		cur++;
	}
	if (lstrip_up_to == NULL) {
		/* String is entirely whitespace - make it emptystring. */
		*s = '\0';
		return 0;
	}

	term = cur;

	if (rstrip_from != NULL) {
		/* Trailing whitespace was found. Trim it. */
		*rstrip_from = '\0';
		term = rstrip_from;
	}

	if (lstrip_up_to != s) {
		/* leading whitespace was found. Shift the string left. */
		memmove(s, lstrip_up_to, term - lstrip_up_to + 1);
	}

	return term - lstrip_up_to;
}
