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

#ifndef MSCOPE_H_
#define MSCOPE_H_

#include "hashtable.h"
#include "node.h"

#define SYMBOL_TABLE_SIZE 1024

typedef enum SymbolType
{
    SYMBOL_VARIABLE = 0,
    SYMBOL_FUNCTION = 1,
    SYMBOL_TYPE = 2
} SymbolType;

typedef struct Symbol
{
    char *name;
    void *symbol_info;
    SymbolType symbol_type;
    struct Symbol *next;
} Symbol;

typedef struct Scope
{
    struct Scope *parent;
    HashTable *symbol_table;
    void *function_ref;
} Scope;

Scope *push_scope(Scope *parent, void *function_ref);
Scope *pop_scope(Scope *scope);
void *find_enclosing_function_ref(Scope* scope);
Symbol *insert_symbol(Scope *scope, char *name, SymbolType symbol_type, void *symbol_info);
Symbol *lookup_symbol(Scope *scope, char *name);
void dispose_scope(Scope *scope);

#endif