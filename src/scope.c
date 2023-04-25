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

Scope *push_scope(Scope *parent, void *function_ref, void (*dispose_symbol_info)(void *))
{
    Scope *scope = malloc(sizeof(Scope));
    scope->parent = parent;
    scope->function_ref = function_ref;
    scope->dispose_symbol_info = dispose_symbol_info;
    scope->symbol_table = new_hash_table(SYMBOL_TABLE_SIZE);
    return scope;
}

Scope *pop_scope(Scope *scope)
{
    Scope *parent = scope->parent;
    dispose_scope(scope);
    return parent;
}

void *find_enclosing_function_ref(Scope *scope)
{
    void *function_ref = scope->function_ref;
    while (function_ref == NULL && scope->parent != NULL)
    {
        function_ref = scope->parent->function_ref;
    }
    return function_ref;
}

Symbol *insert_symbol(Scope *scope, char *name, SymbolType symbol_type, void *symbol_info)
{
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->symbol_type = symbol_type;
    symbol->symbol_info = symbol_info;
    symbol->dispose_symbol_info = scope->dispose_symbol_info;
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
    if (symbol->dispose_symbol_info != NULL)
    {
        symbol->dispose_symbol_info(symbol->symbol_info);
    }
    free(symbol->name);
    free(symbol);
}

void dispose_scope(Scope *scope)
{
    dispose_hash_table(scope->symbol_table, dispose_bucket_value);
    free(scope);
}