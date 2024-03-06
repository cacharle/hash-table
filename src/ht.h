#ifndef __HT_H__
#define __HT_H__

#include <stdbool.h>

#define BUCKETS_COUNT 1024
#define TOMBSTONE -1

struct hash_table_bucket
{
    char *key;
    int   value;
};

typedef struct
{
    struct hash_table_bucket buckets[BUCKETS_COUNT];
} hash_table;

hash_table *hash_table_new(void);
void        hash_table_destroy(hash_table *);
void        hash_table_insert(hash_table *, char *, int);
void        hash_table_delete(hash_table *, char *);
int         hash_table_get(hash_table *, char *);
bool        hash_table_contains(hash_table *, char *);

#endif
