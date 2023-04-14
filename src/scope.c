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

Scope *new_scope(Scope *parent, Function *function)
{
    Scope *scope = malloc(sizeof(Scope));
    scope->parent = parent;
    scope->function = function;
    scope->symbol_table = malloc(SYMBOL_TABLE_SIZE * sizeof(Symbol));

    if (parent == NULL)
    {
        // Global builtins
        // Types
        insert_symbol(scope, "int", SYMBOL_TYPE, new_type_info(TYPE_INT));
        insert_symbol(scope, "float", SYMBOL_TYPE, new_type_info(TYPE_FLOAT));
        // Functions
        insert_symbol(scope, "print", SYMBOL_FUNCTION, new_type_info(TYPE_INT));
        insert_symbol(scope, "as_int", SYMBOL_FUNCTION, new_type_info(TYPE_INT));
        insert_symbol(scope, "as_float", SYMBOL_FUNCTION, new_type_info(TYPE_FLOAT));
    }

    return scope;
}

unsigned int hash(char *str)
{
    unsigned int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % SYMBOL_TABLE_SIZE;
}

Symbol *insert_symbol(Scope *scope, char *name, SymbolType symbol_type, TypeInfo *type_info)
{
    Symbol *existing = lookup_symbol(scope, name);
    if (existing != NULL)
    {
        return NULL;
    }
    unsigned int index = hash(name);
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    symbol->name = strdup(name);
    symbol->symbol_type = symbol_type;
    symbol->type_info = type_info;
    symbol->next = scope->symbol_table[index];
    scope->symbol_table[index] = symbol;
    return symbol;
}

Symbol *lookup_symbol(Scope *scope, char *name)
{
    if (scope == NULL)
    {
        return NULL;
    }

    unsigned int index = hash(name);
    Symbol *symbol = scope->symbol_table[index];

    while (symbol != NULL)
    {
        if (strcmp(symbol->name, name) == 0)
        {
            return symbol;
        }
        symbol = symbol->next;
    }

    return lookup_symbol(scope->parent, name);
}

void dispose_scope(Scope *scope)
{
    free(scope->symbol_table);
    free(scope);
}