#ifndef HC_ARRAY_H
#define HC_ARRAY_H

#ifndef HC_MALLOC
#   define HC_MALLOC(sz) malloc(sz)
#endif

#ifndef HC_REALLOC
#   define HC_REALLOC(ptr, new_sz) realloc(ptr, new_sz)
#endif

#ifndef HC_FREE
#   define HC_FREE(ptr) free(ptr)
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum hc_retcode_array {
    HC_ARRAY_ERROR_OUT_OF_BOUNDS    = -2,
    HC_ARRAY_ERROR_OUT_OF_MEMORY    = -1,
    HC_ARRAY_SUCCESS                = 0,
    HC_ARRAY_EMPTY                  = 1
};

typedef struct hc_array_t {
    void *data;         // Pointer to vector elements
    size_t count;        // Number of elements currently in the vector
    size_t capacity;    // Total vector capacity (allocated space)
    size_t elem_size;   // Size of an element (in bytes)
} hc_array_t;

hc_array_t hc_create_array(size_t capacity, size_t elem_size);
void hc_destroy_vector(hc_array_t* vec);
hc_array_t hc_copy_array(const hc_array_t* src);
bool hc_is_valid_array(const hc_array_t* vec);
bool hc_is_empty_array(const hc_array_t* vec);
int hc_reserve_array(hc_array_t* vec, size_t new_capacity);
int hc_shrink_to_fit_array(hc_array_t* vec);
void hc_clear_array(hc_array_t* vec);
void hc_fill_array(hc_array_t* vec, const void* data);
int hc_insert_array(hc_array_t* vec, size_t index, const void* elements, size_t count);
const void* hc_end_array(const hc_array_t* vec);
void* hc_back_array(hc_array_t* vec);
void* hc_front_array(hc_array_t* vec);
void* hc_at_array(hc_array_t* vec, size_t index);
int hc_push_back_array(hc_array_t* vec, const void *element);
int hc_push_front_array(hc_array_t* vec, const void *element);
int hc_push_at_array(hc_array_t* vec, size_t index, const void* element);
int hc_pop_back_array(hc_array_t* vec, void* element);
int hc_pop_front_array(hc_array_t* vec, void* element);
int hc_pop_at_array(hc_array_t* vec, size_t index, void* element);
bool hc_cmp_array(const hc_array_t* a, const hc_array_t* b);

#endif // HC_ARRAY_H

#ifdef HC_ARRAY_IMPL

hc_array_t hc_create_array(size_t capacity, size_t elem_size)
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

void hc_destroy_vector(hc_array_t* vec)
{
    if (vec->data) {
        HC_FREE(vec->data);
        vec->data = NULL;
    }
    vec->count = 0;
    vec->capacity = 0;
    vec->elem_size = 0;
}

hc_array_t hc_copy_array(const hc_array_t* src)
{
    hc_array_t vec = { 0 };

    size_t size_in_bytes = src->count * src->elem_size;
    if (size_in_bytes == 0) return vec;

    void* data = HC_MALLOC(size_in_bytes);
    if (!data) return vec;

    memcpy(vec.data, src->data, size_in_bytes);

    vec.count = src->count;
    vec.capacity = src->count;
    vec.elem_size = src->elem_size;

    return vec;
}

bool hc_is_valid_array(const hc_array_t* vec)
{
    return vec->data != NULL
        && vec->capacity > 0
        && vec->elem_size > 0;
}

bool hc_is_empty_array(const hc_array_t* vec)
{
    return vec->count == 0;
}

int hc_reserve_array(hc_array_t* vec, size_t new_capacity)
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

int hc_shrink_to_fit_array(hc_array_t* vec)
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

void hc_clear_array(hc_array_t* vec)
{
    vec->count = 0;
}

void hc_fill_array(hc_array_t* vec, const void* data)
{
    const void *end = (char*)vec->data + vec->capacity * vec->elem_size;
    for (char *ptr = (char*)vec->data; (void*)ptr < end; ptr += vec->elem_size) {
        memcpy(ptr, data, vec->elem_size);
    }
    vec->count = vec->capacity;
}

int hc_insert_array(hc_array_t* vec, size_t index, const void* elements, size_t count)
{
    if (index > vec->count) {
        return HC_ARRAY_ERROR_OUT_OF_BOUNDS;
    }

    size_t new_size = vec->count + count;

    if (new_size > vec->capacity) {
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
        int ret = hc_reserve_array(vec, new_size);
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

    // Updating vector count
    vec->count = new_size;

    return HC_ARRAY_SUCCESS;
}

const void* hc_end_array(const hc_array_t* vec)
{
    return (const char*)vec->data + vec->count * vec->elem_size;
}

void* hc_back_array(hc_array_t* vec)
{
    return (char*)vec->data + (vec->count - 1) * vec->elem_size;
}

void* hc_front_array(hc_array_t* vec)
{
    return vec->data;
}

void* hc_at_array(hc_array_t* vec, size_t index)
{
    if (index >= vec->count) return NULL;
    return (char*)vec->data + index * vec->elem_size;
}

int hc_push_back_array(hc_array_t* vec, const void *element)
{
    if (vec->count >= vec->capacity) {
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
        int ret = hc_reserve_array(vec, new_size);
        if (ret < 0) return ret;
    }

    void *target = (char*)vec->data + vec->count * vec->elem_size;
    memcpy(target, element, vec->elem_size);
    vec->count++;

    return HC_ARRAY_SUCCESS;
}

int hc_push_front_array(hc_array_t* vec, const void *element)
{
    if (vec->count >= vec->capacity) {
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
        int ret = hc_reserve_array(vec, new_size);
        if (ret < 0) return ret;
    }

    // Move all existing items to the right to make room
    void *destination = (char*)vec->data + vec->elem_size;
    void *source = vec->data;
    size_t bytes_to_move = vec->count * vec->elem_size;
    memmove(destination, source, bytes_to_move);

    // Copy new item to start
    memcpy(vec->data, element, vec->elem_size);

    // Increment count
    vec->count++;

    return HC_ARRAY_SUCCESS;
}

int hc_push_at_array(hc_array_t* vec, size_t index, const void* element)
{
    if (index >= vec->count) {
        return HC_ARRAY_ERROR_OUT_OF_BOUNDS;
    }

    if (vec->count >= vec->capacity) {
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
        int ret = hc_reserve_array(vec, new_size);
        if (ret < 0) return ret;
    }

    // Move existing items from index to make room
    void *destination = (char*)vec->data + index * vec->elem_size;
    void *source = (char*)vec->data + index * vec->elem_size;
    size_t bytes_to_move = (vec->count - index) * vec->elem_size;
    memmove(destination, source, bytes_to_move);

    // Copy the new element to the specified position
    if (element != NULL) {
        memcpy(source, element, vec->elem_size);
    }

    // Increment count
    vec->count++;

    return HC_ARRAY_SUCCESS;
}

int hc_pop_back_array(hc_array_t* vec, void* element)
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

int hc_pop_front_array(hc_array_t* vec, void* element)
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

    // Reduce vector count
    vec->count--;

    return HC_ARRAY_SUCCESS;
}

int hc_pop_at_array(hc_array_t* vec, size_t index, void* element)
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

    // Reduce vector count
    vec->count--;

    return HC_ARRAY_SUCCESS;
}

bool hc_cmp_array(const hc_array_t* a, const hc_array_t* b)
{
    if (a->count != b->count || a->elem_size != b->elem_size) {
        return false;
    }

    return !memcmp(a->data, b->data, a->count * a->elem_size);
}

#endif // HC_ARRAY_IMPL
