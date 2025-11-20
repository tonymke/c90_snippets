#ifndef MACROS_H
#define MACROS_H

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define STR_IMPL(s) #s
#define STR(s) STR_IMPL(s)

#endif /* MACROS_H */
