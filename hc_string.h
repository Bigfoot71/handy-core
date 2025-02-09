/**
 * MIT License
 * 
 * Copyright (c) 2024-2025 Le Juez Victor
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef HC_STRING_H
#define HC_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>

/* Macros and defintions */

#ifndef HC_MALLOC
#   define HC_MALLOC(sz) malloc(sz)
#endif

#ifndef HC_CALLOC
#   define HC_CALLOC(nb, sz) calloc(nb, sz)
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

hc_string_t hc_string_create(size_t capacity);
hc_string_t hc_string_create_from_cstr(const char* str);
hc_string_t hc_string_create_with_char(char c, size_t count);
void hc_string_destroy(hc_string_t* str);
hc_string_t hc_string_copy(const hc_string_t* src);
int hc_string_concat(hc_string_t *dst, const char* src);
int hc_string_concat_hc(hc_string_t *dst, const hc_string_t* src);
hc_string_t hc_string_format(const char* format, ...);
void hc_string_tolower(hc_string_t* str);
void hc_string_toupper(hc_string_t* str);
int hc_string_replace(hc_string_t* str, const char* old_word, const char* new_word);
int hc_string_trim(hc_string_t* str);
int hc_string_append_char(hc_string_t* str, char c);
int hc_string_substring(hc_string_t *str, size_t start, size_t length);
bool hc_string_ends_with(const hc_string_t* str, const char* suffix);
bool hc_string_starts_with(const hc_string_t* str, const char *prefix);
int hc_string_occurrences(const hc_string_t* str, const char* keyword);
int hc_string_word_count(const hc_string_t* str);
bool hc_string_is_empty(const hc_string_t* str);
bool hc_string_compare(const hc_string_t* a, const hc_string_t* b);

#endif // HC_STRING_H

#ifdef HC_STRING_IMPL

hc_string_t hc_string_create(size_t capacity)
{
    hc_string_t new_str = { 0 };
    new_str.data = HC_CALLOC(capacity, sizeof(char));
    new_str.capacity = capacity;
    new_str.length = 0;
    return new_str;
}

hc_string_t hc_string_create_from_cstr(const char* str)
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

hc_string_t hc_string_create_with_char(char c, size_t count)
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

void hc_string_destroy(hc_string_t* str)
{
    if (str && str->data) {
        HC_FREE(str->data);
        str->data = NULL;
        str->length = 0;
        str->capacity = 0;
    }
}

hc_string_t hc_string_copy(const hc_string_t* src)
{
    hc_string_t new_str = { 0 };
    if (!src || !src->data) return new_str;
    new_str = hc_string_create_from_cstr(src->data);
    return new_str;
}

int hc_string_concat(hc_string_t *dst, const char* src)
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

int hc_string_concat_hc(hc_string_t *dst, const hc_string_t* src)
{
    if (!src) {
        return HC_STRING_ERROR_INVALID_SRC;
    }

    return hc_string_concat(dst, src->data);
}

hc_string_t hc_string_format(const char* format, ...)
{
    hc_string_t formatted_str = { 0 };

    va_list args;
    va_start(args, format);

    size_t size = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);

    char *buffer = HC_MALLOC(size);
    if (!buffer) {
        return formatted_str;
    }

    va_start(args, format);
    vsnprintf(buffer, size, format, args);
    va_end(args);

    formatted_str = hc_string_create_from_cstr(buffer);
    HC_FREE(buffer);

    return formatted_str;
}

void hc_string_tolower(hc_string_t* str)
{
    char* ptr = str->data;
    while (*ptr) {
        *ptr = tolower((unsigned char)*ptr);
        ptr++;
    }
}

void hc_string_toupper(hc_string_t* str)
{
    char* ptr = str->data;
    while (*ptr) {
        *ptr = toupper((unsigned char)*ptr);
        ptr++;
    }
}

int hc_string_replace(hc_string_t* str, const char* old_word, const char* new_word) {
    if (!str || !str->data || str->length == 0) {
        return HC_STRING_ERROR_INVALID_DST;
    }

    if (!old_word || !new_word) {
        return HC_STRING_ERROR_INVALID_SRC;
    }

    int old_len = strlen(old_word);
    int new_len = strlen(new_word);

    // Comptage des occurrences de old_word
    int occurrences = 0;
    const char* ptr = str->data;
    while ((ptr = strstr(ptr, old_word)) != NULL) {
        ptr += old_len;
        occurrences++;
    }

    // Calcul de la nouvelle taille
    size_t result_len = str->length + occurrences * (new_len - old_len);

    // Création de la chaîne de résultat
    hc_string_t result = hc_string_create(result_len + 1);  // +1 pour le caractère nul
    if (result.data == NULL) {
        return HC_STRING_ERROR_OUT_OF_MEMORY;
    }

    char* dst = result.data;
    const char* src = str->data;

    // Remplissage de la chaîne résultat avec les remplacements
    while (*src) {
        if (strncmp(src, old_word, old_len) == 0) {
            strcpy(dst, new_word);
            dst += new_len;
            src += old_len;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';  // Terminer la chaîne

    // Mettre à jour la structure d'origine
    HC_FREE(str->data);
    *str = result;  // Remplacer les données de str par celles de result

    return HC_STRING_SUCCESS;
}

int hc_string_trim(hc_string_t* str)
{
    if (!str || !str->data || str->length == 0) {
        return HC_STRING_ERROR_INVALID_DST;
    }

    char* start = str->data;
    while (isspace((unsigned char)*start)) start++;

    char* end = str->data + str->length - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    size_t new_length = (size_t)(end - start + 1);
    memmove(str->data, start, new_length);
    str->data[new_length] = '\0';
    str->length = new_length;

    return HC_STRING_SUCCESS;
}

int hc_string_append_char(hc_string_t* str, char c)
{
    if (!str || !str->data) {
        return HC_STRING_ERROR_INVALID_DST;
    }

    if (str->length + 1 >= str->capacity) {
        // Here we increase the capacity of the
        // string to the nearest power of two
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

int hc_string_substring(hc_string_t *str, size_t start, size_t length)
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

bool hc_string_ends_with(const hc_string_t* str, const char* suffix)
{
    if (!str || !str->data || !suffix) return false;

    size_t suffix_length = strlen(suffix);
    if (suffix_length > str->length) return false;

    return strcmp(str->data + str->length - suffix_length, suffix) == 0;
}

bool hc_string_starts_with(const hc_string_t* str, const char *prefix)
{
    if (!str || !str->data || !prefix) return false;

    size_t prefix_length = strlen(prefix);
    if (prefix_length > str->length) return false;

    return strncmp(str->data, prefix, prefix_length) == 0;
}

int hc_string_occurrences(const hc_string_t* str, const char* keyword)
{
    int occurences = 0;
    const char* ptr = str->data;
    while ((ptr = strstr(ptr, keyword)) != NULL) {
        ptr += strlen(keyword);
        occurences++;
    }
    return occurences;
}

int hc_string_word_count(const hc_string_t* str)
{
    int count = 0;
    int in_word = 0;
    char* ptr = str->data;
    while (*ptr) {
        if (isspace((unsigned char)*ptr)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        ptr++;
    }
    return count;
}

bool hc_string_is_empty(const hc_string_t* str)
{
    return !str || !str->data || str->length == 0;
}

bool hc_string_compare(const hc_string_t* a, const hc_string_t* b)
{
    if (a->length != b->length) return false;
    return !strncmp(a->data, b->data, a->length);
}

#endif // HC_STRING_IMPL
