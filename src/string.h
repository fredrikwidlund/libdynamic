#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

typedef struct string string;

struct string
{
  buffer buffer;
};

/* allocators */

string  *string_new(char *);
void     string_free(string *);
void     string_init(string *, char *);
char    *string_deconstruct(string *);

/* capacity */

size_t   string_length(string *);
size_t   string_capacity(string *);
int      string_reserve(string *, size_t);
void     string_clear(string *);
int      string_empty(string *);
int      string_shrink_to_fit(string *);

/* modifiers */

int      string_prepend(string *, char *);
int      string_append(string *, char *);
int      string_insert(string *, size_t, char *);
int      string_erase(string *, size_t, size_t);
int      string_replace(string *, size_t, size_t, char *);
int      string_replace_all(string *, char *, char *);

/* string operations */

char    *string_data(string *);
int      string_copy(string *, char *, size_t, size_t, size_t *);
size_t   string_find(string *, char *, size_t);
string  *string_substr(string *, size_t, size_t);
int      string_compare(string *, string *);
vector  *string_split(string *, char *);

/* internals */

void string_split_release(void *);

#endif /* STRING_H_INLCUDED */
