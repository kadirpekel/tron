/******************************************************************************
 * Copyright [2023] [Kadir PEKEL]
 *
 * Licensed under the Apache License, Version 2.0 (the "License\n");
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

#include <stdlib.h>
#include <string.h>

#include "scope.h"

Scope *push_scope(Scope *parent, ScopeType type, void *info, void (*dispose_info)(void *), void (*dispose_symbol_info)(void *))
{

    Scope *scope = malloc(sizeof(Scope));
    scope->parent = parent;
    scope->info = info;
    scope->type = type;
    scope->symbol_table = new_hash_table(SYMBOL_TABLE_SIZE);
    if (dispose_info == NULL)
    {
        assert(scope->parent != NULL);
        assert(scope->parent->dispose_info != NULL);
        scope->dispose_info = scope->parent->dispose_info;
    }
    else
    {
        scope->dispose_info = dispose_info;
    }

    if (dispose_symbol_info == NULL)
    {
        assert(scope->parent != NULL);
        assert(scope->parent->dispose_symbol_info != NULL);
        scope->dispose_symbol_info = scope->parent->dispose_symbol_info;
    }
    else
    {
        scope->dispose_symbol_info = dispose_symbol_info;
    }
    return scope;
}

Scope *pop_scope(Scope *scope)
{
    Scope *parent = scope->parent;
    dispose_scope(scope);
    return parent;
}

void *find_enclosing_scope_info(Scope *scope, ScopeType scope_type)
{
    Scope *current = scope;
    while (current != NULL)
    {
        if (current->type == scope_type)
        {
            return scope->info;
        }
        current = current->parent;
    }
    return NULL;
}

Symbol *insert_symbol(Scope *scope, SymbolType type, char *name, void *info)
{
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->info = info;
    symbol->type = type;
    symbol->dispose_info = scope->dispose_symbol_info;
    Bucket *bucket = insert_value(scope->symbol_table, symbol->name, symbol);
    if (bucket != NULL)
    {
        return (Symbol *)bucket->value;
    }
    return NULL;
}

Symbol *lookup_symbol(Scope *scope, char *name)
{
    Scope *current = scope;
    while (current != NULL)
    {
        Bucket *bucket = (Bucket *)lookup_value(current->symbol_table, name);
        if (bucket != NULL)
        {
            return (Symbol *)bucket->value;
        }
        current = current->parent;
    }
    return NULL;
}

void dispose_bucket_value(void *value)
{
    Symbol *symbol = (Symbol *)value;
    if (symbol->dispose_info != NULL)
    {
        symbol->dispose_info(symbol->info);
    }
    free(symbol->name);
    free(symbol);
}

void dispose_scope(Scope *scope)
{
    dispose_hash_table(scope->symbol_table, dispose_bucket_value);
    if (scope->dispose_info != NULL)
    {
        scope->dispose_info(scope->info);
    }
    free(scope);
}