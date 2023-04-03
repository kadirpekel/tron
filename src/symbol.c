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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>

#include "symbol.h"

Symbol *symbol_table[SYMBOL_TABLE_SIZE] = {NULL};

unsigned int hash(const char *str)
{
    unsigned int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % SYMBOL_TABLE_SIZE;
}

void insert_symbol(const char *name, SymbolType symbol_type, Type type)
{
    unsigned int index = hash(name);

    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->symbol_type = symbol_type;
    new_symbol->type = type;
    new_symbol->next = symbol_table[index];

    symbol_table[index] = new_symbol;
}

Symbol *lookup_symbol(const char *name)
{
    unsigned int index = hash(name);
    Symbol *symbol = symbol_table[index];

    while (symbol != NULL)
    {
        if (strcmp(symbol->name, name) == 0)
        {
            return symbol;
        }
        symbol = symbol->next;
    }

    return NULL;
}