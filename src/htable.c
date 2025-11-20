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

static const struct load_factor_bounds {
	double lower, upper;
} load_factor_bounds = {
	HTABLE_LOWER_LOAD_FACTOR_BOUND, /* lower */
	HTABLE_UPPER_LOAD_FACTOR_BOUND /* upper */
};

struct htable_t {
	size_t len, min_cap, cap;
	struct htable_bucket {
		short in_use;
		size_t hash;
		void *key, *value;
	} *buckets;

	htable_cmp_fn cmp_key; /* required */
	htable_hash_fn hash_key; /* required */
	htable_destroy_fn destroy_key, destroy_val; /* optional */
};

/*
* Destroy each stored key/value in the given hashtable's buckets array. ht->len
* is updated accordingly, but no resizing or free is performed on ht->buckets
* itself.
*/
static void destroy_key_values(htable_t *ht);

/* Locate the bucket for the provided hash and key.

If the caller has already hashed the key, it may provide it.

Only returns NULL if the buckets array is not allocated at all.
*/
static struct htable_bucket *find_bucket_by_key(htable_t *ht, void *key,
						const size_t *precomputed_hash);

/*
 * Inspect the given pointer and return non-zero if it points
 * to a hashtable in a valid state.
 */
static int is_valid_htable(htable_t *ht);

/*
 * Inspect the given pointer and return non-zero if it points to a bucket
 * in a valid state.
 */
static int is_valid_bucket(struct htable_bucket *b);

/*
* Determine the optimal capacity for bucketing the given minimum
* capacity and length, also considering the desired load factor bounds.
* 
* A return value of 0 indicates the platform cannot allocate enough to
* accomodate, and can be treated as an out-of-memory error.
*/
static size_t optimal_cap(size_t min_cap, size_t len,
			  const struct load_factor_bounds *lfb);

/*
 * Resize the given hashtable's buckets array for the current optimal capacity
 * if the array's length were to be adjusted to the given new_len.
 * 
 * If the hashtable is already optimally allocated, no writes are performed.
 *
 * If new_len is 0 and the hashtable is found to not be optimally allocated, an
 * adjustment is still performed.
 * 
 * A non-zero return-value indicates reallocation failure.
 */
static int optimize_buckets_for_len(struct htable_t *ht, size_t new_len,
				    const struct load_factor_bounds *lfb);

htable_t *htable_create(size_t min_cap, htable_hash_fn hash_key,
			htable_cmp_fn cmp_key, htable_destroy_fn destroy_key,
			htable_destroy_fn destroy_val)
{
	htable_t *ht = NULL;

	assert(hash_key != NULL);
	assert(cmp_key != NULL);

	ht = malloc(sizeof(*ht));
	if (ht == NULL) {
		goto error;
	}

	ht->len = 0;
	ht->min_cap = min_cap;
	ht->cap = 0;
	ht->buckets = NULL;
	ht->cmp_key = cmp_key;
	ht->hash_key = hash_key;
	ht->destroy_key = destroy_key;
	ht->destroy_val = destroy_val;

	if (optimize_buckets_for_len(ht, 0, &load_factor_bounds)) {
		goto error;
	}
	assert(ht->cap);
	assert(ht->buckets != NULL);

	return ht;
error:
	if (ht != NULL) {
		free(ht->buckets);
		free(ht);
	}
	return NULL;
}

void htable_destroy(htable_t *ht)
{
	assert(is_valid_htable(ht));

	destroy_key_values(ht);
	free(ht->buckets);
	free(ht);
}

size_t htable_min_cap(htable_t *ht)
{
	assert(is_valid_htable(ht));

	return ht->min_cap;
}

int htable_set_min_cap(htable_t *ht, size_t new_min_cap)
{
	assert(is_valid_htable(ht));

	ht->min_cap = new_min_cap;

	return optimize_buckets_for_len(ht, ht->len, &load_factor_bounds);
}

size_t htable_cap(htable_t *ht)
{
	assert(is_valid_htable(ht));

	return ht->cap;
}

size_t htable_len(htable_t *ht)
{
	assert(is_valid_htable(ht));

	return ht->len;
}

int htable_clear(htable_t *ht)
{
	assert(is_valid_htable(ht));

	destroy_key_values(ht);
	assert(!ht->len);

	return optimize_buckets_for_len(ht, ht->len, &load_factor_bounds);
}

int htable_contains(htable_t *ht, void *key)
{
	struct htable_bucket *b;

	assert(is_valid_htable(ht));

	b = find_bucket_by_key(ht, key, NULL);
	if (b == NULL) {
		return 0;
	}
	assert(is_valid_bucket(b));
	return !!b->in_use;
}

void *htable_get(htable_t *ht, void *key)
{
	struct htable_bucket *b;

	assert(is_valid_htable(ht));

	b = find_bucket_by_key(ht, key, NULL);
	if (b == NULL) {
		return NULL;
	}

	assert(is_valid_bucket(b));
	if (!b->in_use) {
		return NULL;
	}

	return b->value;
}

int htable_remove(htable_t *ht, void *key)
{
	struct htable_bucket *b;

	assert(is_valid_htable(ht));

	b = find_bucket_by_key(ht, key, NULL);
	if (b == NULL)
		return 0;

	assert(is_valid_bucket(b));
	if (!b->in_use) {
		return 0;
	}

	assert(ht->len);
	if (ht->destroy_val != NULL) {
		ht->destroy_val(b->value);
	}
	b->value = NULL;
	if (ht->destroy_key != NULL) {
		ht->destroy_key(b->key);
	}
	b->key = NULL;
	b->in_use = b->hash = 0;

	if (optimize_buckets_for_len(ht, --ht->len, &load_factor_bounds)) {
		return -1;
	}

	return 1;
}

int htable_set(htable_t *ht, void *key, void *value)
{
	struct htable_bucket *b;
	short found_existing = 0;
	size_t hash;

	assert(is_valid_htable(ht));
	assert(ht->cap);
	assert(ht->buckets);

	hash = ht->hash_key(key);
	b = find_bucket_by_key(ht, key, &hash);
	assert(b != NULL);

	if (b->in_use) {
		if (ht->destroy_val != NULL) {
			ht->destroy_val(b->value);
		}
		found_existing = 1;
	} else {
		assert(ht->len < (size_t)-1);
	}

	b->in_use = 1;
	b->hash = hash;
	b->key = key;
	b->value = value;

	if (!found_existing) {
		if (optimize_buckets_for_len(ht, ht->len + 1,
					     &load_factor_bounds)) {
			return -1;
		}
		ht->len++;
	}

	return found_existing;
}

static void destroy_key_values(struct htable_t *ht)
{
	size_t i;

	assert(is_valid_htable(ht));

	for (i = 0; ht->len && i < ht->cap; i++) {
		struct htable_bucket *b = &ht->buckets[i];
		assert(is_valid_bucket(b));

		if (!b->in_use) {
			continue;
		}

		if (ht->destroy_val != NULL) {
			ht->destroy_val(b->value);
		}
		b->value = NULL;
		if (ht->destroy_key != NULL) {
			ht->destroy_key(b->key);
		}
		b->key = NULL;

		b->hash = b->in_use = 0;
		assert(ht->len);
		ht->len--;
	}
}

static struct htable_bucket *find_bucket_by_key(htable_t *ht, void *key,
						const size_t *precomputed_hash)
{
	/* Linear probe 
	   TODO quadratic probe? */

	struct htable_bucket *b;
	size_t i0, i, hash;

	assert(is_valid_htable(ht));

	if (!ht->cap || ht->buckets == NULL) {
		return NULL;
	}

	if (precomputed_hash == NULL) {
		hash = ht->hash_key(key);
	} else {
		hash = *precomputed_hash;
	}

	b = NULL;
	i0 = i = hash % ht->cap;
	while (b == NULL || i0 != i) {
		b = &ht->buckets[i];

		/* no bucket with this key, and this is the first non-empty match.
		return it so it can be used in a set operation
		*/
		if (!b->in_use) {
			return b;
		}

		if (ht->cmp_key(key, b->key)) {
			/* found a matching key */
			return b;
		}

		i++;
		if (i >= ht->cap) {
			i = 0;
		}
	}
	/* Neither key nor an empty bucket found. Impossible, since we should
	 * always have space. Dump core for debugging. */
	abort();
}

static int is_valid_htable(htable_t *ht)
{
	if (ht == NULL) {
		return 0;
	}

	if (ht->len > ht->cap) {
		return 0;
	}

	if (ht->cap) {
		if (ht->cap < ht->min_cap) {
			return 0;
		}
		if (ht->buckets == NULL) {
			return 0;
		}
	}

	if (ht->hash_key == NULL) {
		return 0;
	}

	if (ht->cmp_key == NULL) {
		return 0;
	}

	return 1;
}

static int is_valid_bucket(struct htable_bucket *b)
{
	if (b == NULL) {
		return 0;
	}

	if (!b->in_use) {
		if (b->hash != 0) {
			return 0;
		}
		if (b->key != NULL) {
			return 0;
		}
		if (b->value != NULL) {
			return 0;
		}
	}

	return 1;
}

static size_t optimal_cap(size_t min_cap, size_t len,
			  const struct load_factor_bounds *lfb)
{
	size_t i;

	for (i = 0; i < prime_po2s_cap; i++) {
		unsigned long candidate = prime_po2s[i];

		if (candidate < HTABLE_ABSOLUTE_MINIMUM_CAP) {
			continue;
		}

		if (candidate > (unsigned long)((size_t)-1)) {
			/* exceeded valid sizes we could allocate */
			break;
		}

		if (candidate < len) {
			continue;
		}

		if (candidate < min_cap) {
			continue;
		}

		if (len && lfb != NULL && lfb->upper) {
			double load_factor;

			assert(lfb->lower >= 0.0 && lfb->lower < 1.0);
			assert(lfb->upper > 0.0 && lfb->lower < 1.0);
			assert(lfb->lower <= lfb->upper);
			assert((double)candidate > 0.0);

			load_factor = (double)len / (double)candidate;

			if (load_factor > lfb->upper) {
				continue;
			}
		}

		return (size_t)candidate;
	}
	/* no valid candidate found - system can't allocate enough space. */
	return 0;
}

static int optimize_buckets_for_len(struct htable_t *ht, size_t new_len,
				    const struct load_factor_bounds *lfb)
{
	struct htable_t new;
	short may_need_realloc = 0;
	size_t i, old_len;

	assert(is_valid_htable(ht));

	/* discern if we need to do anything */
	if (ht->buckets == NULL || ht->cap < HTABLE_ABSOLUTE_MINIMUM_CAP ||
	    ht->cap < ht->min_cap || ht->cap < new_len) {
		may_need_realloc = 1;
	} else if (lfb != NULL && ht->cap) {
		double load_factor = 0.0;

		assert(lfb->lower >= 0.0 && lfb->lower < 1.0);
		assert(lfb->upper > 0.0 && lfb->lower < 1.0);
		assert(lfb->lower <= lfb->upper);

		load_factor = (double)new_len / (double)ht->cap;
		if (load_factor >= lfb->upper) {
			may_need_realloc = 1;
		} else if (load_factor <= lfb->lower) {
			may_need_realloc = -1;
		}
	}

	if (!may_need_realloc) {
		return 0;
	}

	/* we do - make a new ht on the stack and copy all elements in.
	 Then free the old bucket and apply that state to the given ht pointer.*/

	memcpy(&new, ht, sizeof(new));

	new.cap = optimal_cap(ht->min_cap, new_len, lfb);
	if (!new.cap) {
		return -1; /* ENOMEM */
	}
	assert(ht->cap != new.cap);

	new.buckets = calloc(new.cap, sizeof(*ht->buckets));
	if (new.buckets == NULL) {
		return -1;
	}

	old_len = ht->len;
	for (i = 0; old_len && i < ht->cap; i++) {
		struct htable_bucket *old_bucket = &ht->buckets[i], *new_bucket;

		if (!old_bucket->in_use) {
			continue;
		}

		new_bucket = &new.buckets[old_bucket->hash % new.cap];
		new_bucket->in_use = 1;
		new_bucket->hash = old_bucket->hash;
		new_bucket->key = old_bucket->key;
		new_bucket->value = old_bucket->value;
		old_len--;
	}
	assert(!old_len);

	free(ht->buckets);
	ht->buckets = new.buckets;
	ht->cap = new.cap;

	return 0;
}
