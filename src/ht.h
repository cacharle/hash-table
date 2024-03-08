#ifndef __HT_H__
#define __HT_H__

#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdbool.h>

#define TOMBSTONE -1

struct hash_table_bucket
{
    char *key;
    int   value;
};

typedef struct
{
    size_t capacity;
    size_t occupied_count;
    struct hash_table_bucket *buckets;
} hash_table;

hash_table *hash_table_new(void);
void        hash_table_destroy(hash_table *);
void        hash_table_insert(hash_table *, char *, int);
void        hash_table_delete(hash_table *, char *);
int         hash_table_get(hash_table *, char *);
bool        hash_table_contains(hash_table *, char *);

#endif
