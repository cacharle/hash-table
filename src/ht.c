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

// djb hashing function
// (https://theartincode.stanis.me/008-djb2/)
static size_t hash(const char *key)
{
    size_t digest = 5381;
    for (; *key != '\0'; key++)
        digest = ((digest << 5) + digest) + *key; // <<5 + 1 means *33
    return digest % BUCKETS_COUNT;
}

static struct hash_table_bucket *get_bucket(hash_table *ht, char *key, bool skip_tombstone)
{
    size_t digest = hash(key);
    for (size_t probe = 0; probe < BUCKETS_COUNT; probe++)
    {
        // probe^2 since quadratic probing reduces clustering
        struct hash_table_bucket *bucket =
            &ht->buckets[(digest + (probe * probe)) % BUCKETS_COUNT];
        if (bucket->key == NULL)
        {
            if (skip_tombstone && bucket->value == TOMBSTONE)
                continue;
            return bucket;
        }
        if (strcmp(bucket->key, key) == 0)
            return bucket;
    }
    return NULL;
}

void hash_table_insert(hash_table *ht, char *key, int value)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key, false);
    if (bucket->key == NULL)
        bucket->key = xstrdup(key);
    bucket->value = value;
}
// TODO: recreate the hash table when there is too many tombstones (when half of
// table is tombstones) since tombstones causes lookups to be slower

// TODO: make the hash table use a dynamic array instead of fixed length to avoid
// crash when running out of buckets

// FIXME: have to shift back the collided values
void hash_table_delete(hash_table *ht, char *key)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key, true);
    if (bucket == NULL)
        return;
    free(bucket->key);
    bucket->key = NULL;
    bucket->value = TOMBSTONE;
}

int hash_table_get(hash_table *ht, char *key)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key, true);
    if (bucket == NULL || bucket->key == NULL)
        abort();
    return bucket->value;
}

bool hash_table_contains(hash_table *ht, char *key)
{
    struct hash_table_bucket *bucket = get_bucket(ht, key, true);
    return bucket != NULL && bucket->key != NULL;
}
