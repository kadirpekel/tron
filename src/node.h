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

#ifndef MNODE_H_
#define MNODE_H_

#include <stdio.h>

#include "assert.h"
#include "token.h"
#include "type.h"
typedef struct TypeInfo
{
    Type type;
    struct TypeInfo *next;
} TypeInfo;

typedef enum NodeType
{
    N_NUMBER = 0,
    N_NAME = 1 << 0,
    N_EXPRESSION = 1 << 1,
    N_VARIABLE = 1 << 2,
    N_ASSIGNMENT = 1 << 3,
    N_FUNCTION = 1 << 4,
    N_CALL = 1 << 5,
    N_RETURN = 1 << 6,
    N_BLOCK = 1 << 7,
    N_TYPEINFO = 1 << 8,
} NodeType;

typedef enum LeafType
{
    L_NA = 0,
    L_NUMBER = 1 << 0,
    L_NAME = 1 << 1,
    L_CALL = 1 << 2,
} LeafType;

typedef struct Node
{
    NodeType node_type;
    void *data;
    struct Node *next;
} Node;

typedef struct Number
{
    int value;
} Number;

typedef struct Name
{
    char *value;
} Name;

typedef struct Expression
{
    char *op;
    struct Expression *next;
    struct Expression *left;
    struct Expression *right;
    LeafType leaf_type;
    void *leaf;
} Expression;

typedef struct Variable
{
    char *name;
    TypeInfo *type_info;
    Expression *expression;
    struct Variable *next;
} Variable;

typedef struct Assignment
{
    char *name;
    TypeInfo *type_info;
    Expression *expression;
} Assignment;

typedef struct Call
{
    char *name;
    TypeInfo *type_info;
    Expression *expression;
} Call;

typedef struct Block
{
    Node *statements;
} Block;

typedef struct Return
{
    Expression *expression;
} Return;

typedef struct Function
{
    char *name;
    TypeInfo *type_info;
    Variable *params;
    Block *body;
} Function;

Node *new_node(NodeType nodeType, void *data);
Variable *new_variable(char *name, TypeInfo *type_info, Expression *expression);
Assignment *new_assignment(char *name, TypeInfo *type_info, Expression *expression);
Call *new_call(char *name, TypeInfo *type_info, Expression *expression);
Expression *new_expression(char *op, Expression *left, Expression *right, LeafType leaf_type, void *leaf);
Number *new_number(int value);
Name *new_name(char *value);
Function *new_function(char *name, TypeInfo *type_info, Variable *params, Block *body);
Block *new_block(Node *statements);
Return *new_return(Expression *expression);
TypeInfo *new_type_info(Type type);

char *node_to_string(Node *node);
void destroy_node(Node *node);

#endif
