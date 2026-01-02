#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "test.h"

static int test_hash_int_rjenkins_nomult(void)
{
	size_t h1, h2;

	/* Test basic functionality */
	h1 = hash_int_rjenkins_nomult(0);
	h2 = hash_int_rjenkins_nomult(0);
	ASSERT_EQ(h1, h2, "same input should produce same hash");

	/* Test different inputs produce different hashes */
	h1 = hash_int_rjenkins_nomult(0);
	h2 = hash_int_rjenkins_nomult(1);
	ASSERT(h1 != h2, "different inputs should produce different hashes");

	/* Test known values */
	h1 = hash_int_rjenkins_nomult(42);
	ASSERT(h1 != 42, "hash should transform the input");

	printf("  hash_int_rjenkins_nomult: OK\n");
	return 0;
}

static int test_hash_int_knuth(void)
{
	size_t h1, h2;

	/* Test basic functionality */
	h1 = hash_int_knuth(0);
	h2 = hash_int_knuth(0);
	ASSERT_EQ(h1, h2, "same input should produce same hash");

	/* Test different inputs produce different hashes */
	h1 = hash_int_knuth(0);
	h2 = hash_int_knuth(1);
	ASSERT(h1 != h2, "different inputs should produce different hashes");

	printf("  hash_int_knuth: OK\n");
	return 0;
}

static int test_hash_int_multiandxor(void)
{
	size_t h1, h2;

	/* Test basic functionality */
	h1 = hash_int_multiandxor(0);
	h2 = hash_int_multiandxor(0);
	ASSERT_EQ(h1, h2, "same input should produce same hash");

	/* Test different inputs produce different hashes */
	h1 = hash_int_multiandxor(0);
	h2 = hash_int_multiandxor(1);
	ASSERT(h1 != h2, "different inputs should produce different hashes");

	printf("  hash_int_multiandxor: OK\n");
	return 0;
}

static int test_hash_cstring_djb2(void)
{
	size_t h1, h2;

	/* Test NULL handling */
	h1 = hash_cstring_djb2(NULL);
	ASSERT_EQ(h1, 0, "NULL string should hash to 0");

	/* Test empty string */
	h1 = hash_cstring_djb2("");
	ASSERT(h1 == 5381, "empty string should return initial hash value");

	/* Test same strings produce same hash */
	h1 = hash_cstring_djb2("hello");
	h2 = hash_cstring_djb2("hello");
	ASSERT_EQ(h1, h2, "same string should produce same hash");

	/* Test different strings produce different hashes */
	h1 = hash_cstring_djb2("hello");
	h2 = hash_cstring_djb2("world");
	ASSERT(h1 != h2, "different strings should produce different hashes");

	/* Test case sensitivity */
	h1 = hash_cstring_djb2("Hello");
	h2 = hash_cstring_djb2("hello");
	ASSERT(h1 != h2, "hash should be case sensitive");

	printf("  hash_cstring_djb2: OK\n");
	return 0;
}

static int test_hash_cstring_fnv_1a(void)
{
	size_t h1, h2;

	/* Test NULL handling */
	h1 = hash_cstring_fnv_1a(NULL);
	ASSERT_EQ(h1, 0, "NULL string should hash to 0");

	/* Test empty string */
	h1 = hash_cstring_fnv_1a("");
	ASSERT(h1 == 2166136261UL, "empty string should return initial FNV offset");

	/* Test same strings produce same hash */
	h1 = hash_cstring_fnv_1a("hello");
	h2 = hash_cstring_fnv_1a("hello");
	ASSERT_EQ(h1, h2, "same string should produce same hash");

	/* Test different strings produce different hashes */
	h1 = hash_cstring_fnv_1a("hello");
	h2 = hash_cstring_fnv_1a("world");
	ASSERT(h1 != h2, "different strings should produce different hashes");

	/* Test case sensitivity */
	h1 = hash_cstring_fnv_1a("Hello");
	h2 = hash_cstring_fnv_1a("hello");
	ASSERT(h1 != h2, "hash should be case sensitive");

	/* Test that djb2 and fnv1a produce different results */
	h1 = hash_cstring_djb2("test");
	h2 = hash_cstring_fnv_1a("test");
	ASSERT(h1 != h2, "different hash algorithms should produce different results");

	printf("  hash_cstring_fnv_1a: OK\n");
	return 0;
}

int main(void)
{
	int result = 0;

	printf("Running hash tests...\n");

	result |= test_hash_int_rjenkins_nomult();
	result |= test_hash_int_knuth();
	result |= test_hash_int_multiandxor();
	result |= test_hash_cstring_djb2();
	result |= test_hash_cstring_fnv_1a();

	if (result == 0) {
		printf("All hash tests passed!\n");
	}

	return result;
}
