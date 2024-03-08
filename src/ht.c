#include "ht.h"
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <string.h>

// Good blog post explaining the different implementations of hash tables:
// https://thenumb.at/Hashtables/

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

static hash_table *hash_table_with_capacity(size_t capacity)
{
    hash_table *ht = xmalloc(sizeof(hash_table));
    ht->capacity = capacity;
    ht->occupied_count = 0;
    ht->buckets = xmalloc(capacity * sizeof(struct hash_table_bucket));
    return ht;
}

hash_table *hash_table_new(void)
{
    return hash_table_with_capacity(0);
}

void hash_table_destroy(hash_table *ht)
{
    for (size_t i = 0; i < ht->capacity; i++)
        free(ht->buckets[i].key);
    free(ht->buckets);
    free(ht);
}

// djb hashing function
// (https://theartincode.stanis.me/008-djb2/)
static size_t hash(hash_table *ht, const char *key)
{
    size_t digest = 5381;
    for (; *key != '\0'; key++)
        digest = ((digest << 5) + digest) + *key; // <<5 + 1 means *33
    return digest % ht->capacity;
}

static struct hash_table_bucket *get_bucket(hash_table *ht, char *key, bool skip_tombstone)
{
    size_t digest = hash(ht, key);
    for (size_t probe = 0; probe < ht->capacity; probe++)
    {
        // probe^2 since quadratic probing reduces clustering
        struct hash_table_bucket *bucket =
            &ht->buckets[(digest + (probe * probe)) % ht->capacity];
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

static const size_t GROWTH_FACTOR = 2;

void hash_table_insert(hash_table *ht, char *key, int value)
{
    if (ht->occupied_count >= ht->capacity)
    {
        if (ht->capacity == 0)
            ht->capacity = 1;
        hash_table *ht_new = hash_table_with_capacity(ht->capacity * GROWTH_FACTOR);
        for (size_t i = 0; i < ht->capacity; i++)
        {
            if (ht->buckets[i].key == NULL)
                continue;
            hash_table_insert(ht_new, ht->buckets[i].key, ht->buckets[i].value);
        }
        // TODO: insert_no_dup
        for (size_t i = 0; i < ht->capacity; i++)
            free(ht->buckets[i].key);
        free(ht->buckets);
        ht->buckets = ht_new->buckets;
        ht->capacity = ht_new->capacity;
        free(ht_new);
    }
    struct hash_table_bucket *bucket = get_bucket(ht, key, false);
    if (bucket->key == NULL)
        bucket->key = xstrdup(key);
    bucket->value = value;
    ht->occupied_count++;
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
    ht->occupied_count--;
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
