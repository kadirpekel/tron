/******************************************************************************
 * Copyright [2023] [Kadir PEKEL]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#include "hashtable.h"

unsigned int hash(HashTable *table, char *str)
{
    unsigned int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % table->size;
}

Bucket *new_bucket(char *key, void *value)
{
    Bucket *bucket = malloc(sizeof(Bucket));
    bucket->key = key;
    bucket->value = value;
    return bucket;
}

HashTable *new_hash_table(size_t size)
{
    HashTable *table = malloc(sizeof(HashTable));
    table->size = size;
    table->buckets = calloc(size, sizeof(Bucket *));
    return table;
}

Bucket *insert_value(HashTable *table, char *key, void *value)
{
    Bucket *bucket = lookup_value(table, key);
    if (bucket)
    {
        return NULL;
    }
    unsigned int index = hash(table, key);
    Bucket *next_bucket = new_bucket(key, value);
    next_bucket->next = bucket;
    table->buckets[index] = next_bucket;
    return next_bucket;
}

Bucket *lookup_value(HashTable *table, char *key)
{
    unsigned int index = hash(table, key);
    Bucket *bucket = (Bucket *)table->buckets[index];

    while (bucket != NULL)
    {
        if (strcmp(bucket->key, key) == 0)
        {
            return bucket;
        }
        bucket = bucket->next;
    }
    return NULL;
}

void dispose_hash_table(HashTable *table)
{
    int i;
    for (i = 0; i < table->size; i++)
    {
        Bucket *bucket = table->buckets[i];
        dispose_bucket(bucket);
    }
    free(table);
}

void dispose_bucket(Bucket *bucket)
{
    if (bucket == NULL)
    {
        return;
    }

    dispose_bucket(bucket->next);
    free(bucket);
}
