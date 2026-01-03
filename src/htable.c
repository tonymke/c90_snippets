#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "htable.h"
#include "prime_po2s.h"

#ifndef HTABLE_ABSOLUTE_MINIMUM_CAP
#define HTABLE_ABSOLUTE_MINIMUM_CAP 2U
#endif

#ifndef HTABLE_LOWER_LOAD_FACTOR_BOUND
#define HTABLE_LOWER_LOAD_FACTOR_BOUND 0.15
#endif

#ifndef HTABLE_UPPER_LOAD_FACTOR_BOUND
#define HTABLE_UPPER_LOAD_FACTOR_BOUND 0.75
#endif

struct htable_bucket {
	unsigned int in_use : 1, is_deleted : 1;
	size_t hash;
	void *key, *value;
};

struct htable_t {
	size_t len, min_cap, cap;
	struct htable_bucket *buckets;

	htable_cmp_fn cmp_key; /* required */
	htable_hash_fn hash_key; /* required */
	htable_destroy_fn destroy_key, destroy_val; /* optional */
};

/* Resize the table to a new capacity and rehash all entries.
 * direction: Positive to grow, negative to shrink
 * Returns 0 on success, -1 on allocation failure.
 * Behavior when direction is 0 is undefined.
 */
static int htable_resize(htable_t *ht, int direction);

/* Find the bucket index for a given key.
 * Returns the index if found, or ht->cap if not found.
 */
static size_t htable_find(const htable_t *ht, const void *key);

htable_t *htable_create(size_t min_cap, htable_hash_fn hash_key,
			htable_cmp_fn cmp_key, htable_destroy_fn destroy_key,
			htable_destroy_fn destroy_val)
{
	htable_t *ht;
	size_t i, actual_cap;

	assert(hash_key != NULL);
	assert(cmp_key != NULL);

	/* Enforce minimum capacity */
	if (min_cap < HTABLE_ABSOLUTE_MINIMUM_CAP) {
		min_cap = HTABLE_ABSOLUTE_MINIMUM_CAP;
	}

	/* Find the first prime >= min_cap */
	actual_cap = prime_po2s[prime_po2s_cap - 1]; /* default to largest */
	for (i = 0; i < prime_po2s_cap; i++) {
		if (prime_po2s[i] >= min_cap) {
			actual_cap = prime_po2s[i];
			break;
		}
	}

	ht = malloc(sizeof(*ht));
	if (ht == NULL) {
		return NULL;
	}

	ht->buckets = calloc(actual_cap, sizeof(*ht->buckets));
	if (ht->buckets == NULL) {
		free(ht);
		return NULL;
	}

	ht->len = 0;
	ht->min_cap = min_cap;
	ht->cap = actual_cap;
	ht->hash_key = hash_key;
	ht->cmp_key = cmp_key;
	ht->destroy_key = destroy_key;
	ht->destroy_val = destroy_val;

	return ht;
}

void htable_destroy(htable_t *ht)
{
	size_t i;

	assert(ht != NULL);

	for (i = 0; i < ht->cap; i++) {
		if (ht->buckets[i].in_use && !ht->buckets[i].is_deleted) {
			if (ht->destroy_key != NULL) {
				ht->destroy_key(ht->buckets[i].key);
			}
			if (ht->destroy_val != NULL) {
				ht->destroy_val(ht->buckets[i].value);
			}
		}
	}

	free(ht->buckets);
	free(ht);
}

int htable_get(const htable_t *ht, const void *key, void **out_val)
{
	size_t idx;

	assert(ht != NULL);
	assert(key != NULL);

	idx = htable_find(ht, key);

	if (idx < ht->cap) {
		if (out_val != NULL) {
			*out_val = ht->buckets[idx].value;
		}
		return 0; /* Found */
	}

	return 1; /* Not found */
}

int htable_insert(htable_t *ht, void *key, void *val)
{
	size_t hash, idx, i;
	size_t insert_idx;
	int resize_result;

	assert(ht != NULL);
	assert(key != NULL);

	insert_idx = ht->cap; /* sentinel: not found */
	hash = ht->hash_key(key);

	/* Linear probe for the insertion point */
	for (i = 0; i < ht->cap; i++) {
		idx = (hash + i) % ht->cap;

		/* If we find an in_use, non-deleted bucket with matching key, it already exists */
		if (ht->buckets[idx].in_use && !ht->buckets[idx].is_deleted) {
			if (ht->cmp_key(ht->buckets[idx].key, key) == 0) {
				return 1; /* Key already exists */
			}
		}

		/* Record first available slot (empty or deleted) */
		if (insert_idx == ht->cap && (!ht->buckets[idx].in_use ||
					      (ht->buckets[idx].in_use &&
					       ht->buckets[idx].is_deleted))) {
			insert_idx = idx;
		}

		/* Stop probing once we hit a truly empty slot */
		if (!ht->buckets[idx].in_use) {
			break;
		}
	}

	/* Table is full (shouldn't happen with proper load factor management) */
	if (insert_idx >= ht->cap) {
		return -1;
	}
	ht->buckets[insert_idx].key = key;
	ht->buckets[insert_idx].value = val;
	ht->buckets[insert_idx].hash = hash;
	ht->buckets[insert_idx].in_use = 1;
	ht->buckets[insert_idx].is_deleted = 0;
	ht->len++;

	/* Check if we need to resize up */
	if ((double)ht->len / ht->cap > HTABLE_UPPER_LOAD_FACTOR_BOUND) {
		resize_result = htable_resize(ht, 1);
		if (resize_result != 0) {
			return resize_result;
		}
	}

	return 0;
}

int htable_replace(htable_t *ht, void *key, void *val)
{
	size_t idx;

	assert(ht != NULL);
	assert(key != NULL);

	idx = htable_find(ht, key);

	if (idx < ht->cap) {
		/* Destroy old key and value */
		if (ht->destroy_key != NULL) {
			ht->destroy_key(ht->buckets[idx].key);
		}
		if (ht->destroy_val != NULL) {
			ht->destroy_val(ht->buckets[idx].value);
		}
		/* Replace with new key and value */
		ht->buckets[idx].key = key;
		ht->buckets[idx].value = val;
		return 0; /* Replaced */
	}

	return 1; /* Key not found */
}

int htable_steal(htable_t *ht, const void *key, void **out_key, void **out_val)
{
	size_t idx;
	int resize_result;

	assert(ht != NULL);
	assert(key != NULL);

	idx = htable_find(ht, key);

	if (idx < ht->cap) {
		/* Return the key and value without destroying */
		if (out_key != NULL) {
			*out_key = ht->buckets[idx].key;
		}
		if (out_val != NULL) {
			*out_val = ht->buckets[idx].value;
		}

		/* Mark as deleted */
		ht->buckets[idx].is_deleted = 1;
		ht->len--;

		/* Check if we need to resize down */
		if ((double)ht->len / ht->cap <
		    HTABLE_LOWER_LOAD_FACTOR_BOUND) {
			resize_result = htable_resize(ht, -1);
			if (resize_result != 0) {
				return resize_result;
			}
		}

		return 0; /* Found and stolen */
	}

	return 1; /* Not found */
}

int htable_remove(htable_t *ht, const void *key)
{
	size_t idx;
	int resize_result;

	assert(ht != NULL);
	assert(key != NULL);

	idx = htable_find(ht, key);

	if (idx < ht->cap) {
		/* Destroy the key and value */
		if (ht->destroy_key != NULL) {
			ht->destroy_key(ht->buckets[idx].key);
		}
		if (ht->destroy_val != NULL) {
			ht->destroy_val(ht->buckets[idx].value);
		}

		/* Mark as deleted */
		ht->buckets[idx].is_deleted = 1;
		ht->len--;

		/* Check if we need to resize down */
		if ((double)ht->len / ht->cap <
		    HTABLE_LOWER_LOAD_FACTOR_BOUND) {
			resize_result = htable_resize(ht, -1);
			if (resize_result != 0) {
				return resize_result;
			}
		}

		return 0; /* Found and removed */
	}

	return 1; /* Not found */
}

int htable_clear(htable_t *ht)
{
	size_t i;

	assert(ht != NULL);

	/* Destroy all stored entries */
	for (i = 0; i < ht->cap; i++) {
		if (ht->buckets[i].in_use && !ht->buckets[i].is_deleted) {
			if (ht->destroy_key != NULL) {
				ht->destroy_key(ht->buckets[i].key);
			}
			if (ht->destroy_val != NULL) {
				ht->destroy_val(ht->buckets[i].value);
			}
		}
	}

	ht->len = 0;

	/* Clear all buckets before resizing */
	memset(ht->buckets, 0, ht->cap * sizeof(*ht->buckets));

	/* Resize down to min_cap */
	return htable_resize(ht, -1);
}

int htable_foreach(const htable_t *ht, htable_foreach_fn fn, void *user_data)
{
	size_t i;
	int result;

	assert(ht != NULL);
	assert(fn != NULL);

	for (i = 0; i < ht->cap; i++) {
		if (ht->buckets[i].in_use && !ht->buckets[i].is_deleted) {
			result = fn(ht->buckets[i].key, ht->buckets[i].value,
				    user_data);
			if (result != 0) {
				return result;
			}
		}
	}

	return 0;
}

size_t htable_cap(const htable_t *ht)
{
	assert(ht != NULL);
	return ht->cap;
}

size_t htable_len(const htable_t *ht)
{
	assert(ht != NULL);
	return ht->len;
}

static int htable_resize(htable_t *ht, int direction)
{
	size_t i, j, new_cap, idx;
	struct htable_bucket *old_buckets;
	size_t old_cap;

	assert(ht != NULL);
	assert(direction);

	old_buckets = ht->buckets;
	old_cap = ht->cap;

	/* Determine new capacity */
	if (direction > 0) {
		/* Growing: find next prime in table */
		new_cap =
			prime_po2s[prime_po2s_cap - 1]; /* default to largest */
		for (i = 0; i < prime_po2s_cap; i++) {
			if (prime_po2s[i] > old_cap) {
				new_cap = prime_po2s[i];
				break;
			}
		}
	} else {
		/* Shrinking: find previous prime or min_cap */
		new_cap = ht->min_cap;
		for (i = 0; i < prime_po2s_cap; i++) {
			if (prime_po2s[i] >= ht->min_cap &&
			    prime_po2s[i] < old_cap) {
				new_cap = prime_po2s[i];
			}
		}
	}

	/* No change needed */
	if (new_cap == old_cap) {
		return 0;
	}

	ht->buckets = calloc(new_cap, sizeof(*ht->buckets));
	if (ht->buckets == NULL) {
		ht->buckets = old_buckets;
		return -1;
	}

	ht->cap = new_cap;
	ht->len = 0; /* Will be recounted during rehashing */

	/* Rehash all entries */
	for (i = 0; i < old_cap; i++) {
		if (old_buckets[i].in_use && !old_buckets[i].is_deleted) {
			/* Re-insert the entry into the new table */
			for (j = 0; j < new_cap; j++) {
				idx = (old_buckets[i].hash + j) % new_cap;

				if (!ht->buckets[idx].in_use) {
					ht->buckets[idx] = old_buckets[i];
					ht->len++;
					break;
				}
			}
		}
	}

	free(old_buckets);
	return 0;
}

static size_t htable_find(const htable_t *ht, const void *key)
{
	size_t hash, idx, i;

	assert(ht != NULL);
	assert(key != NULL);

	hash = ht->hash_key(key);

	/* Linear probing to find key */
	for (i = 0; i < ht->cap; i++) {
		idx = (hash + i) % ht->cap;

		if (ht->buckets[idx].in_use && !ht->buckets[idx].is_deleted) {
			if (ht->cmp_key(ht->buckets[idx].key, key) == 0) {
				return idx; /* Found */
			}
		}

		/* If we hit an empty slot, key is not in table */
		if (!ht->buckets[idx].in_use) {
			return ht->cap; /* Not found */
		}
	}

	return ht->cap; /* Not found */
}
