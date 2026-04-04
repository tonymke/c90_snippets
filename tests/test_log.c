#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#define BUFFER_SIZE 4096

/* Helper to capture log output from a FILE stream */
static int read_log_output(FILE *f, char *buf, size_t bufsize)
{
	size_t nread;

	if (fseek(f, 0, SEEK_SET) != 0) {
		fprintf(stderr, "FAIL: %s:%d: fseek failed\n", __FILE__, __LINE__);
		return -1;
	}

	nread = fread(buf, 1, bufsize - 1, f);
	buf[nread] = '\0';

	if (ferror(f)) {
		fprintf(stderr, "FAIL: %s:%d: fread failed\n", __FILE__, __LINE__);
		return -1;
	}

	return (int)nread;
}

static void test_log_level_management(void)
{
	enum log_level orig_level, level;
	FILE *orig_file, *temp_file;

	/* Save originals */
	orig_level = log_level();
	orig_file = log_file();

	/* Create temporary file for testing */
	temp_file = tmpfile();
	assert(temp_file != NULL);

	/* Set to temporary file */
	log_file_set(temp_file);

	/* Test default level */
	level = log_level();
	printf("  log_level_management: initial level is %d\n", (int)level);

	/* Test setting and getting levels */
	log_level_set(LOG_LEVEL_DEBUG);
	assert(log_level() == LOG_LEVEL_DEBUG);

	log_level_set(LOG_LEVEL_ERR);
	assert(log_level() == LOG_LEVEL_ERR);

	log_level_set(LOG_LEVEL_EMERG);
	assert(log_level() == LOG_LEVEL_EMERG);

	/* Restoration */
	log_level_set(orig_level);
	log_file_set(orig_file);
	fclose(temp_file);

	printf("  log_level_management: OK\n");
}

static void test_log_file_management(void)
{
	FILE *orig_file, *temp_file;

	/* Save original file */
	orig_file = log_file();

	/* Create temporary file for testing */
	temp_file = tmpfile();
	assert(temp_file != NULL);

	/* Test setting and getting file */
	log_file_set(temp_file);
	assert(log_file() == temp_file);

	/* Restoration */
	log_level_set(LOG_LEVEL_INFO); /* default */
	log_file_set(orig_file);
	fclose(temp_file);

	printf("  log_file_management: OK\n");
}

static void test_log_emission(void)
{
	FILE *orig_file, *temp_file;
	char buf[BUFFER_SIZE];
	enum log_level orig_level;

	/* Save originals */
	orig_file = log_file();
	orig_level = log_level();

	/* Create temporary file */
	temp_file = tmpfile();
	assert(temp_file != NULL);

	/* Set to DEBUG to capture all messages */
	log_level_set(LOG_LEVEL_DEBUG);
	log_file_set(temp_file);

	/* Emit some messages */
	log_emit(LOG_LEVEL_INFO, "test_log_emission", 123, "hello");
	log_emit(LOG_LEVEL_WARNING, "test_log_emission", 125, "warn %d", 42);
	log_emit(LOG_LEVEL_ERR, "test_log_emission", 127, "err %s", "test");

	/* Read back output */
	assert(read_log_output(temp_file, buf, sizeof(buf)) >= 0);

	/* Verify output is non-empty - just check that messages were logged */
	assert(strlen(buf) > 0);

	/* Check for log level indicators */
	assert(strstr(buf, "INFO") != NULL);
	assert(strstr(buf, "WARNING") != NULL);
	assert(strstr(buf, "ERR") != NULL);

	/* Restoration */
	log_level_set(orig_level);
	log_file_set(orig_file);
	fclose(temp_file);

	printf("  log_emission: OK\n");
}

static void test_log_level_filtering(void)
{
	FILE *orig_file, *temp_file;
	char buf[BUFFER_SIZE];
	enum log_level orig_level;

	/* Save originals */
	orig_file = log_file();
	orig_level = log_level();

	/* Create temporary file */
	temp_file = tmpfile();
	assert(temp_file != NULL);

	/* Set to WARNING level - DEBUG and INFO should not appear */
	log_level_set(LOG_LEVEL_WARNING);
	log_file_set(temp_file);

	log_emit(LOG_LEVEL_DEBUG, "test", 1, "dbg");
	log_emit(LOG_LEVEL_INFO, "test", 2, "inf");
	log_emit(LOG_LEVEL_WARNING, "test", 3, "wrn");
	log_emit(LOG_LEVEL_ERR, "test", 4, "err");

	/* Read back output */
	assert(read_log_output(temp_file, buf, sizeof(buf)) >= 0);

	/* Verify filtering: WARNING and above should appear */
	assert(strlen(buf) > 0);
	assert(strstr(buf, "WARNING") != NULL);
	assert(strstr(buf, "ERR") != NULL);

	/* DEBUG and INFO should not appear */
	assert(strstr(buf, "DEBUG") == NULL);
	assert(strstr(buf, "INFO") == NULL);

	/* Restoration */
	log_level_set(orig_level);
	log_file_set(orig_file);
	fclose(temp_file);

	printf("  log_level_filtering: OK\n");
}

static void test_log_macros(void)
{
	FILE *orig_file, *temp_file;
	char buf[BUFFER_SIZE];
	enum log_level orig_level;

	/* Save originals */
	orig_file = log_file();
	orig_level = log_level();

	/* Create temporary file */
	temp_file = tmpfile();
	assert(temp_file != NULL);

	/* Set to DEBUG to capture all */
	log_level_set(LOG_LEVEL_DEBUG);
	log_file_set(temp_file);

	/* Test various macros */
	log_info("msg1");
	log_warning1("msg2 %d", 123);
	log_err2("msg3 %s %d", "a", 456);
	log_debug3("msg4 %d %d %d", 1, 2, 3);

	/* Read back output */
	assert(read_log_output(temp_file, buf, sizeof(buf)) >= 0);

	/* Verify all messages appear by checking for level keywords */
	assert(strlen(buf) > 0);
	assert(strstr(buf, "INFO") != NULL);
	assert(strstr(buf, "WARNING") != NULL);
	assert(strstr(buf, "ERR") != NULL);
	assert(strstr(buf, "DEBUG") != NULL);

	/* Restoration */
	log_level_set(orig_level);
	log_file_set(orig_file);
	fclose(temp_file);

	printf("  log_macros: OK\n");
}

int main(void)
{
	printf("Running log tests...\n");

	test_log_level_management();
	test_log_file_management();
	test_log_emission();
	test_log_level_filtering();
	test_log_macros();

	printf("All log tests passed!\n");

	return 0;
}
