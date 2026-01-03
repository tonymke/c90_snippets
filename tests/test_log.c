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

	/* Save original level */
	orig_level = log_level();

	/* Test default level */
	level = log_level();
	printf("  log_level_management: initial level is %d\n", level);

	/* Test setting and getting levels */
	log_level_set(LOG_LEVEL_DEBUG);
	if (log_level() != LOG_LEVEL_DEBUG) {
		fprintf(stderr, "FAIL: %s:%d: log_level_set(DEBUG) failed\n",
			__FILE__, __LINE__);
		log_level_set(orig_level);
		return 1;
	}

	log_level_set(LOG_LEVEL_ERR);
	if (log_level() != LOG_LEVEL_ERR) {
		fprintf(stderr, "FAIL: %s:%d: log_level_set(ERR) failed\n",
			__FILE__, __LINE__);
		log_level_set(orig_level);
		return 1;
	}

	log_level_set(LOG_LEVEL_EMERG);
	if (log_level() != LOG_LEVEL_EMERG) {
		fprintf(stderr, "FAIL: %s:%d: log_level_set(EMERG) failed\n",
			__FILE__, __LINE__);
		log_level_set(orig_level);
		return 1;
	}

	/* Restore original level */
	log_level_set(orig_level);

	printf("  log_level_management: OK\n");
	return 0;
}

static int test_log_file_management(void)
{
	FILE *orig_file, *temp_file;
	int result = 0;

	/* Save original file */
	orig_file = log_file();

	/* Create temporary file for testing */
	temp_file = tmpfile();
	if (temp_file == NULL) {
		fprintf(stderr, "FAIL: %s:%d: tmpfile() failed\n", __FILE__,
			__LINE__);
		return 1;
	}

	/* Test setting and getting file */
	log_file_set(temp_file);
	if (log_file() != temp_file) {
		fprintf(stderr, "FAIL: %s:%d: log_file_set failed\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

cleanup:
	log_file_set(orig_file);
	fclose(temp_file);

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
	if (temp_file == NULL) {
		fprintf(stderr, "FAIL: %s:%d: tmpfile() failed\n", __FILE__,
			__LINE__);
		return 1;
	}

	/* Set to DEBUG to capture all messages */
	log_level_set(LOG_LEVEL_DEBUG);
	log_file_set(temp_file);

	/* Emit some messages */
	log_emit(LOG_LEVEL_INFO, "test_log_emission", 123, "hello");
	log_emit(LOG_LEVEL_WARNING, "test_log_emission", 125, "warn %d", 42);
	log_emit(LOG_LEVEL_ERR, "test_log_emission", 127, "err %s", "test");

	/* Read back output */
	if (read_log_output(temp_file, buf, sizeof(buf)) < 0) {
		result = 1;
		goto cleanup;
	}

	/* Verify output is non-empty - just check that messages were logged */
	if (strlen(buf) == 0) {
		fprintf(stderr, "FAIL: %s:%d: no output captured\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

	/* Check for log level indicators */
	if (strstr(buf, "INFO") == NULL || strstr(buf, "WARNING") == NULL ||
	    strstr(buf, "ERR") == NULL) {
		fprintf(stderr, "FAIL: %s:%d: expected log levels not in output\n",
			__FILE__, __LINE__);
		result = 1;
		goto cleanup;
	}

cleanup:
	log_level_set(orig_level);
	log_file_set(orig_file);
	fclose(temp_file);

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
	if (temp_file == NULL) {
		fprintf(stderr, "FAIL: %s:%d: tmpfile() failed\n", __FILE__,
			__LINE__);
		return 1;
	}

	/* Set to WARNING level - DEBUG and INFO should not appear */
	log_level_set(LOG_LEVEL_WARNING);
	log_file_set(temp_file);

	log_emit(LOG_LEVEL_DEBUG, "test", 1, "dbg");
	log_emit(LOG_LEVEL_INFO, "test", 2, "inf");
	log_emit(LOG_LEVEL_WARNING, "test", 3, "wrn");
	log_emit(LOG_LEVEL_ERR, "test", 4, "err");

	/* Read back output */
	if (read_log_output(temp_file, buf, sizeof(buf)) < 0) {
		result = 1;
		goto cleanup;
	}

	/* Verify filtering: WARNING and above should appear */
	if (strlen(buf) == 0) {
		fprintf(stderr, "FAIL: %s:%d: no output captured\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

	if (strstr(buf, "WARNING") == NULL || strstr(buf, "ERR") == NULL) {
		fprintf(stderr, "FAIL: %s:%d: warning/error not in output\n",
			__FILE__, __LINE__);
		result = 1;
		goto cleanup;
	}

	/* DEBUG and INFO should not appear */
	if (strstr(buf, "DEBUG") != NULL) {
		fprintf(stderr,
			"FAIL: %s:%d: DEBUG should be filtered out\n",
			__FILE__, __LINE__);
		result = 1;
		goto cleanup;
	}

	if (strstr(buf, "INFO") != NULL) {
		fprintf(stderr, "FAIL: %s:%d: INFO should be filtered out\n",
			__FILE__, __LINE__);
		result = 1;
		goto cleanup;
	}

cleanup:
	log_level_set(orig_level);
	log_file_set(orig_file);
	fclose(temp_file);

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
	if (temp_file == NULL) {
		fprintf(stderr, "FAIL: %s:%d: tmpfile() failed\n", __FILE__,
			__LINE__);
		return 1;
	}

	/* Set to DEBUG to capture all */
	log_level_set(LOG_LEVEL_DEBUG);
	log_file_set(temp_file);

	/* Test various macros */
	log_info("msg1");
	log_warning1("msg2 %d", 123);
	log_err2("msg3 %s %d", "a", 456);
	log_debug3("msg4 %d %d %d", 1, 2, 3);

	/* Read back output */
	if (read_log_output(temp_file, buf, sizeof(buf)) < 0) {
		result = 1;
		goto cleanup;
	}

	/* Verify all messages appear by checking for level keywords */
	if (strlen(buf) == 0) {
		fprintf(stderr, "FAIL: %s:%d: no output captured\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

	if (strstr(buf, "INFO") == NULL) {
		fprintf(stderr, "FAIL: %s:%d: info macro failed\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

	if (strstr(buf, "WARNING") == NULL) {
		fprintf(stderr, "FAIL: %s:%d: warning1 macro failed\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

	if (strstr(buf, "ERR") == NULL) {
		fprintf(stderr, "FAIL: %s:%d: err2 macro failed\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

	if (strstr(buf, "DEBUG") == NULL) {
		fprintf(stderr, "FAIL: %s:%d: debug3 macro failed\n", __FILE__,
			__LINE__);
		result = 1;
		goto cleanup;
	}

cleanup:
	log_level_set(orig_level);
	log_file_set(orig_file);
	fclose(temp_file);

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
