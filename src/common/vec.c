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

// Copies string to newly allocated owned vector. The result is null terminated.
Vec(char) string_to_owned(string str) {
	Vec(char) v = vec_new(char, str.len + 1);

	memcpy(v, str.raw, str.len);
	v[str.len] = '\0';
	return v;
}

// Takes ownership of cstring and creates a vector. Moves the string in-place
// to insert a leading header, and returning the new vec.
Vec(char) cstring_to_vec(char* str) {
	// The assumption is that you make a vec to append to it, so we reserve
	// more up-front. The growth factor of 2 may be changed but idk
	usize len = strlen(str);
	str = realloc(str, len * 2 + sizeof(VecHeader));

	// memmove so that strings can overlap.
	memmove(str, (u8*)str + sizeof(VecHeader), len);

	Vec(char) v = vec_elems_from_header(str);

	if (v == nullptr)
		return nullptr;

	vec_cap(v) = len * 2;
	vec_len(v) = len;

	return v;
}

// Takes ownership of string and creates a vector. Moves the string in-place
// to insert a leading header, and returning the new vec.
Vec(char) string_to_vec(string str) {
	// The assumption is that you make a vec to append to it, so we reserve
	// more up-front. The growth factor of 2 may be changed but idk
	str.raw = realloc(str.raw, str.len * 2 + sizeof(VecHeader));

	// memmove so that strings can overlap.
	memmove(str.raw, (u8*)str.raw + sizeof(VecHeader), str.len);

	Vec(char) v = vec_elems_from_header(str.raw);

	if (v == nullptr)
		return nullptr;

	vec_cap(v) = str.len * 2;
	vec_len(v) = str.len;

	return v;
}

// Appends formatted string to a Vec(char), leaving the result null-terminated. 
// reallocating to make space for the new format. Begins printing at the 
// source's trailing null character, or its length if none is present.
void vec_appendf(Vec(char) str, const char* format, ...) {
    va_list a;
    va_start(a, format);
    va_list b;
    va_copy(b, a);

    usize printlen = 1 + vsnprintf("", 0, format, a);

    // if already null-terminated, go back and print over it
    if (str[vec_len(str)] == '\0')
	vec_len(str)--;

    vec_reserve(&str, printlen);

    vsnprintf(&str[vec_len(str)], printlen, format, b);
    str[printlen] = '\0';

    va_end(a);
    va_end(b);
}
