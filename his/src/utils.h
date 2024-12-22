#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>

#define COUNT_OF(x) (sizeof(x)/sizeof(*x))

#if defined(_DEBUG)
    #define UNREACHABLE()                                                           \
    do {                                                                            \
        fprintf(stderr, "%s:%d: Unreachable line reached!\n", __FILE__, __LINE__);  \
        exit(666);                                                                  \
    } while (0)
#else
    #if defined(COMPILER_MSVC)
        #define UNREACHABLE()   __assume(0)
    #elif defined(COMPILER_GNUC) || defined(COMPILER_CLANG)
        #define UNREACHABLE()   __builtin_unreachable()
    #endif
#endif

static inline void
utils_noop(void) { }

#if defined(_DEBUG)
    #define ASSERT(...) \
    ( \
        !(__VA_ARGS__) ? ( \
            fprintf(stderr, "%s:%d: Assert failed! (%s)\n", __FILE__, __LINE__, #__VA_ARGS__), \
            exit(1) \
        ) \
        : ( \
            utils_noop() \
        ) \
    )
#else
    #define ASSERT(...) (utils_noop())
#endif

#if defined(COMPILER_MSVC)
    #define FMT_U64 "%llu"
    #define FMT_I64 "%lld"
    #define FMT_X64 "%llx"
#else
    #define FMT_U64 "%lu"
    #define FMT_I64 "%ld"
    #define FMT_X64 "%lx"
#endif


typedef enum { false = 0, true = 1 } bool;


#define DCK_STRETCHY_T(data_type, size_type) struct { data_type *data; size_type count, capacity; }

#define DCK_STRETCHY_FOR(dck, type, elem) \
    for (type *elem = (dck).data; elem < (dck).data + (dck).count; ++elem)

#define DCK_STRETCHY_PUSH(dck, ...)                                                     \
do {                                                                                    \
    if ((dck).count == (dck).capacity) {                                                \
        if ((dck).capacity == 0) {                                                      \
            if (sizeof(*((dck).data)) > 4096) {                                         \
                (dck).capacity = sizeof(*((dck).data));                                 \
            }                                                                           \
            else {                                                                      \
                (dck).capacity = 4096 / sizeof(*((dck).data));                          \
            }                                                                           \
        }                                                                               \
        else {                                                                          \
            (dck).capacity *= 2;                                                        \
        }                                                                               \
        (dck).data = realloc((dck).data, sizeof(*((dck).data)) * (dck).capacity);       \
        if (!(dck).data) {                                                              \
            fprintf(stderr, "%s:%d: malloc failure! exiting...\n", __FILE__, __LINE__); \
            exit(666);                                                                  \
        }                                                                               \
    }                                                                                   \
    (dck).data[(dck).count] = __VA_ARGS__;                                              \
    (dck).count++;                                                                      \
} while (0)

#define DCK_STRETCHY_RESERVE(dck, amount)                                               \
do {                                                                                    \
    if ((dck).count + (amount) > (dck).capacity) {                                      \
        if ((dck).capacity == 0) {                                                      \
            if (sizeof(*((dck).data)) > 4096) {                                         \
                (dck).capacity = sizeof(*((dck).data));                                 \
            }                                                                           \
            else {                                                                      \
                (dck).capacity = 4096 / sizeof(*((dck).data));                          \
            }                                                                           \
        }                                                                               \
        while ((dck).count + (amount) > (dck).capacity) {                               \
            (dck).capacity *= 2;                                                        \
        }                                                                               \
        (dck).data = realloc((dck).data, sizeof(*((dck).data)) * (dck).capacity);       \
        if (!(dck).data) {                                                              \
            fprintf(stderr, "%s:%d: malloc failure! exiting...\n", __FILE__, __LINE__); \
            exit(666);                                                                  \
        }                                                                               \
    }                                                                                   \
} while (0)


#endif // UTILS_H_
