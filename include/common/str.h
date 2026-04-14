#ifndef ORBIT_STRING_H
#define ORBIT_STRING_H

#include <stdlib.h>

#include "common/type.h"
#include "common/portability.h"
#include "common/util.h"

// strings and string-related utils.

typedef struct string {
    char*  raw;
    usize len;
} string;

#define NULL_STR ((string){nullptr, 0})
#define is_null_str(str) ((str).raw == nullptr)

#define str_fmt "%.*s"
#define str_arg(str) (int)(str).len, (str).raw

#define string_make(ptr, len) ((string){(ptr), (len)})
#define string_len(s) ((s).len)
#define string_raw(s) ((s).raw)
#define is_within(haystack, needle) (((haystack).raw <= (needle).raw) && ((haystack).raw + (haystack).len >= (needle).raw + (needle).len))
#define substring(str, start, end) ((string){(str).raw + (start), (end) - (start)})
#define substring_len(str, start, len) ((string){(str).raw + (start), (len)})
#define string_wrap(cstring) ((string){(char*)(cstring), strlen((cstring))})
#define strlit(cstring) ((string){(char*)cstring, sizeof(cstring)-1})

#define string_lt(a, b) (string_cmp((a), (b)) < 0)
#define string_gt(a, b) (string_cmp((a), (b)) > 0)
#define string_le(a, b) (string_cmp((a), (b)) <= 0)
#define string_ge(a, b) (string_cmp((a), (b)) >= 0)

#define string_free(str) free(str.raw)

/// \brief Allocates len bytes for a new string
///
/// \param len Length of buffer
///
/// \return string A string with a buffer of at least length len. The resulting 
///               buffer is zero-filled.
///
/// \note a null-terminated string can be allocated by providing len + 1.
static inline string string_alloc(usize len);

/// \brief Clones the source string to owned C string.
///
/// Allocates a null-terminated C string, and copies the source string.
///
/// \return cstring Owned null terminated C string.
///
/// \see clone_to_string
static inline char* clone_to_cstring(string str);

/// \brief Prints a string
///
/// Emits each character up to the sources length to stdout.
static inline void printstr(string str);

/// \brief Prints to a newly allocated string.
///
/// \param format printf format specifier
/// \param ... printf format arguments
///
/// \return str null-terminated string, or NULL_STR on allocation fail.
string strprintf(char* format, ...) FORMAT_CHECK(1, 2);

/// \brief Clones the source string.
///
/// Allocates and copies the source string, without inserting a null terminator.
///
/// \return string Owned string
///
/// \see clone_to_cstring
string string_clone(string str);

/// \brief Concatenates two strings.
///
/// Allocates a buffer containing the concatenation of a and b.
///
/// \return str Allocated string containing ab, without a null terminator.
string string_concat(string a, string b);

/// \brief Concatenates two strings into buf.
///
/// \param[out] buf buffer of minimum length a.len + b.len.
///            buf is filled with the concatenation ab, without a null terminator
///
/// \warning buffer must be large enough contain a and b, crashes otherwise.
void string_concat_buf(string buf, string a, string b);

/// \brief Compares two strings.
///
/// \return cond Returns 0 when equal, -1 when a < b, and 1 when a > b
///             (their byte-wise, aggregate u8 difference is used to compare).
///
/// \note for equality, string_eq is more efficient, using heuristic length 
/// checks before checking character by character.
///
/// \see string_eq
isize string_cmp(string a, string b);

/// \brief Checks if two strings are equal.
///
/// \return cond true if equal, else false
///
/// \see string_cmp
bool string_eq(string a, string b);

/// \brief Checks if string ends with another.
///
/// \return cond true if yes, else false. If the ending string is larger than 
///              the source, returns false.
bool string_ends_with(string source, string ending);

#endif // ORBIT_STRING_H
