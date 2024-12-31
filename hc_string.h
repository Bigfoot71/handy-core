#ifndef HC_STRING_H
#define HC_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

/* Macros and defintions */

#ifndef HC_MALLOC
#   define HC_MALLOC(sz) malloc(sz)
#endif

#ifndef HC_REALLOC
#   define HC_REALLOC(ptr, new_sz) realloc(ptr, new_sz)
#endif

#ifndef HC_FREE
#   define HC_FREE(ptr) free(ptr)
#endif

/* Types definitions */

enum hc_retcode_array {
    HC_STRING_ERROR_OUT_OF_MEMORY   = -3,
    HC_STRING_ERROR_INVALID_DST     = -2,
    HC_STRING_ERROR_INVALID_SRC     = -1,
    HC_STRING_SUCCESS               = 0
};

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} hc_string_t;

/* Function declarations */

hc_string_t hc_create_string(const char* str);
hc_string_t hc_create_string_with_char(char c, size_t count);
void hc_destroy_string(hc_string_t* str);
hc_string_t hc_copy_string(const hc_string_t* src);
int hc_concat_string(hc_string_t *dst, const char* src);
int hc_concat_string_hc(hc_string_t *dst, const hc_string_t* src);
hc_string_t hc_format_string(const char* format, ...);
bool hc_ends_with_string(const hc_string_t* str, const char* suffix);
bool hc_starts_with_string(const hc_string_t* str, const char *prefix);
bool hc_is_empty_string(const hc_string_t* str);
int hc_substring_string(hc_string_t *str, size_t start, size_t length);
int hc_append_char_string(hc_string_t* str, char c);
bool hc_compare_string(const hc_string_t* a, const hc_string_t* b);

#endif // HC_STRING_H

#ifdef HC_STRING_IMPL

hc_string_t hc_create_string(const char* str)
{
    hc_string_t new_str = { 0 };
    if (str == NULL) return new_str;

    size_t length = strlen(str);
    new_str.data = HC_MALLOC(length + 1);
    if (new_str.data == NULL) return new_str;

    strncpy(new_str.data, str, length);
    new_str.data[length] = '\0';
    new_str.length = length;
    new_str.capacity = length + 1;

    return new_str;
}

hc_string_t hc_create_string_with_char(char c, size_t count)
{
    hc_string_t new_str = { 0 };
    if (count == 0) return new_str;

    new_str.data = HC_MALLOC(count + 1);
    if (!new_str.data) return new_str;

    memset(new_str.data, c, count);
    new_str.data[count] = '\0';
    new_str.length = count;
    new_str.capacity = count + 1;

    return new_str;
}

void hc_destroy_string(hc_string_t* str)
{
    if (str && str->data) {
        HC_FREE(str->data);
        str->data = NULL;
        str->length = 0;
        str->capacity = 0;
    }
}

hc_string_t hc_copy_string(const hc_string_t* src)
{
    if (!src || !src->data) {
        return hc_create_string(NULL);
    }

    return hc_create_string(src->data);
}

int hc_concat_string(hc_string_t *dst, const char* src)
{
    if (!dst || !dst->data) {
        return HC_STRING_ERROR_INVALID_DST;
    }

    if (!src) {
        return HC_STRING_ERROR_INVALID_SRC;
    }

    size_t src_length = strlen(src);
    size_t new_length = dst->length + src_length;

    if (new_length + 1 > dst->capacity) {
        size_t new_capacity = (new_length + 1) * 2;
        char *new_data = (char *)HC_REALLOC(dst->data, new_capacity);
        if (!new_data) return HC_STRING_ERROR_INVALID_DST;

        dst->data = new_data;
        dst->capacity = new_capacity;
    }

    strcat(dst->data, src);
    dst->length = new_length;

    return HC_STRING_SUCCESS;
}

int hc_concat_string_hc(hc_string_t *dst, const hc_string_t* src)
{
    if (!src) {
        return HC_STRING_ERROR_INVALID_SRC;
    }

    return hc_concat_string(dst, src->data);
}

hc_string_t hc_format_string(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    size_t size = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);

    char *buffer = (char *)HC_MALLOC(size);
    if (!buffer) return hc_create_string(NULL);

    va_start(args, format);
    vsnprintf(buffer, size, format, args);
    va_end(args);

    hc_string_t formatted_str = hc_create_string(buffer);
    HC_FREE(buffer);

    return formatted_str;
}

bool hc_ends_with_string(const hc_string_t* str, const char* suffix)
{
    if (!str || !str->data || !suffix) return false;

    size_t suffix_length = strlen(suffix);
    if (suffix_length > str->length) return false;

    return strcmp(str->data + str->length - suffix_length, suffix) == 0;
}

bool hc_starts_with_string(const hc_string_t* str, const char *prefix)
{
    if (!str || !str->data || !prefix) return false;

    size_t prefix_length = strlen(prefix);
    if (prefix_length > str->length) return false;

    return strncmp(str->data, prefix, prefix_length) == 0;
}

bool hc_is_empty_string(const hc_string_t* str)
{
    return !str || !str->data || str->length == 0;
}

int hc_substring_string(hc_string_t *str, size_t start, size_t length)
{
    if (!str || !str->data || start >= str->length) {
        return HC_STRING_ERROR_INVALID_DST;
    }

    size_t max_length = str->length - start;
    if (length > max_length) length = max_length;

    memmove(str->data, str->data + start, length);
    str->data[length] = '\0';
    str->length = length;

    return HC_STRING_SUCCESS;
}

int hc_append_char_string(hc_string_t* str, char c)
{
    if (!str || !str->data) {
        return HC_STRING_ERROR_INVALID_DST;
    }

    if (str->length + 1 >= str->capacity) {
        size_t new_capacity = str->capacity + 1;
        if ((new_capacity & (new_capacity - 1)) == 0) {
            new_capacity <<= 1; // *= 2
        } else {
            new_capacity--;
            new_capacity |= new_capacity >> 1;
            new_capacity |= new_capacity >> 2;
            new_capacity |= new_capacity >> 4;
            new_capacity |= new_capacity >> 8;
            new_capacity |= new_capacity >> 16;
            #if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
                new_capacity |= new_capacity >> 32;
            #endif
            new_capacity++;
        }
        char *new_data = (char*)HC_REALLOC(str->data, new_capacity);
        if (!new_data) return HC_STRING_ERROR_OUT_OF_MEMORY;

        str->data = new_data;
        str->capacity = new_capacity;
    }

    str->data[str->length++] = c;
    str->data[str->length] = '\0';

    return HC_STRING_SUCCESS;
}

bool hc_compare_string(const hc_string_t* a, const hc_string_t* b)
{
    if (a->length != b->length) return false;
    return !strncmp(a->data, b->data, a->length);
}

#endif // HC_STRING_IMPL
