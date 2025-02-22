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

#ifndef HC_ARRAY_H
#define HC_ARRAY_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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
    HC_ARRAY_ERROR_OUT_OF_BOUNDS    = -2,
    HC_ARRAY_ERROR_OUT_OF_MEMORY    = -1,
    HC_ARRAY_SUCCESS                = 0,
    HC_ARRAY_EMPTY                  = 1
};

typedef struct hc_array_t {
    void *data;             // Pointer to array elements
    size_t count;           // Number of elements currently in the array
    size_t capacity;        // Total array capacity (allocated space)
    size_t elem_size;       // Size of an element (in bytes)
} hc_array_t;

/* Function declarations */

hc_array_t hc_array_create(size_t capacity, size_t elem_size);
void hc_array_destroy(hc_array_t* vec);
hc_array_t hc_array_copy(const hc_array_t* src);
bool hc_array_is_valid(const hc_array_t* vec);
bool hc_array_is_empty(const hc_array_t* vec);
int hc_array_reserve(hc_array_t* vec, size_t new_capacity);
int hc_array_shrink_to_fit(hc_array_t* vec);
void hc_array_clear(hc_array_t* vec);
void hc_array_fill(hc_array_t* vec, const void* data);
int hc_array_insert(hc_array_t* vec, size_t index, const void* elements, size_t count);
const void* hc_array_end(const hc_array_t* vec);
void* hc_array_back(hc_array_t* vec);
void* hc_array_front(hc_array_t* vec);
void* hc_array_at(hc_array_t* vec, size_t index);
int hc_array_push_back(hc_array_t* vec, const void *element);
int hc_array_push_front(hc_array_t* vec, const void *element);
int hc_array_push_at(hc_array_t* vec, size_t index, const void* element);
int hc_array_pop_back(hc_array_t* vec, void* element);
int hc_array_pop_front(hc_array_t* vec, void* element);
int hc_array_pop_at(hc_array_t* vec, size_t index, void* element);
bool hc_array_compare(const hc_array_t* a, const hc_array_t* b);

#endif // HC_ARRAY_H

#ifdef HC_ARRAY_IMPL

hc_array_t hc_array_create(size_t capacity, size_t elem_size)
{
    hc_array_t vec = { 0 };

    if (capacity == 0 || elem_size == 0) {
        return vec;
    }

    void *data = HC_MALLOC(capacity * elem_size);
    if (!data) return vec;

    vec.data = data;
    vec.capacity = capacity;
    vec.elem_size = elem_size;

    return vec;
}

void hc_array_destroy(hc_array_t* vec)
{
    if (vec->data) {
        HC_FREE(vec->data);
        vec->data = NULL;
    }
    vec->count = 0;
    vec->capacity = 0;
    vec->elem_size = 0;
}

hc_array_t hc_array_copy(const hc_array_t* src)
{
    hc_array_t vec = { 0 };

    size_t size_in_bytes = src->count * src->elem_size;
    if (size_in_bytes == 0) return vec;

    vec.data = HC_MALLOC(size_in_bytes);
    if (vec.data == NULL) return vec;

    memcpy(vec.data, src->data, size_in_bytes);

    vec.count = src->count;
    vec.capacity = src->count;
    vec.elem_size = src->elem_size;

    return vec;
}

bool hc_array_is_valid(const hc_array_t* vec)
{
    return vec->data != NULL
        && vec->capacity > 0
        && vec->elem_size > 0;
}

bool hc_array_is_empty(const hc_array_t* vec)
{
    return vec->count == 0;
}

int hc_array_reserve(hc_array_t* vec, size_t new_capacity)
{
    if (vec->capacity >= new_capacity) {
        return HC_ARRAY_SUCCESS;
    }

    void *new_data = HC_REALLOC(vec->data, new_capacity * vec->elem_size);
    if (!new_data) return HC_ARRAY_ERROR_OUT_OF_MEMORY;

    vec->data = new_data;
    vec->capacity = new_capacity;

    return HC_ARRAY_SUCCESS;
}

int hc_array_shrink_to_fit(hc_array_t* vec)
{
    if (vec->count == vec->capacity) {
        return 1;
    }

    if (vec->count == 0) {
        return HC_ARRAY_EMPTY;
    }

    void *new_data = HC_REALLOC(vec->data, vec->count * vec->elem_size);
    if (!new_data) return HC_ARRAY_ERROR_OUT_OF_MEMORY;

    vec->data = new_data;
    vec->capacity = vec->count;

    return HC_ARRAY_SUCCESS;
}

void hc_array_clear(hc_array_t* vec)
{
    vec->count = 0;
}

void hc_array_fill(hc_array_t* vec, const void* data)
{
    const void *end = (char*)vec->data + vec->capacity * vec->elem_size;
    for (char *ptr = (char*)vec->data; (void*)ptr < end; ptr += vec->elem_size) {
        memcpy(ptr, data, vec->elem_size);
    }
    vec->count = vec->capacity;
}

int hc_array_insert(hc_array_t* vec, size_t index, const void* elements, size_t count)
{
    if (index > vec->count) {
        return HC_ARRAY_ERROR_OUT_OF_BOUNDS;
    }

    size_t new_size = vec->count + count;

    if (new_size > vec->capacity) {
        // Here we increase the capacity of the
        // array to the nearest power of two
        if ((new_size & (new_size - 1)) == 0) {
            new_size <<= 1; // *= 2
        } else {
            new_size--;
            new_size |= new_size >> 1;
            new_size |= new_size >> 2;
            new_size |= new_size >> 4;
            new_size |= new_size >> 8;
            new_size |= new_size >> 16;
            #if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
                new_size |= new_size >> 32;
            #endif
            new_size++;
        }
        int ret = hc_array_reserve(vec, new_size);
        if (ret < 0) return ret;
    }

    // Moving items to make room for new items
    void *destination = (char*)vec->data + (index + count) * vec->elem_size;
    void *source = (char*)vec->data + index * vec->elem_size;
    size_t bytes_to_move = (vec->count - index) * vec->elem_size;
    memmove(destination, source, bytes_to_move);

    // Inserting new elements
    void *target = (char*)vec->data + index * vec->elem_size;
    memcpy(target, elements, count * vec->elem_size);

    // Updating array count
    vec->count = new_size;

    return HC_ARRAY_SUCCESS;
}

const void* hc_array_end(const hc_array_t* vec)
{
    return (const char*)vec->data + vec->count * vec->elem_size;
}

void* hc_array_back(hc_array_t* vec)
{
    return (char*)vec->data + (vec->count - 1) * vec->elem_size;
}

void* hc_array_front(hc_array_t* vec)
{
    return vec->data;
}

void* hc_array_at(hc_array_t* vec, size_t index)
{
    if (index >= vec->count) return NULL;
    return (char*)vec->data + index * vec->elem_size;
}

int hc_array_push_back(hc_array_t* vec, const void *element)
{
    if (vec->count >= vec->capacity) {
        // Here we increase the capacity of the
        // array to the nearest power of two
        size_t new_size = vec->count + 1;
        if ((new_size & (new_size - 1)) == 0) {
            new_size <<= 1; // *= 2
        } else {
            new_size--;
            new_size |= new_size >> 1;
            new_size |= new_size >> 2;
            new_size |= new_size >> 4;
            new_size |= new_size >> 8;
            new_size |= new_size >> 16;
            #if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
                new_size |= new_size >> 32;
            #endif
            new_size++;
        }
        int ret = hc_array_reserve(vec, new_size);
        if (ret < 0) return ret;
    }

    void *target = (char*)vec->data + vec->count * vec->elem_size;
    if (element) memcpy(target, element, vec->elem_size);
    else memset(target, 0, vec->elem_size);
    vec->count++;

    return HC_ARRAY_SUCCESS;
}

int hc_array_push_front(hc_array_t* vec, const void *element)
{
    if (vec->count >= vec->capacity) {
        // Here we increase the capacity of the
        // array to the nearest power of two
        size_t new_size = vec->count + 1;
        if ((new_size & (new_size - 1)) == 0) {
            new_size <<= 1; // *= 2
        } else {
            new_size--;
            new_size |= new_size >> 1;
            new_size |= new_size >> 2;
            new_size |= new_size >> 4;
            new_size |= new_size >> 8;
            new_size |= new_size >> 16;
            #if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
                new_size |= new_size >> 32;
            #endif
            new_size++;
        }
        int ret = hc_array_reserve(vec, new_size);
        if (ret < 0) return ret;
    }

    // Move all existing items to the right to make room
    void *destination = (char*)vec->data + vec->elem_size;
    void *source = vec->data;
    size_t bytes_to_move = vec->count * vec->elem_size;
    memmove(destination, source, bytes_to_move);

    // Copy new item to start or fill with zeroes
    if (element) memcpy(vec->data, element, vec->elem_size);
    else memset(vec->data, 0, vec->elem_size);

    // Increment count
    vec->count++;

    return HC_ARRAY_SUCCESS;
}

int hc_array_push_at(hc_array_t* vec, size_t index, const void* element)
{
    if (index >= vec->count) {
        return HC_ARRAY_ERROR_OUT_OF_BOUNDS;
    }

    if (vec->count >= vec->capacity) {
        // Here we increase the capacity of the
        // array to the nearest power of two
        size_t new_size = vec->count + 1;
        if ((new_size & (new_size - 1)) == 0) {
            new_size <<= 1; // *= 2
        } else {
            new_size--;
            new_size |= new_size >> 1;
            new_size |= new_size >> 2;
            new_size |= new_size >> 4;
            new_size |= new_size >> 8;
            new_size |= new_size >> 16;
            #if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
                new_size |= new_size >> 32;
            #endif
            new_size++;
        }
        int ret = hc_array_reserve(vec, new_size);
        if (ret < 0) return ret;
    }

    // Move existing items from index to make room
    void *destination = (char*)vec->data + index * vec->elem_size;
    void *source = (char*)vec->data + index * vec->elem_size;
    size_t bytes_to_move = (vec->count - index) * vec->elem_size;
    memmove(destination, source, bytes_to_move);

    // Copy new item to destination or fill with zeroes
    if (element) memcpy(source, element, vec->elem_size);
    else memset(source, 0, vec->elem_size);

    // Increment count
    vec->count++;

    return HC_ARRAY_SUCCESS;
}

int hc_array_pop_back(hc_array_t* vec, void* element)
{
    if (vec->count == 0) {
        return HC_ARRAY_EMPTY;
    }

    vec->count--;
    if (element != NULL) {
        void *source = (char*)vec->data + vec->count * vec->elem_size;
        memcpy(element, source, vec->elem_size);
    }

    return HC_ARRAY_SUCCESS;
}

int hc_array_pop_front(hc_array_t* vec, void* element)
{
    if (vec->count == 0) {
        return HC_ARRAY_EMPTY;
    }

    if (element != NULL) {
        memcpy(element, vec->data, vec->elem_size);
    }

    // Move all remaining items to the left
    void *source = (char*)vec->data + vec->elem_size;
    void *destination = vec->data;
    size_t bytes_to_move = (vec->count - 1) * vec->elem_size;
    memmove(destination, source, bytes_to_move);

    // Reduce array count
    vec->count--;

    return HC_ARRAY_SUCCESS;
}

int hc_array_pop_at(hc_array_t* vec, size_t index, void* element)
{
    if (index >= vec->count) {
        return HC_ARRAY_ERROR_OUT_OF_BOUNDS;
    }

    if (element != NULL) {
        void *source = (char*)vec->data + index * vec->elem_size;
        memcpy(element, source, vec->elem_size);
    }

    // Move the remaining items to the left to fill the hole
    void *destination = (char*)vec->data + index * vec->elem_size;
    void *source_start = (char*)vec->data + (index + 1) * vec->elem_size;
    size_t bytes_to_move = (vec->count - index - 1) * vec->elem_size;

    memmove(destination, source_start, bytes_to_move);

    // Reduce array count
    vec->count--;

    return HC_ARRAY_SUCCESS;
}

bool hc_array_compare(const hc_array_t* a, const hc_array_t* b)
{
    if (a->count != b->count || a->elem_size != b->elem_size) {
        return false;
    }

    return !memcmp(a->data, b->data, a->count * a->elem_size);
}

#endif // HC_ARRAY_IMPL
