#ifndef TEST_H
#define TEST_H

#include <stdio.h>

/*
 * Test Assertion Macros
 *
 * All test functions utilizing these macros MUST:
 * 1. Define an 'int result = 0;' variable.
 * 2. Define a 'cleanup:' label for resource cleanup.
 * 3. Return 'result'.
 */

#define ASSERT(cond, msg) \
	do { \
		if (!(cond)) { \
			fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, (msg)); \
			result = 1; \
			goto cleanup; \
		} \
	} while (0)

#define ASSERT_EQ(a, b, msg) \
	do { \
		if ((a) != (b)) { \
			fprintf(stderr, "FAIL: %s:%d: %s (expected %lu, got %lu)\n", \
				__FILE__, __LINE__, (msg), (unsigned long)(b), \
				(unsigned long)(a)); \
			result = 1; \
			goto cleanup; \
		} \
	} while (0)

#endif /* TEST_H */
