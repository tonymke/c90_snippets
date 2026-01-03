#ifndef HTABLE_H
#define HTABLE_H

#include <stddef.h>

/* A generic open-addressing, linear-probing hash table.
 *
 * Not thread-safe.
*/
typedef struct htable_t htable_t;

/* Function type providing the htable a way to compare two pointers to key values
 * of the same type. Return values should follow strcmp semantics, though htable
 * only cares about equality.
 * - 0, if *a and *b are equal;
 * - a negative value if *a is less than *b;
 * - a positive value if *a is greater than *b.
 */
typedef int (*htable_cmp_fn)(const void *a, const void *b);

/* Function type providing the htable a way to hash pointers to keys of the
 * same type. */
typedef size_t (*htable_hash_fn)(const void *p);

/* Function type providing the htable a way to clean up a stored pointer, such as
 * when destroying the entire table. */
typedef void (*htable_destroy_fn)(void *p);

/* Function type for iterating over hash table entries.
 * Return 0 to continue iteration, or non-zero to stop early.
 * The non-zero value will be returned by htable_foreach.
 */
typedef int (*htable_foreach_fn)(void *key, void *val, void *user_data);

/* Create a new hash table.
 *
 * Parameters:
 * min_cap:     Minimum items the table should be able to hold without resizing.
 *              There is always a compile-time minimum (that is
 *              MAX(min_cap, compile_time_min_cap) is ultimately used.
 * hash_key:    Function for hashing the keys.
 * cmp_key:     Function for comparing two keys.
 * destroy_key: Optional function for cleaning up keys.
 * destroy_val: Optional function for cleaning up values.
 *
 * Return value:
 * A pointer to a valid htable, or NULL on allocation failure.
 *
 * Errors:
 * - If hash_key or cmp_key are NULL, behavior is undefined.
 */
htable_t *htable_create(size_t min_cap, htable_hash_fn hash_key,
			htable_cmp_fn cmp_key, htable_destroy_fn destroy_key,
			htable_destroy_fn destroy_val);

/* Destroy the hash table.
 *
 * If non-NULL destroy functions were provided, they are invoked on every
 * key and value currently stored in the table.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
 */
void htable_destroy(htable_t *ht);

/* Retrieve a value from the htable by key.
 *
 * Parameters:
 * ht:      A valid htable pointer.
 * key:     The key to look up.
 * out_val: Optional out-pointer to receive the value stored in the table.
 *
 * Return value:
 * 0 if the key was found.
 * Positive value if the key was not found.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
 * - If key is NULL, behavior is undefined.
 */
int htable_get(const htable_t *ht, const void *key, void **out_val);

/* Insert a new entry into the htable.
 *
 * Parameters:
 * ht:  A valid htable pointer.
 * key: A pointer to the key.
 * val: A pointer to the value.
 *
 * Return value:
 * 0 on success.
 * Positive value if the key already exists (no change made).
 * Negative value on allocation failure.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
 * - If key is NULL, behavior is undefined.
 */
int htable_insert(htable_t *ht, void *key, void *val);

/* Replace an entry in the htable.
 *
 * Only replaces an existing entry. If the key does not exist, returns an error.
 * The previously stored key and value are destroyed (if destroy functions were
 * provided) before storing the new key and value.
 *
 * Parameters:
 * ht:  A valid htable pointer.
 * key: A pointer to the key.
 * val: A pointer to the value.
 *
 * Return value:
 * 0 on success (replaced existing entry).
 * 1 if the key does not exist (no change made).
 * Negative value on allocation failure (e.g., during resize).
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
 * - If key is NULL, behavior is undefined.
 */
int htable_replace(htable_t *ht, void *key, void *val);

/* Remove an entry and take back ownership of the pointers.
 *
 * Neither destroy_key nor destroy_val are called.
 *
 * Parameters:
 * ht:      A valid htable pointer.
 * key:     The key to search for.
 * out_key: Optional out-pointer to receive the stored key.
 * out_val: Optional out-pointer to receive the stored value.
 *
 * Return value:
 * 0 if found and stolen.
 * 1 if not found.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
 * - If key is NULL, behavior is undefined.
 * - If out_key or out_val are NULL, the corresponding stored pointer is not
 * returned and the caller loses that reference
 */
int htable_steal(htable_t *ht, const void *key, void **out_key, void **out_val);

/* Remove an entry and destroy its contents.
 *
 * Invokes destroy functions if they were provided at creation.
 *
 * Return value:
 * 0 if the key was found and removed.
 * 1 if the entry was not found.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
 * - If key is NULL, behavior is undefined.
 */
int htable_remove(htable_t *ht, const void *key);

/* Return the number of element spaces presently allocated in the table.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
*/
size_t htable_cap(const htable_t *ht);

/* Return the number of elements currently stored in the table.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
*/
size_t htable_len(const htable_t *ht);

/* Empty the table. Invokes destroy functions on all entries.
 *
 * Return value:
 * 0 on success
 * Negative value on allocation failure.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
*/
int htable_clear(htable_t *ht);

/* Iterate over all entries in the hash table.
 *
 * Calls the provided function for each key-value pair currently stored.
 * Iteration order is undefined and may change between calls.
 *
 * Parameters:
 * ht:        A valid htable pointer.
 * fn:        Function to call for each entry. Receives key, value, and user_data.
 * user_data: Optional pointer passed through to each invocation of fn.
 *
 * Return value:
 * 0 if iteration completed normally.
 * Non-zero value returned by fn if iteration was short-circuited.
 *
 * Errors:
 * - If ht is NULL or has been passed to free or htable_destroy previously,
 * behavior is undefined.
 * - If fn is NULL, behavior is undefined.
 * - Modifying the hash table during iteration (insert, remove, clear, etc.)
 * results in undefined behavior.
 */
int htable_foreach(const htable_t *ht, htable_foreach_fn fn, void *user_data);

#endif /* HTABLE_H */
