#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "common/portability.h"
#include "common/vec.h"

WEAK Vec(void) _vec_new(size_t stride, size_t initial_cap) {
    if (initial_cap < 2) {
        initial_cap = 2;
    }
    Vec(void) vec = (char*)malloc(sizeof(VecHeader) + stride * initial_cap) + sizeof(VecHeader);
    vec_cap(vec) = initial_cap;
    vec_len(vec) = 0;

    return vec;
}

WEAK void _vec_reserve(Vec(void)* v, size_t stride, size_t slots) {
    if (slots + vec_len(*v) > vec_cap(*v)) {
        vec_cap(*v) += slots + (vec_cap(*v) / 2);
        *v = vec_elems_from_header(realloc(vec_header(*v), sizeof(VecHeader) + vec_cap(*v) * stride));
    }
}

WEAK void _vec_reserve1(Vec(void)* v, size_t stride) {
    if (vec_len(*v) + 1 > vec_cap(*v)) {
        vec_cap(*v) += vec_cap(*v) / 2;
        *v = vec_elems_from_header(realloc(vec_header(*v), sizeof(VecHeader) + vec_cap(*v) * stride));
    }
}

WEAK void _vec_shrink(Vec(void)* v, size_t stride) {
    if (vec_cap(*v) != vec_len(*v)) {
        vec_cap(*v) = vec_len(*v);
        *v = vec_elems_from_header(realloc(vec_header(*v), sizeof(VecHeader) + vec_cap(*v) * stride));
    }
}

WEAK void _vec_destroy(Vec(void)* v) {
    free(vec_header(*v));
    *v = nullptr;
}

/* string specifics */

/**
 * @brief Copies string to newly allocated owned vector.
 *
 * @param str source slice to copy
 * @returns vec char vector containing str
 *              (of capacity equal to the input's length).
 *
 * @warning The result is not null-terminated.
 */
Vec(char) string_to_owned(string str) {
    Vec(char) v = vec_new(char, str.len);

    memcpy(v, str.raw, str.len);
    return v;
}

/* @brief Takes ownership of string and creates a char vector.
 *
 * Allocates a new space for a header at the start of the string's raw pointer,
 * then moves the string in-place, and returning the new vec.
 *
 * @param str dynamically allocated null-terminated string.
 *
 * @warning The result is not null-terminated.
 *
 * @warning To allocate space, the function uses realloc, so the underlying
 * data must have been allocated by an *alloc family function (i.e malloc,
 * calloc, alligned_alloc etc).
 **/
Vec(char) cstring_to_vec(const char* str) {
    // The assumption is that you make a vec to append to it, so we reserve
    // more up-front. The growth factor of 2 may be changed but idk
    usize len = strlen(str);
    str = realloc((u8*)str, len * 2 + sizeof(VecHeader));

    // memmove so that strings can overlap.
    memmove((u8*)str, (u8*)str + sizeof(VecHeader), len);

    Vec(char) v = vec_elems_from_header(str);

    if (v == nullptr)
        return nullptr;

    vec_cap(v) = len * 2;
    vec_len(v) = len;

    return v;
}

/**
 * @brief Takes ownership of string and creates a char vector.
 *
 * Allocates a new space for a header at the start of the string's raw pointer,
 * then moves the string in-place, and returning the new vec.
 *
 * @param str dynamically allocated string, possibly null-terminated.
 * @return vec char vector containing str
 *         (with a capacity of 1.5x the input's length).
 *
 * @warning The result is not null-terminated.
 *
 * @warning To allocate space, the function uses realloc, so the underlying
 * data must have been allocated by an *alloc family function (i.e malloc,
 * calloc, alligned_alloc etc).
 */
Vec(char) string_to_vec(string str) {
    // The assumption is that you make a vec to append to it, so we reserve
    // more up-front.
    str.raw = realloc(str.raw, str.len * 1.5 + sizeof(VecHeader));

    // memmove so that strings can overlap.
    memmove(str.raw, (u8*)str.raw + sizeof(VecHeader), str.len);

    Vec(char) v = vec_elems_from_header(str.raw);

    if (v == nullptr)
        return nullptr;

    vec_cap(v) += str.len / 2;
    vec_len(v) = str.len;

    return v;
}

/**
 * @brief Format print to the end of a char vec
 *
 * Appends formatted string to a Vec(char), reallocating to make space.
 *
 * @param str string vector, possibly null-terminated
 * @param format printf format specifier
 * @param ... printf format arguments
 *
 * @warning The result is not null-terminated, nor does the function write
 *          over any old null terminators.
 */
void vec_appendf(Vec(char) str, const char* format, ...) {
    va_list a;
    va_start(a, format);
    va_list b;
    va_copy(b, a);

    usize printlen = 1 + vsnprintf("", 0, format, a);

    vec_reserve(&str, printlen);

    vsnprintf(&str[vec_len(str)], printlen, format, b);

    va_end(a);
    va_end(b);
}
