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

#define UTILS_LEN(arr) (sizeof(arr) / sizeof(*(arr)))

static inline void *
utils_hopeful_search(void *elems, ptrdiff_t elem_size, ptrdiff_t elem_count, ptrdiff_t id_offset, unsigned id)
{
    unsigned char *elements = elems;
    unsigned char *ptr;

    if (id > 0 && id < (elem_count + 1)) {
        ptr = elements + elem_size * (id - 1);

        if (*(unsigned *)(ptr + id_offset) == id)
            return ptr;
    }

    for (ptrdiff_t i = 0; i < elem_count; ++i) {
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

#define UTILS_STRETCHY_HOSE(dck, id_field_name, id) \
    utils_hopeful_search((dck).data, sizeof((dck).data[0]), (dck).count, (unsigned char *)(&((dck).data)->id_field_name) - (unsigned char *)((dck).data), id)

static inline unsigned char *
utils_read_file(const char *path)
{
    FILE *file = NULL;
    unsigned char *contents = NULL;

    file = fopen(path, "rb");
    if (!file)
        goto error_exit;

    if (fseek(file, 0, SEEK_END))
        goto error_exit;

    long long size = ftell(file);
    if (size == -1L)
        goto error_exit;

    rewind(file);

    contents = malloc(size + 1);

    if (!contents)
        goto error_exit;

    if (fread(contents, 1, size, file) != (size_t)size)
        goto error_exit;

    contents[size] = 0;

    fclose(file);

    return contents;

error_exit:
    if (file) {
        fclose(file);
    }

    free(contents);

    return NULL;
}

typedef struct
{
    char *data;
    UTILS_STRETCHY_T (char *, unsigned) lines;
} utils_file_lines_t;

static inline utils_file_lines_t
utils_file_lines_create(const char *file_path)
{
    utils_file_lines_t res = {
        .data = (char *)utils_read_file(file_path),
    };

    if (!res.data)
        return res;

    UTILS_STRETCHY_PUSH(res.lines, res.data);

    for (char *pos = res.data; *pos; ++pos) {
        if (*pos == '\n') {
            UTILS_STRETCHY_PUSH(res.lines, pos + 1);
            *pos = 0;
        }
    }

    return res;
}

static inline const char *
utils_file_lines_get(utils_file_lines_t *file_lines, unsigned line)
{
    if (line >= file_lines->lines.count)
        return "<nonexistent line>";

    return (const char *)(file_lines->lines.data[line]);
}

static inline void
utils_file_lines_free(utils_file_lines_t *file_lines)
{
    if (file_lines->data) {
        free(file_lines->data);
    }
    if (file_lines->lines.data) {
        free(file_lines->lines.data);
    }

    file_lines->data = NULL;

    file_lines->lines.data     = NULL;
    file_lines->lines.count    = 0;
    file_lines->lines.capacity = 0;
}

static inline bool
utils_str_eq(const char *str_1, const char *str_2)
{
    if (str_1 == str_2)
        return true;

    while (*str_1 && *str_2 && *str_1 == *str_2) {
        ++str_1;
        ++str_2;
    }

    return *str_1 == *str_2;
}

#endif // UTILS_H_
