#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

uint64_t hash_string(char *);
uint64_t hash_data(void *, size_t);
uint64_t hash_uint64(uint64_t);

#endif /* HASH_H_INCLUDED */
