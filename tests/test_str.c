#include <stdlib.h>
#include <string.h>

#include "test.h"
#include "str.h"

static int test_str_dup(void)
{
	char *dup1 = NULL, *dup2 = NULL, *dup3 = NULL;
	const char *original;
	int result = 0;

	/* Test basic duplication */
	original = "hello";
	dup1 = str_dup(original);
	ASSERT(dup1 != NULL, "str_dup should not return NULL for valid input");
	ASSERT_EQ(strcmp(dup1, original), 0, "duplicated string should match original");
	ASSERT(dup1 != original, "dup should be a different pointer");

	/* Test empty string */
	dup2 = str_dup("");
	ASSERT(dup2 != NULL, "str_dup should handle empty strings");
	ASSERT_EQ(strlen(dup2), 0, "duplicated empty string should have length 0");

	/* Test long string */
	original = "The quick brown fox jumps over the lazy dog";
	dup3 = str_dup(original);
	ASSERT(dup3 != NULL, "str_dup should handle long strings");
	ASSERT_EQ(strcmp(dup3, original), 0, "long string should be duplicated correctly");

cleanup:
	free(dup1);
	free(dup2);
	free(dup3);

	if (result == 0) {
		printf("  str_dup: OK\n");
	}
	return result;
}

static int test_str_lstrip(void)
{
	char buf[256];
	size_t len;
	int result = 0;

	/* Test no leading whitespace */
	strcpy(buf, "hello");
	len = str_lstrip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "no leading whitespace should be unchanged");
	ASSERT_EQ(len, 5, "should return correct length");

	/* Test leading spaces */
	strcpy(buf, "   hello");
	len = str_lstrip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "leading spaces should be removed");
	ASSERT_EQ(len, 5, "should return correct length after lstrip");

	/* Test leading tabs and newlines */
	strcpy(buf, "\t\n  hello");
	len = str_lstrip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "various whitespace should be removed");
	ASSERT_EQ(len, 5, "should return correct length");

	/* Test all whitespace */
	strcpy(buf, "   \t\n  ");
	len = str_lstrip(buf);
	ASSERT_EQ(len, 0, "all whitespace should result in empty string");
	ASSERT_EQ(strlen(buf), 0, "string should be empty");

	/* Test empty string */
	strcpy(buf, "");
	len = str_lstrip(buf);
	ASSERT_EQ(len, 0, "empty string should have length 0");

cleanup:
	if (result == 0) {
		printf("  str_lstrip: OK\n");
	}
	return result;
}

static int test_str_rstrip(void)
{
	char buf[256];
	size_t len;
	int result = 0;

	/* Test no trailing whitespace */
	strcpy(buf, "hello");
	len = str_rstrip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "no trailing whitespace should be unchanged");
	ASSERT_EQ(len, 5, "should return correct length");

	/* Test trailing spaces */
	strcpy(buf, "hello   ");
	len = str_rstrip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "trailing spaces should be removed");
	ASSERT_EQ(len, 5, "should return correct length after rstrip");

	/* Test trailing tabs and newlines */
	strcpy(buf, "hello\t\n  ");
	len = str_rstrip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "various trailing whitespace should be removed");
	ASSERT_EQ(len, 5, "should return correct length");

	/* Test all whitespace */
	strcpy(buf, "   \t\n  ");
	len = str_rstrip(buf);
	ASSERT_EQ(len, 0, "all whitespace should result in empty string");
	ASSERT_EQ(strlen(buf), 0, "string should be empty");

	/* Test empty string */
	strcpy(buf, "");
	len = str_rstrip(buf);
	ASSERT_EQ(len, 0, "empty string should have length 0");

cleanup:
	if (result == 0) {
		printf("  str_rstrip: OK\n");
	}
	return result;
}

static int test_str_strip(void)
{
	char buf[256];
	size_t len;
	int result = 0;

	/* Test no whitespace */
	strcpy(buf, "hello");
	len = str_strip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "no whitespace should be unchanged");
	ASSERT_EQ(len, 5, "should return correct length");

	/* Test both leading and trailing spaces */
	strcpy(buf, "  hello world  ");
	len = str_strip(buf);
	ASSERT_EQ(strcmp(buf, "hello world"), 0, "both leading and trailing spaces should be removed");
	ASSERT_EQ(len, 11, "should return correct length after strip");

	/* Test leading spaces only */
	strcpy(buf, "  hello world");
	len = str_strip(buf);
	ASSERT_EQ(strcmp(buf, "hello world"), 0, "leading spaces should be removed");
	ASSERT_EQ(len, 11, "should return correct length");

	/* Test trailing spaces only */
	strcpy(buf, "hello world  ");
	len = str_strip(buf);
	ASSERT_EQ(strcmp(buf, "hello world"), 0, "trailing spaces should be removed");
	ASSERT_EQ(len, 11, "should return correct length");

	/* Test various whitespace characters */
	strcpy(buf, "\t\n  hello  \t\n");
	len = str_strip(buf);
	ASSERT_EQ(strcmp(buf, "hello"), 0, "various whitespace should be removed");
	ASSERT_EQ(len, 5, "should return correct length");

	/* Test all whitespace */
	strcpy(buf, "   \t\n  ");
	len = str_strip(buf);
	ASSERT_EQ(len, 0, "all whitespace should result in empty string");
	ASSERT_EQ(strlen(buf), 0, "string should be empty");

	/* Test empty string */
	strcpy(buf, "");
	len = str_strip(buf);
	ASSERT_EQ(len, 0, "empty string should have length 0");

cleanup:
	if (result == 0) {
		printf("  str_strip: OK\n");
	}
	return result;
}

int main(void)
{
	int result = 0;

	printf("Running str tests...\n");

	result |= test_str_dup();
	result |= test_str_lstrip();
	result |= test_str_rstrip();
	result |= test_str_strip();

	if (result == 0) {
		printf("All str tests passed!\n");
	}

	return result;
}
