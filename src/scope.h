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
#include <assert.h>

#include "hashtable.h"
#include "node.h"
#include "stdbool.h"

#define SYMBOL_TABLE_SIZE 1024

typedef enum SymbolType
{
    SYMBOL_VARIABLE = 0,
    SYMBOL_FUNCTION = 1,
    SYMBOL_TYPE = 2,
    SYMBOL_ARG = 3,
} SymbolType;

typedef enum ScopeType
{
    SCOPE_ROOT = 0,
    SCOPE_FUNCTION = 1,
    SCOPE_IF = 2,
    SCOPE_WHILE = 3,
} ScopeType;

typedef struct Symbol
{
    char *name;
    void *info;
    SymbolType type;
    void (*dispose_info)(void *);
    struct Symbol *next;
} Symbol;

typedef struct Scope
{
    struct Scope *parent;
    void (*dispose_info)(void *);
    void (*dispose_symbol_info)(void *);
    HashTable *symbol_table;
    void *info;
    ScopeType type;
} Scope;

Scope *push_scope(Scope *parent, ScopeType type, void *info, void (*dispose_info)(void *), void (*dispose_symbol_info)(void *));
Scope *pop_scope(Scope *scope);
void *find_enclosing_scope_info(Scope *scope, ScopeType block_type);
Symbol *insert_symbol(Scope *scope, SymbolType type, char *name, void *info);
Symbol *lookup_symbol(Scope *scope, char *name);
void dispose_scope(Scope *scope);

#endif