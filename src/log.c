#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "log.h"
#include "str.h"

#ifndef LOG_FILE_INITIAL
#define LOG_FILE_INITIAL stderr
#endif

#ifndef LOG_LEVEL_INITIAL
#define LOG_LEVEL_INITIAL LOG_LEVEL_WARNING
#endif

static FILE *current_file = NULL;
static enum log_level current_lvl = LOG_LEVEL_INITIAL;

static int is_valid_log_level(enum log_level lvl);
static const char *log_level_s(enum log_level lvl);

int log_emit(enum log_level lvl, const char *file, unsigned lineno,
	     const char *fmt, ...)
{
	int written = -1;
	va_list ap;
	time_t now;
	char *now_s;
	const char *lvl_s;

	assert(is_valid_log_level(lvl));
	assert(file != NULL);
	assert(fmt != NULL);

	if (lvl > current_lvl) {
		return 0;
	}

	now = time(NULL);
	if (now == (time_t)-1) {
		return -1;
	}
	now_s = ctime(&now);
	if (now_s == NULL) {
		return -1;
	}
	str_rstrip(now_s);

	lvl_s = log_level_s(lvl);
	assert(lvl_s != NULL);

	written = fprintf(log_file(),
			  ("[%s]" /* time */
			   " "
			   "%s" /* level */
			   " "
			   "%s:%d" /* file:lineno */
			   " "),
			  now_s, lvl_s, file, lineno);
	if (written < 0) {
		return -1;
	}

	va_start(ap, fmt);
	written = vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (written < 0) {
		return -1;
	}

	written = fprintf(stderr, "\n");
	if (written < 0) {
		return -1;
	}

	if (fflush(stderr) == EOF) {
		return -1;
	}

	return 0;
}

enum log_level log_level(void)
{
	assert(is_valid_log_level(current_lvl));
	return current_lvl;
}

void log_level_set(enum log_level lvl)
{
	assert(is_valid_log_level(current_lvl));
	assert(is_valid_log_level(lvl));

	current_lvl = lvl;
}

FILE *log_file(void)
{
	if (current_file == NULL) {
		log_file_set(LOG_FILE_INITIAL);
	}
	return current_file;
}

void log_file_set(FILE *f)
{
	assert(f != NULL);
	current_file = f;
}

static int is_valid_log_level(enum log_level lvl)
{
	switch (lvl) {
	case LOG_LEVEL_EMERG:
	case LOG_LEVEL_ALERT:
	case LOG_LEVEL_CRIT:
	case LOG_LEVEL_ERR:
	case LOG_LEVEL_WARNING:
	case LOG_LEVEL_NOTICE:
	case LOG_LEVEL_INFO:
	case LOG_LEVEL_DEBUG:
		return 1;
	default:
		return 0;
	}
}

static const char *log_level_s(enum log_level lvl)
{
	static const char *level_ss[] = { "EMERG",   "ALERT",  "CRIT", "ERR",
					  "WARNING", "NOTICE", "INFO", "DEBUG" };

	assert(is_valid_log_level(lvl));
	return level_ss[lvl];
}
