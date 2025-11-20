#include <stddef.h>

#include "hash.h"

size_t hash_int_rjenkins_nomult(unsigned long key)
{
	size_t hash = key;

	hash = (hash + 0x7ed55d16UL) + (hash << 12);
	hash = (hash ^ 0xc761c23cUL) ^ (hash >> 19);
	hash = (hash + 0x165667b1UL) + (hash << 5);
	hash = (hash + 0xd3a2646cUL) ^ (hash << 9);
	hash = (hash + 0xfd7046c5UL) + (hash << 3);
	hash = (hash ^ 0xb55a4f09UL) ^ (hash >> 16);

	return hash;
}

size_t hash_int_knuth(unsigned long key)
{
	size_t hash;

	hash = (size_t)key * 2654435761UL;
	hash ^= hash >> 16;

	return hash;
}

size_t hash_int_multiandxor(unsigned long key)
{
	size_t hash;

	hash = (size_t)key;
	hash ^= (hash >> 32);
	hash *= 0x85ebca6bUL;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35UL;
	hash ^= (hash >> 16);

	return (size_t)hash;
}

size_t hash_cstring_djb2(const char *s)
{
	size_t hash;
	unsigned c;

	hash = 5381;
	while ((c = (unsigned)*s++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}

size_t hash_cstring_fnv_1a(const char *s)
{
	size_t hash;
	unsigned char c;

	hash = 2166136261UL;
	while ((c = (unsigned char)*s++)) {
		hash ^= c;
		hash *= 16777619UL;
	}
	return hash;
}
