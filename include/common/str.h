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

char*  clone_to_cstring(string str); // this allocates
void   printstr(string str);
string strprintf(char* format, ...) FORMAT_CHECK(1, 2);

string  string_alloc(usize len);
string  string_clone(string str); // this allocates as well
string  string_concat(string a, string b); // allocates
void    string_concat_buf(string buf, string a, string b); // this does not

u8   string_cmp(string a, string b);
bool string_eq(string a, string b);
bool string_ends_with(string source, string ending);

#endif // ORBIT_STRING_H
