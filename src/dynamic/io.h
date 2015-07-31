#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED 

/* utilities */

buffer *io_read(int, size_t);
int     io_write(int, buffer *);
int     io_writefile(buffer *, char *);
buffer *io_buffer_load(char *);

string *io_string_load(char *);
string *io_string_get(int);

#endif /* IO_H_INCLUDED */
