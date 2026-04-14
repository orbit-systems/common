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

Vec(char) string_to_vec(string str) {
    Vec(char) v = vec_new(char, str.len);

    memcpy(v, str.raw, str.len);
    return v;
}

static inline Vec(char) cstring_to_vec(const char* str) {
    return string_to_vec(string_wrap(str));
}

Vec(char) realloc_string_to_vec(string str) {
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

static inline Vec(char) realloc_cstring_to_vec(char* str) {
    return realloc_string_to_vec(string_wrap(str));
}

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
