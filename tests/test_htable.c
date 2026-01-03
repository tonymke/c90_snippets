#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "htable.h"
#include "str.h"
#include "hash.h"
#include "test.h"

static size_t string_hash_wrapper(const void *p)
{
	return hash_cstring_djb2(p);
}

static int string_cmp(const void *a, const void *b)
{
	return strcmp((const char *)a, (const char *)b);
}

static int test_htable_create_destroy(void)
{
	htable_t *ht = NULL;
	int result = 0;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}
	if (htable_len(ht) != 0) {
		fprintf(stderr, "FAIL: %s:%d: new table should be empty\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (htable_cap(ht) <= 0) {
		fprintf(stderr, "FAIL: %s:%d: new table should have capacity\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_create_destroy: OK\n");
	}
	return result;
}

static int test_htable_insert_get(void)
{
	htable_t *ht = NULL;
	char *val = NULL;
	int result = 0;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	/* Insert a key-value pair */
	result = htable_insert(ht, str_dup("key1"), str_dup("value1"));
	if (result != 0) {
		fprintf(stderr, "FAIL: %s:%d: first insert should succeed\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: len should be 1 after insert\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Get the value back */
	result = htable_get(ht, "key1", (void **)&val);
	if (result != 0) {
		fprintf(stderr, "FAIL: %s:%d: get should find the key\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (val == NULL || strcmp(val, "value1") != 0) {
		fprintf(stderr,
			"FAIL: %s:%d: get should return correct value\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Try to insert duplicate key */
	{
		char *dup_key = str_dup("key1");
		char *dup_val = str_dup("value2");
		result = htable_insert(ht, dup_key, dup_val);
		if (result != 1) {
			fprintf(stderr,
				"FAIL: %s:%d: insert duplicate should return 1\n",
				__FILE__, __LINE__);
			free(dup_key);
			free(dup_val);
			goto cleanup;
		}
		/* When insert fails (result != 0), the key/value are not consumed by the table */
		if (result != 0) {
			free(dup_key);
			free(dup_val);
		}
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: len should still be 1\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Get non-existent key */
	result = htable_get(ht, "nonexistent", (void **)&val);
	if (result != 1) {
		fprintf(stderr,
			"FAIL: %s:%d: get non-existent should return 1\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_insert_get: OK\n");
	}
	return result;
}

static int test_htable_replace(void)
{
	htable_t *ht = NULL;
	char *val = NULL;
	int result = 0;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	/* Insert initial value */
	if (htable_insert(ht, str_dup("key1"), str_dup("original")) != 0) {
		fprintf(stderr, "FAIL: %s:%d: initial insert failed\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: should have 1 item\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	/* Replace existing key */
	result = htable_replace(ht, str_dup("key1"), str_dup("updated"));
	if (result != 0) {
		fprintf(stderr,
			"FAIL: %s:%d: replace existing should return 0\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: len should still be 1\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Verify new value */
	if (htable_get(ht, "key1", (void **)&val) != 0 ||
	    strcmp(val, "updated") != 0) {
		fprintf(stderr, "FAIL: %s:%d: should have new value\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Try to replace non-existent key */
	{
		char *repl_key = str_dup("nonexistent");
		char *repl_val = str_dup("value");
		result = htable_replace(ht, repl_key, repl_val);
		if (result != 1) {
			fprintf(stderr,
				"FAIL: %s:%d: replace non-existent should return 1\n",
				__FILE__, __LINE__);
			free(repl_key);
			free(repl_val);
			goto cleanup;
		}
		/* When replace fails (result != 0), the key/value are not consumed by the table */
		if (result != 0) {
			free(repl_key);
			free(repl_val);
		}
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: len should not change\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_replace: OK\n");
	}
	return result;
}

/* Test remove */
static int test_htable_remove(void)
{
	htable_t *ht = NULL;
	int result = 0;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	/* Insert some keys */
	if (htable_insert(ht, str_dup("key1"), str_dup("value1")) != 0 ||
	    htable_insert(ht, str_dup("key2"), str_dup("value2")) != 0) {
		fprintf(stderr, "FAIL: %s:%d: inserts failed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 2) {
		fprintf(stderr, "FAIL: %s:%d: should have 2 items\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	/* Remove existing key */
	result = htable_remove(ht, "key1");
	if (result != 0) {
		fprintf(stderr, "FAIL: %s:%d: remove should succeed\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: len should be 1 after remove\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Verify it's gone */
	result = htable_get(ht, "key1", NULL);
	if (result != 1) {
		fprintf(stderr,
			"FAIL: %s:%d: removed key should not be found\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Other key should still exist */
	result = htable_get(ht, "key2", NULL);
	if (result != 0) {
		fprintf(stderr, "FAIL: %s:%d: other key should still exist\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Try to remove non-existent key */
	result = htable_remove(ht, "nonexistent");
	if (result != 1) {
		fprintf(stderr,
			"FAIL: %s:%d: remove non-existent should return 1\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_remove: OK\n");
	}
	return result;
}

static int test_htable_steal(void)
{
	htable_t *ht = NULL;
	char *key = NULL, *val = NULL;
	int result = 0;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	/* Insert a key-value pair */
	if (htable_insert(ht, str_dup("key1"), str_dup("value1")) != 0) {
		fprintf(stderr, "FAIL: %s:%d: insert failed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: should have 1 item\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	/* Steal the entry */
	result = htable_steal(ht, "key1", (void **)&key, (void **)&val);
	if (result != 0) {
		fprintf(stderr, "FAIL: %s:%d: steal should succeed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 0) {
		fprintf(stderr, "FAIL: %s:%d: len should be 0 after steal\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (key == NULL || strcmp(key, "key1") != 0) {
		fprintf(stderr, "FAIL: %s:%d: should return key\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (val == NULL || strcmp(val, "value1") != 0) {
		fprintf(stderr, "FAIL: %s:%d: should return value\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	/* Try to steal non-existent key */
	result = htable_steal(ht, "nonexistent", NULL, NULL);
	if (result != 1) {
		fprintf(stderr,
			"FAIL: %s:%d: steal non-existent should return 1\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	/* Clean up stolen memory */
	free(key);
	free(val);
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_steal: OK\n");
	}
	return result;
}

/* Test foreach with short-circuiting */
static int foreach_count = 0;
static int foreach_callback(void *key, void *val, void *user_data)
{
	int *limit;
	(void)key;
	(void)val;

	foreach_count++;
	limit = (int *)user_data;
	if (limit && foreach_count >= *limit) {
		return 1; /* short-circuit */
	}
	return 0;
}

static int test_htable_foreach(void)
{
	htable_t *ht = NULL;
	int result = 0, limit;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	/* Insert some items */
	if (htable_insert(ht, str_dup("key1"), str_dup("value1")) != 0 ||
	    htable_insert(ht, str_dup("key2"), str_dup("value2")) != 0 ||
	    htable_insert(ht, str_dup("key3"), str_dup("value3")) != 0) {
		fprintf(stderr, "FAIL: %s:%d: inserts failed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 3) {
		fprintf(stderr, "FAIL: %s:%d: should have 3 items\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	/* Iterate all items */
	foreach_count = 0;
	result = htable_foreach(ht, foreach_callback, NULL);
	if (result != 0) {
		fprintf(stderr, "FAIL: %s:%d: full iteration should return 0\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (foreach_count != 3) {
		fprintf(stderr, "FAIL: %s:%d: should visit all 3 items\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Iterate with short-circuit */
	foreach_count = 0;
	limit = 2;
	result = htable_foreach(ht, foreach_callback, &limit);
	if (result != 1) {
		fprintf(stderr,
			"FAIL: %s:%d: short-circuited iteration should return 1\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (foreach_count != 2) {
		fprintf(stderr, "FAIL: %s:%d: should stop after 2 items\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_foreach: OK\n");
	}
	return result;
}

/* Test clear */
static int test_htable_clear(void)
{
	htable_t *ht = NULL;
	int result = 0;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	/* Insert some items */
	if (htable_insert(ht, str_dup("key1"), str_dup("value1")) != 0 ||
	    htable_insert(ht, str_dup("key2"), str_dup("value2")) != 0) {
		fprintf(stderr, "FAIL: %s:%d: inserts failed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 2) {
		fprintf(stderr, "FAIL: %s:%d: should have 2 items\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	/* Clear the table */
	result = htable_clear(ht);
	if (result != 0) {
		fprintf(stderr, "FAIL: %s:%d: clear should succeed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 0) {
		fprintf(stderr,
			"FAIL: %s:%d: table should be empty after clear\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Verify we can insert again */
	result = htable_insert(ht, str_dup("key3"), str_dup("value3"));
	if (result != 0) {
		fprintf(stderr,
			"FAIL: %s:%d: insert after clear should succeed\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: should have 1 item\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_clear: OK\n");
	}
	return result;
}

/* Test tombstone reuse (insert after delete) */
static int test_htable_tombstone_reuse(void)
{
	htable_t *ht = NULL;
	char *val = NULL;
	int result = 0;

	ht = htable_create(10, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	/* Insert, delete, then insert again */
	if (htable_insert(ht, str_dup("key1"), str_dup("value1")) != 0) {
		fprintf(stderr, "FAIL: %s:%d: insert failed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr,
			"FAIL: %s:%d: should have 1 item after insert\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	if (htable_remove(ht, "key1") != 0) {
		fprintf(stderr, "FAIL: %s:%d: remove failed\n", __FILE__,
			__LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 0) {
		fprintf(stderr,
			"FAIL: %s:%d: should have 0 items after remove\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* This should reuse the tombstone slot */
	result = htable_insert(ht, str_dup("key2"), str_dup("value2"));
	if (result != 0) {
		fprintf(stderr,
			"FAIL: %s:%d: insert after delete should succeed\n",
			__FILE__, __LINE__);
		goto cleanup;
	}
	if (htable_len(ht) != 1) {
		fprintf(stderr, "FAIL: %s:%d: should have 1 item\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	/* Verify the new value */
	result = htable_get(ht, "key2", (void **)&val);
	if (result != 0 || val == NULL || strcmp(val, "value2") != 0) {
		fprintf(stderr,
			"FAIL: %s:%d: should find the new key with correct value\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_tombstone_reuse: OK\n");
	}
	return result;
}

/* Test resizing (insert many items) */
static int test_htable_resize(void)
{
	htable_t *ht = NULL;
	char key_buf[32];
	size_t initial_cap, final_cap;
	int i, result = 0;

	ht = htable_create(2, string_hash_wrapper, string_cmp, free, free);
	if (ht == NULL) {
		fprintf(stderr, "FAIL: %s:%d: create should return non-NULL\n",
			__FILE__, __LINE__);
		return 1;
	}

	initial_cap = htable_cap(ht);

	/* Insert many items to trigger resize */
	for (i = 0; i < 20; i++) {
		sprintf(key_buf, "key%d", i);
		result = htable_insert(ht, str_dup(key_buf), str_dup("value"));
		if (result != 0) {
			fprintf(stderr,
				"FAIL: %s:%d: all inserts should succeed\n",
				__FILE__, __LINE__);
			goto cleanup;
		}
	}

	if (htable_len(ht) != 20) {
		fprintf(stderr, "FAIL: %s:%d: should have 20 items\n", __FILE__,
			__LINE__);
		goto cleanup;
	}

	final_cap = htable_cap(ht);
	if (final_cap <= initial_cap) {
		fprintf(stderr,
			"FAIL: %s:%d: capacity should grow after many inserts\n",
			__FILE__, __LINE__);
		goto cleanup;
	}

	/* Verify all items are still findable */
	for (i = 0; i < 20; i++) {
		sprintf(key_buf, "key%d", i);
		result = htable_get(ht, key_buf, NULL);
		if (result != 0) {
			fprintf(stderr,
				"FAIL: %s:%d: all inserted keys should be findable\n",
				__FILE__, __LINE__);
			goto cleanup;
		}
	}

	result = 0;
cleanup:
	htable_destroy(ht);
	if (result == 0) {
		printf("  htable_resize: OK\n");
	}
	return result;
}

int main(void)
{
	int result = 0;

	printf("Running hash table tests...\n");

	result |= test_htable_create_destroy();
	result |= test_htable_insert_get();
	result |= test_htable_replace();
	result |= test_htable_remove();
	result |= test_htable_steal();
	result |= test_htable_foreach();
	result |= test_htable_clear();
	result |= test_htable_tombstone_reuse();
	result |= test_htable_resize();

	if (result == 0) {
		printf("\nAll tests passed!\n");
	} else {
		printf("\nSome tests failed!\n");
	}

	return result;
}
