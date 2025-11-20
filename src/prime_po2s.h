#ifndef PRIME_PO2S_H
#define PRIME_PO2S_H
/* Array of the closest prime numbers to every power of 2 between 2^0 and 2^64
 * that the system can address. Useful for hashtable/hashset bucketing - which
 * are best with prime modulos.
 */

#include <stddef.h> /* for size_t */

extern const unsigned long prime_po2s[];
extern const size_t prime_po2s_cap;

#endif /* PRIME_PO2S_H */
