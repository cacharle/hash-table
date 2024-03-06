#include "ht.h"
#include <criterion/criterion.h>
#include <signal.h>

Test(hash_table, new)
{
    hash_table *ht = hash_table_new();
    cr_assert_not_null(ht);
    for (size_t i = 0; i < BUCKETS_COUNT; i++)
    {
        cr_assert_null(ht->buckets[i].key);
        cr_assert_eq(ht->buckets[i].value, 0);
    }
}

Test(hash_table, destroy)
{
    hash_table *ht = hash_table_new();
    hash_table_destroy(ht);
}

Test(hash_table, insert_get)
{
    hash_table *ht = hash_table_new();
    hash_table_insert(ht, "foo", 42);
    cr_assert(hash_table_contains(ht, "foo"));
    int value = hash_table_get(ht, "foo");
    cr_assert_eq(value, 42);
}

Test(hash_table, insert_same_key)
{
    hash_table *ht = hash_table_new();
    hash_table_insert(ht, "foo", 42);
    hash_table_insert(ht, "foo", 24);
    int value = hash_table_get(ht, "foo");
    cr_assert_eq(value, 24);
}

Test(hash_table, delete)
{
    hash_table *ht = hash_table_new();
    hash_table_insert(ht, "foo", 42);
    cr_assert(hash_table_contains(ht, "foo"));
    hash_table_delete(ht, "foo");
    cr_assert(!hash_table_contains(ht, "foo"));
    // delete non existant key should do nothing
    hash_table_delete(ht, "foo");
    hash_table_delete(ht, "foo");
}

Test(hash_table, get_non_existant_crash, .signal = SIGABRT)
{
    hash_table *ht = hash_table_new();
    hash_table_get(ht, "foo");
}
