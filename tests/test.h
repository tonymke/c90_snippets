#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define ASSERT(cond, msg) \
	do { \
		if (!(cond)) { \
			fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, msg); \
			return 1; \
		} \
	} while (0)

#define ASSERT_EQ(a, b, msg) \
	do { \
		if ((a) != (b)) { \
			fprintf(stderr, "FAIL: %s:%d: %s (expected %lu, got %lu)\n", \
				__FILE__, __LINE__, msg, (unsigned long)(b), (unsigned long)(a)); \
			return 1; \
		} \
	} while (0)

#endif /* TEST_H */
