#ifndef HASH_H
#define HASH_H

/* A collection of hashing functions for various cases. */

#include <stddef.h> /* for size_t */

/*
An integer hash method with good distribution consisting entirely of adds,
shifts, and xors.

Quite a few operations, but none of them are multiplication - making this a good
choice for platforms without hardware-assisted multiplication.

Taken from https://gist.github.com/badboy/6267743#robert-jenkins-32-bit-integer-hash-function
*/
size_t hash_int_rjenkins_nomult(unsigned long key);

/*
An integer hash method where we multiply the input by the golden ratio of 2^32
(2654435761) and do a bitshift+XORing.

Few operations, but one is multiplication - and only good distribution for 
values <~ 2^16.

A fair balance between speed and distribution for the small-values case.

Taken straight from The Art of Computer Programming.
*/
size_t hash_int_knuth(unsigned long key);

/*
A more robust hash for larger values, but not as fast - particularly on machines
without hardware assistance for multiplication (like old DOS PCs).

I don't recall where I originally found this.
*/
size_t hash_int_multiandxor(unsigned long key);

/* A fast, simple string hash. Ideal for when hashing speed is preferred over
 * collision resistance - as no multiplication is done.
 *
 * Famously the accepted answer on the "give me a function to hash a string in C"
 * question on stackoverflow.
 * 
 * Taken from there many years ago. https://stackoverflow.com/a/7666577/929861
 */
size_t hash_cstring_djb2(const char *s);

/*
 * A slower but much more collision resistant string hash of high repute.
 * 
 * Taken from https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
size_t hash_cstring_fnv_1a(const char *s);

#endif /* HASH_H */
