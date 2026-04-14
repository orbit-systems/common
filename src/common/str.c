#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common/str.h"

string strprintf(char* format, ...) {
    string c = NULL_STR;
    va_list a;
    va_start(a, format);
    va_list b;
    va_copy(b, a);
    usize bufferlen = 1 + vsnprintf("", 0, format, a);
    c = string_alloc(bufferlen);
    vsnprintf(c.raw, c.len, format, b);
    c.len--;
    va_end(a);
    va_end(b);
    return c;
}

string string_concat(string a, string b) {
    string c = string_alloc(a.len + b.len);
    string_concat_buf(c, a, b);
    return c;
}

void string_concat_buf(string buf, string a, string b) {
    if (buf.len < a.len + b.len)
        CRASH("Buffer is too small. %zu < %zu + %zu", buf.len, a.len, b.len);

    usize i;
    for_n(i, 0, a.len) {
        buf.raw[i] = a.raw[i];
    }
    for_n(i, 0, b.len) {
        buf.raw[a.len + i] = b.raw[i];
    }
}

bool string_ends_with(string source, string ending) {
    if (source.len < ending.len) return false;

    return string_eq(substring_len(source, source.len-ending.len, ending.len), ending);
}

inline string string_alloc(usize len) {
    return (string){calloc(len, sizeof(char*)), len};
};

u8 string_cmp(string a, string b) {
    // copied from odin's implementation lmfao
    int res = memcmp(a.raw, b.raw, a.len < b.len ? a.len : b.len);
    if (res == 0 && a.len != b.len) return a.len <= b.len ? -1 : 1;
    else if (a.len == 0 && b.len == 0) return 0;
    return res;
}

inline bool string_eq(string a, string b) {
    return a.len == 0 || (a.len == b.len && string_cmp(a, b) == 0);
}

inline char* clone_to_cstring(string str) {
    // NOTE: the returned char* is always allocated, so no null literal
    // (otherwise subsequent realloc results in undefined behaviour).
    // The last null character is always allocated internally.
    return strndup(str.raw, str.len);
}

string string_clone(string str) {
    string new_str = string_alloc(str.len);
    if (memmove(new_str.raw, str.raw, str.len) != new_str.raw) return NULL_STR;
    return new_str;
}

void printn(char* text, usize len) {
    usize c = 0;
    while (c < len && text[c] != '\0')
        putchar(text[c++]);
}

inline void printstr(string str) {
    printn(str.raw, str.len);
}
