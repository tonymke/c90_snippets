#ifndef HTABLE_H
#define HTABLE_H

/* A generic open-addressing, linear-probing hash table. */

#include <stddef.h>

typedef struct htable_t htable_t;

typedef int (*htable_cmp_fn)(void *a, void *b);
typedef int (*htable_hash_fn)(void *p);
typedef void (*htable_destroy_fn)(void *p);

htable_t *htable_create(size_t min_cap, htable_hash_fn hash_key,
			htable_cmp_fn cmp_key, htable_destroy_fn destroy_key,
			htable_destroy_fn destroy_val);
void htable_destroy(htable_t *ht);

size_t htable_min_cap(htable_t *ht);
int htable_set_min_cap(htable_t *ht, size_t new_min_cap);
size_t htable_cap(htable_t *ht);
size_t htable_len(htable_t *ht);

int htable_clear(htable_t *ht);
int htable_contains(htable_t *ht, void *key);
void *htable_get(htable_t *ht, void *key);
int htable_remove(htable_t *ht, void *key);
int htable_set(htable_t *ht, void *key, void *value);

#endif /* HTABLE_H */
