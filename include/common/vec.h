#ifndef VEC_H
#define VEC_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common/str.h"

///
/// +--------+--------+--------+--------+--------+--------+
/// | header | vec[0] | vec[1] | vec[2] |  ....  | vec[N] |
/// +--------+--------+--------+--------+--------+--------+
/// ^         ^ Vec(T) given to you
/// |
/// Start of allocated memory
/// 
/// Every `Vec(T)` has a `VecHeader` that sits behind it in memory.
/// This header is where the allocated memory really starts, and
/// `Vec(T)` is just a pointer to where the element array starts.

/// Information about a vector that sits behind the element array.
typedef struct VecHeader {
    /// The vector's length (currently used space).
    uint32_t len;
    /// The vector's allocated capacity.
    uint32_t cap;
} VecHeader;

/// Small utility to catch errors where someone passes
/// the vector by value instead of by reference
#define _vec_assert_ptr_ptr(pp) (((void)**(pp)), (pp))

/// Vector (array list) of type T.
#define Vec(T) T*
/// Width of the elements of a vector.
#define vec_stride(vec) sizeof(*(vec))
/// This vector's `VecHeader`.
#define vec_header(vec) ((VecHeader*)((char*)(vec) - sizeof(VecHeader)))
/// Get the pointer to a vec's elements from the header.
#define vec_elems_from_header(header) ((void*)((u8*)(header) + sizeof(VecHeader)))
/// This vector's length.
#define vec_len(vec) vec_header(vec)->len
/// This vector's capacity.
#define vec_cap(vec) vec_header(vec)->cap
/// Clear the vector (set it's length to zero).
#define vec_clear(vecptr) (vec_len(*_vec_assert_ptr_ptr(vecptr)) = 0)
/// Create a new vector, storing type `T` with an initial capacity `initial_cap`.
#define vec_new(T, initial_cap) ((Vec(T)) _vec_new(sizeof(T), initial_cap))
/// Ensure the vector `vecptr` can append `slots` items without allocating.
#define vec_reserve(vecptr, slots) _vec_reserve((void**)_vec_assert_ptr_ptr(vecptr), vec_stride(*vecptr), slots)
/// Append `item` to vector `vecptr`.
#define vec_append(vecptr, item) do { \
    _vec_reserve1((void**)_vec_assert_ptr_ptr(vecptr), vec_stride(*vecptr)); \
    (*vecptr)[vec_len(*vecptr)++] = (item); \
} while (0)
/// reallocate the vector `vecptr` such that its capapcity is equal to its length. 
#define vec_shrink(vecptr) _vec_shrink((void**)_vec_assert_ptr_ptr(vecptr), vec_stride(*vecptr))
/// Destroy and deallocate vector `vecptr`
#define vec_destroy(vecptr) _vec_destroy((void**)_vec_assert_ptr_ptr(vecptr))
/// Peek at the last element of a vector.
#define vec_peek(vec) (vec)[vec_len(vec)-1]
/// Remove the element at `index` and replace it with the last element in the vector `vecptr`.
#define vec_remove_unordered(vecptr, index) do {            \
    _vec_assert_ptr_ptr(vecptr);                            \
    if (index < vec_len(*vecptr)) {                         \
        (*vecptr)[index] = (*vecptr)[vec_len(*vecptr) - 1]; \
        vec_len(*vecptr) -= 1;                              \
    }                                                       \
} while (0)
/// Remove the element at `index` and copy the rest of the elements forward.
#define vec_remove_ordered(vecptr, index) do {  \
    _vec_assert_ptr_ptr(vecptr);                \
    usize _length = vec_len(*vecptr);           \
    if (index < _length - 1) {                  \
        memmove(                                \
            &(*vecptr)[index],                  \
            &(*vecptr)[index + 1],              \
            (_length - index - 1) * vec_stride(*vecptr) \
        );                                      \
    }                                           \
    if (index < _length) {                      \
        vec_len(*vecptr) -= 1;                  \
    }                                           \
} while (0)

Vec(void) _vec_new(size_t stride, size_t initial_cap);
void _vec_reserve(Vec(void)* v, size_t stride, size_t slots);
void _vec_reserve1(Vec(void)* v, size_t stride);
void _vec_shrink(Vec(void)* v, size_t stride);
void _vec_destroy(Vec(void)* v);

/* string specifics */

/// \brief Copies string to newly allocated owned vector.
///
/// \param str source slice to copy
/// \return vec char vector containing str
///              (of capacity equal to the input's length).
///
/// \warning The result is not null-terminated.
///
/// It may be preferable to realloc in-place to reduce fragmentation. For
/// dynamically allocated strings,
/// \see realloc_string_to_vec
Vec(char) string_to_vec(string str);

/// \brief Copies a C string to newly allocated owned vector.
///
/// \param str source C string to copy
/// \return vec char vector containing str
///              (of capacity equal to the input's length).
///
/// \warning The result is not null-terminated.
///
/// It may be preferable to create a vector in-place to reduce fragmentation.
/// For dynamically allocated C strings,
/// \see realloc_string_to_vec
static inline Vec(char) cstring_to_vec(const char* str);

/// \brief Takes ownership of string and creates a char vector.
///
/// Allocates a new space for a header at the start of the string's raw pointer,
/// then moves the string in-place, and returning the new vec.
///
/// \param str dynamically allocated string, possibly null-terminated.
/// \return vec char vector containing str
///         (with a capacity of 1.5x the input's length).
///
/// \warning The result is not null-terminated.
///
/// \warning To allocate space, the function uses realloc, so the underlying
/// data must have been allocated by an *alloc family function (i.e malloc,
/// calloc, alligned_alloc etc).
///
/// For a copy only function,
/// \see string_to_vec
Vec(char) realloc_string_to_vec(string str);

/// \brief Takes ownership of string and creates a char vector.
///
/// Allocates a new space for a header at the start of the str,
/// then moves the string in-place, and returning the new vec.
///
/// \param str dynamically allocated null-terminated string.
/// \return vec char vector containing str
///         (with a capacity of 1.5x the input's length).
///
/// \warning The result is not null-terminated.
///
/// \warning To allocate space, the function uses realloc, so the source str
/// must have been allocated by an *alloc family function (i.e malloc,
/// calloc, alligned_alloc etc).
///
/// For a copy only function,
/// \see cstring_to_vec
static inline Vec(char) realloc_cstring_to_vec(char* str);

/// \brief Format print to the end of a char vec
///
/// Appends formatted string to a Vec(char), reallocating to make space.
///
/// \param str string vector, possibly null-terminated
/// \param format printf format specifier
/// \param ... printf format arguments
///
/// \warning The result is not null-terminated, nor does the function write
///          over any old null terminators.
FORMAT_CHECK(2, 3) void vec_appendf(Vec(char) str, const char* format, ...);

#endif // VEC_H
