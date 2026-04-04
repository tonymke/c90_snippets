#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

static void test_str_dup(void)
{
	char *dup1 = NULL, *dup2 = NULL, *dup3 = NULL;
	const char *original;

	/* Test basic duplication */
	original = "hello";
	dup1 = str_dup(original);
	assert(dup1 != NULL);
	assert(strcmp(dup1, original) == 0);
	assert(dup1 != original);

	/* Test empty string */
	dup2 = str_dup("");
	assert(dup2 != NULL);
	assert(strlen(dup2) == 0);

	/* Test long string */
	original = "The quick brown fox jumps over the lazy dog";
	dup3 = str_dup(original);
	assert(dup3 != NULL);
	assert(strcmp(dup3, original) == 0);

	free(dup1);
	free(dup2);
	free(dup3);

	printf("  str_dup: OK\n");
}

static void test_str_lstrip(void)
{
	char buf[256];
	size_t len;

	/* Test no leading whitespace */
	strcpy(buf, "hello");
	len = str_lstrip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test leading spaces */
	strcpy(buf, "   hello");
	len = str_lstrip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test leading tabs and newlines */
	strcpy(buf, "\t\n  hello");
	len = str_lstrip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test all whitespace */
	strcpy(buf, "   \t\n  ");
	len = str_lstrip(buf);
	assert(len == 0);
	assert(strlen(buf) == 0);

	/* Test empty string */
	strcpy(buf, "");
	len = str_lstrip(buf);
	assert(len == 0);

	printf("  str_lstrip: OK\n");
}

static void test_str_rstrip(void)
{
	char buf[256];
	size_t len;

	/* Test no trailing whitespace */
	strcpy(buf, "hello");
	len = str_rstrip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test trailing spaces */
	strcpy(buf, "hello   ");
	len = str_rstrip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test trailing tabs and newlines */
	strcpy(buf, "hello\t\n  ");
	len = str_rstrip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test all whitespace */
	strcpy(buf, "   \t\n  ");
	len = str_rstrip(buf);
	assert(len == 0);
	assert(strlen(buf) == 0);

	/* Test empty string */
	strcpy(buf, "");
	len = str_rstrip(buf);
	assert(len == 0);

	printf("  str_rstrip: OK\n");
}

static void test_str_strip(void)
{
	char buf[256];
	size_t len;

	/* Test no whitespace */
	strcpy(buf, "hello");
	len = str_strip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test both leading and trailing spaces */
	strcpy(buf, "  hello world  ");
	len = str_strip(buf);
	assert(strcmp(buf, "hello world") == 0);
	assert(len == 11);

	/* Test leading spaces only */
	strcpy(buf, "  hello world");
	len = str_strip(buf);
	assert(strcmp(buf, "hello world") == 0);
	assert(len == 11);

	/* Test trailing spaces only */
	strcpy(buf, "hello world  ");
	len = str_strip(buf);
	assert(strcmp(buf, "hello world") == 0);
	assert(len == 11);

	/* Test various whitespace characters */
	strcpy(buf, "\t\n  hello  \t\n");
	len = str_strip(buf);
	assert(strcmp(buf, "hello") == 0);
	assert(len == 5);

	/* Test all whitespace */
	strcpy(buf, "   \t\n  ");
	len = str_strip(buf);
	assert(len == 0);
	assert(strlen(buf) == 0);

	/* Test empty string */
	strcpy(buf, "");
	len = str_strip(buf);
	assert(len == 0);

	printf("  str_strip: OK\n");
}

int main(void)
{
	printf("Running str tests...\n");

	test_str_dup();
	test_str_lstrip();
	test_str_rstrip();
	test_str_strip();

	printf("All str tests passed!\n");

	return 0;
}
