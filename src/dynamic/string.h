#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

typedef struct string string;

struct string
{
  buffer buffer;
};

/* constructor/destructor */
void     string_construct(string *);
void     string_destruct(string *);
void     string_copy(string *, string *);

/* capacity */
size_t   string_length(string *);
size_t   string_capacity(string *);
void     string_reserve(string *, size_t);
void     string_clear(string *);
int      string_empty(string *);
void     string_shrink_to_fit(string *);

/* modifiers */
void     string_insert(string *, size_t, char *);
void     string_insert_buffer(string *, size_t, char *, size_t);
void     string_insert_substring(string *, size_t, string *, size_t, size_t);
void     string_prepend(string *, char *);
void     string_append(string *, char *);
void     string_erase(string *, size_t, size_t);
void     string_replace(string *, size_t, size_t, char *);
void     string_replace_all(string *, char *, char *);

/* string operations */
char    *string_data(string *);
ssize_t  string_find(string *, char *, size_t);
int      string_compare(string *, string *);
//vector  *string_split(string *, char *);

#endif /* STRING_H_INLCUDED */
