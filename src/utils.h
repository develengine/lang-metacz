#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static inline void
utils_noop(void) { }

#if defined(_DEBUG)
    #define UTILS_ASSERT(...) \
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
    #define UTILS_ASSERT(...) (utils_noop())
#endif

typedef enum { false = 0, true = 1 } bool;


static inline void *
utils_hopeful_search(void *elems, size_t elem_size, size_t elem_count, size_t id_offset, unsigned id)
{
    unsigned char *elements = elems;
    unsigned char *ptr;

    if (id > 0 && id < (elem_count + 1)) {
        ptr = elements + elem_size * (id - 1);

        if (*(unsigned *)(ptr + id_offset) == id)
            return ptr;
    }

    for (size_t i = 0; i < elem_count; ++i) {
        ptr = elements + elem_size * i;

        if (*(unsigned *)(ptr + id_offset) == id)
            return ptr;
    }

    return NULL;
}


#define UTILS_STRETCHY_T(data_type, size_type) struct { data_type *data; size_type count, capacity; }

#define UTILS_STRETCHY_FOR(dck, type, elem) \
    for (type *elem = (dck).data; elem < (dck).data + (dck).count; ++elem)

#define UTILS_STRETCHY_PUSH(dck, ...)                                                     \
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

#define UTILS_STRETCHY_RESERVE(dck, amount)                                               \
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

#define UTILS_STRETCHY_HOSE(stretchy, id_field_name, id) \
    utils_hopeful_search((stretchy).data, sizeof((stretchy).data[0]), (stretchy).count, (unsigned char *)(&((stretchy).data)->id_field_name) - (unsigned char *)((stretchy).data), id)

#endif // UTILS_H_
