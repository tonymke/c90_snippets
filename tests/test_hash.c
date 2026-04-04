#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

static void test_hash_int_rjenkins_nomult(void)
{
	size_t h1, h2;

	/* Test basic functionality */
	h1 = hash_int_rjenkins_nomult(0);
	h2 = hash_int_rjenkins_nomult(0);
	assert(h1 == h2);

	/* Test different inputs produce different hashes */
	h1 = hash_int_rjenkins_nomult(0);
	h2 = hash_int_rjenkins_nomult(1);
	assert(h1 != h2);

	/* Test known values */
	h1 = hash_int_rjenkins_nomult(42);
	assert(h1 != 42);

	printf("  hash_int_rjenkins_nomult: OK\n");
}

static void test_hash_int_knuth(void)
{
	size_t h1, h2;

	/* Test basic functionality */
	h1 = hash_int_knuth(0);
	h2 = hash_int_knuth(0);
	assert(h1 == h2);

	/* Test different inputs produce different hashes */
	h1 = hash_int_knuth(0);
	h2 = hash_int_knuth(1);
	assert(h1 != h2);

	printf("  hash_int_knuth: OK\n");
}

static void test_hash_int_multiandxor(void)
{
	size_t h1, h2;

	/* Test basic functionality */
	h1 = hash_int_multiandxor(0);
	h2 = hash_int_multiandxor(0);
	assert(h1 == h2);

	/* Test different inputs produce different hashes */
	h1 = hash_int_multiandxor(0);
	h2 = hash_int_multiandxor(1);
	assert(h1 != h2);

	printf("  hash_int_multiandxor: OK\n");
}

static void test_hash_cstring_djb2(void)
{
	size_t h1, h2;

	/* Test NULL handling */
	h1 = hash_cstring_djb2(NULL);
	assert(h1 == 0);

	/* Test empty string */
	h1 = hash_cstring_djb2("");
	assert(h1 == 5381);

	/* Test same strings produce same hash */
	h1 = hash_cstring_djb2("hello");
	h2 = hash_cstring_djb2("hello");
	assert(h1 == h2);

	/* Test different strings produce different hashes */
	h1 = hash_cstring_djb2("hello");
	h2 = hash_cstring_djb2("world");
	assert(h1 != h2);

	/* Test case sensitivity */
	h1 = hash_cstring_djb2("Hello");
	h2 = hash_cstring_djb2("hello");
	assert(h1 != h2);

	printf("  hash_cstring_djb2: OK\n");
}

static void test_hash_cstring_fnv_1a(void)
{
	size_t h1, h2;

	/* Test NULL handling */
	h1 = hash_cstring_fnv_1a(NULL);
	assert(h1 == 0);

	/* Test empty string */
	h1 = hash_cstring_fnv_1a("");
	assert(h1 == 2166136261UL);

	/* Test same strings produce same hash */
	h1 = hash_cstring_fnv_1a("hello");
	h2 = hash_cstring_fnv_1a("hello");
	assert(h1 == h2);

	/* Test different strings produce different hashes */
	h1 = hash_cstring_fnv_1a("hello");
	h2 = hash_cstring_fnv_1a("world");
	assert(h1 != h2);

	/* Test case sensitivity */
	h1 = hash_cstring_fnv_1a("Hello");
	h2 = hash_cstring_fnv_1a("hello");
	assert(h1 != h2);

	/* Test that djb2 and fnv1a produce different results */
	h1 = hash_cstring_djb2("test");
	h2 = hash_cstring_fnv_1a("test");
	assert(h1 != h2);

	printf("  hash_cstring_fnv_1a: OK\n");
}

int main(void)
{
	printf("Running hash tests...\n");

	test_hash_int_rjenkins_nomult();
	test_hash_int_knuth();
	test_hash_int_multiandxor();
	test_hash_cstring_djb2();
	test_hash_cstring_fnv_1a();

	printf("All hash tests passed!\n");

	return 0;
}
