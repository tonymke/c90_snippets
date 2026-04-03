#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"
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

static int test_log_level_management(void)
{
	enum log_level orig_level, level;
	FILE *orig_file, *temp_file;
	int result = 0;

	/* Save originals */
	orig_level = log_level();
	orig_file = log_file();

	/* Create temporary file for testing */
	temp_file = tmpfile();
	ASSERT(temp_file != NULL, "tmpfile() failed");

	/* Set to temporary file */
	log_file_set(temp_file);

	/* Test default level */
	level = log_level();
	printf("  log_level_management: initial level is %d\n", (int)level);

	/* Test setting and getting levels */
	log_level_set(LOG_LEVEL_DEBUG);
	ASSERT_EQ(log_level(), LOG_LEVEL_DEBUG, "log_level_set(DEBUG) failed");

	log_level_set(LOG_LEVEL_ERR);
	ASSERT_EQ(log_level(), LOG_LEVEL_ERR, "log_level_set(ERR) failed");

	log_level_set(LOG_LEVEL_EMERG);
	ASSERT_EQ(log_level(), LOG_LEVEL_EMERG, "log_level_set(EMERG) failed");

cleanup:
	log_level_set(orig_level);
	log_file_set(orig_file);
	if (temp_file) {
		fclose(temp_file);
	}

	if (result == 0) {
		printf("  log_level_management: OK\n");
	}
	return result;
}

static int test_log_file_management(void)
{
	FILE *orig_file, *temp_file;
	int result = 0;

	/* Save original file */
	orig_file = log_file();

	/* Create temporary file for testing */
	temp_file = tmpfile();
	ASSERT(temp_file != NULL, "tmpfile() failed");

	/* Test setting and getting file */
	log_file_set(temp_file);
	ASSERT_EQ(log_file(), temp_file, "log_file_set failed");

cleanup:
	log_level_set(LOG_LEVEL_INFO); /* default */
	log_file_set(orig_file);
	if (temp_file) {
		fclose(temp_file);
	}

	if (result == 0) {
		printf("  log_file_management: OK\n");
	}
	return result;
}

static int test_log_emission(void)
{
	FILE *orig_file, *temp_file;
	char buf[BUFFER_SIZE];
	enum log_level orig_level;
	int result = 0;

	/* Save originals */
	orig_file = log_file();
	orig_level = log_level();

	/* Create temporary file */
	temp_file = tmpfile();
	ASSERT(temp_file != NULL, "tmpfile() failed");

	/* Set to DEBUG to capture all messages */
	log_level_set(LOG_LEVEL_DEBUG);
	log_file_set(temp_file);

	/* Emit some messages */
	log_emit(LOG_LEVEL_INFO, "test_log_emission", 123, "hello");
	log_emit(LOG_LEVEL_WARNING, "test_log_emission", 125, "warn %d", 42);
	log_emit(LOG_LEVEL_ERR, "test_log_emission", 127, "err %s", "test");

	/* Read back output */
	ASSERT(read_log_output(temp_file, buf, sizeof(buf)) >= 0, "read_log_output failed");

	/* Verify output is non-empty - just check that messages were logged */
	ASSERT(strlen(buf) > 0, "no output captured");

	/* Check for log level indicators */
	ASSERT(strstr(buf, "INFO") != NULL, "INFO level not in output");
	ASSERT(strstr(buf, "WARNING") != NULL, "WARNING level not in output");
	ASSERT(strstr(buf, "ERR") != NULL, "ERR level not in output");

cleanup:
	log_level_set(orig_level);
	log_file_set(orig_file);
	if (temp_file) {
		fclose(temp_file);
	}

	if (result == 0) {
		printf("  log_emission: OK\n");
	}
	return result;
}

static int test_log_level_filtering(void)
{
	FILE *orig_file, *temp_file;
	char buf[BUFFER_SIZE];
	enum log_level orig_level;
	int result = 0;

	/* Save originals */
	orig_file = log_file();
	orig_level = log_level();

	/* Create temporary file */
	temp_file = tmpfile();
	ASSERT(temp_file != NULL, "tmpfile() failed");

	/* Set to WARNING level - DEBUG and INFO should not appear */
	log_level_set(LOG_LEVEL_WARNING);
	log_file_set(temp_file);

	log_emit(LOG_LEVEL_DEBUG, "test", 1, "dbg");
	log_emit(LOG_LEVEL_INFO, "test", 2, "inf");
	log_emit(LOG_LEVEL_WARNING, "test", 3, "wrn");
	log_emit(LOG_LEVEL_ERR, "test", 4, "err");

	/* Read back output */
	ASSERT(read_log_output(temp_file, buf, sizeof(buf)) >= 0, "read_log_output failed");

	/* Verify filtering: WARNING and above should appear */
	ASSERT(strlen(buf) > 0, "no output captured");
	ASSERT(strstr(buf, "WARNING") != NULL, "WARNING not in output");
	ASSERT(strstr(buf, "ERR") != NULL, "ERR not in output");

	/* DEBUG and INFO should not appear */
	ASSERT(strstr(buf, "DEBUG") == NULL, "DEBUG should be filtered out");
	ASSERT(strstr(buf, "INFO") == NULL, "INFO should be filtered out");

cleanup:
	log_level_set(orig_level);
	log_file_set(orig_file);
	if (temp_file) {
		fclose(temp_file);
	}

	if (result == 0) {
		printf("  log_level_filtering: OK\n");
	}
	return result;
}

static int test_log_macros(void)
{
	FILE *orig_file, *temp_file;
	char buf[BUFFER_SIZE];
	enum log_level orig_level;
	int result = 0;

	/* Save originals */
	orig_file = log_file();
	orig_level = log_level();

	/* Create temporary file */
	temp_file = tmpfile();
	ASSERT(temp_file != NULL, "tmpfile() failed");

	/* Set to DEBUG to capture all */
	log_level_set(LOG_LEVEL_DEBUG);
	log_file_set(temp_file);

	/* Test various macros */
	log_info("msg1");
	log_warning1("msg2 %d", 123);
	log_err2("msg3 %s %d", "a", 456);
	log_debug3("msg4 %d %d %d", 1, 2, 3);

	/* Read back output */
	ASSERT(read_log_output(temp_file, buf, sizeof(buf)) >= 0, "read_log_output failed");

	/* Verify all messages appear by checking for level keywords */
	ASSERT(strlen(buf) > 0, "no output captured");
	ASSERT(strstr(buf, "INFO") != NULL, "info macro failed");
	ASSERT(strstr(buf, "WARNING") != NULL, "warning1 macro failed");
	ASSERT(strstr(buf, "ERR") != NULL, "err2 macro failed");
	ASSERT(strstr(buf, "DEBUG") != NULL, "debug3 macro failed");

cleanup:
	log_level_set(orig_level);
	log_file_set(orig_file);
	if (temp_file) {
		fclose(temp_file);
	}

	if (result == 0) {
		printf("  log_macros: OK\n");
	}
	return result;
}

int main(void)
{
	int result = 0;

	printf("Running log tests...\n");

	result |= test_log_level_management();
	result |= test_log_file_management();
	result |= test_log_emission();
	result |= test_log_level_filtering();
	result |= test_log_macros();

	if (result == 0) {
		printf("All log tests passed!\n");
	}

	return result;
}
