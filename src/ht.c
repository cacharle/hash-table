#include "ht.h"
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <string.h>

static void *xmalloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL)
        abort();
    return p;
}

static void *xstrdup(const char *s)
{
    char *p = strdup(s);
    if (p == NULL)
        abort();
    return p;
}

hash_table *hash_table_new(void)
{
    hash_table *ht = xmalloc(sizeof(hash_table));
    memset(ht->buckets, 0, sizeof ht->buckets);
    return ht;
}

void hash_table_destroy(hash_table *ht)
{
    for (size_t i = 0; i < BUCKETS_COUNT; i++)
        free(ht->buckets[i].key);
    free(ht);
}

static size_t hash(const char *key)
{
    size_t digest = 1;
    for (; *key != '\0'; key++)
    {
        digest *= *key;
        digest %= BUCKETS_COUNT;
    }
    return digest;
}

static struct hash_table_bucket *get_bucket(hash_table *ht, char *key)
{
    size_t digest = hash(key);
    for (size_t probe = 0; probe < BUCKETS_COUNT; probe++)
    {
        struct hash_table_bucket *bucket =
            &ht->buckets[(digest + probe) % BUCKETS_COUNT];
        if (bucket->key == NULL)
            return bucket;
        if (strcmp(bucket->key, key) == 0)
            return bucket;
    }
    return NULL;
}

void hash_table_insert(hash_table *ht, char *key, int value)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key);
    if (bucket->key == NULL)
        bucket->key = xstrdup(key);
    bucket->value = value;
}

void hash_table_delete(hash_table *ht, char *key)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key);
    if (bucket == NULL)
        return;
    free(bucket->key);
    bucket->key = NULL;
    bucket->value = 0;
}

int hash_table_get(hash_table *ht, char *key)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key);
    if (bucket == NULL || bucket->key == NULL)
        abort();
    return bucket->value;
}

bool hash_table_contains(hash_table *ht, char *key)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key);
    return bucket != NULL && bucket->key != NULL;
}
