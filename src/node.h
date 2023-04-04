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
#include "scope.h"
#include "type.h"

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
    Node *left;
    Node *right;
} Expression;

typedef struct Variable
{
    char *name;
    Node *type_info;
    Node *expression;
} Variable;

typedef struct Assignment
{
    char *name;
    Node *type_info;
    Node *expression;
} Assignment;

typedef struct Call
{
    char *name;
    Node *type_info;
    Node *arguments;
} Call;

typedef struct Block
{
    Node *statements;
} Block;

typedef struct Return
{
    Node *expression;
} Return;

typedef struct Function
{
    char *name;
    Node *type_info;
    Node *parameters;
    Node *body;
} Function;

Node *new_node(NodeType nodeType, void *data);
Node *new_variable(char *name, Node *type_info, Node *expression);
Node *new_assignment(Symbol *symbol, Node *expression);
Node *new_call(Symbol *symbol, Node *arguments);
Node *new_expression(char *op, Node *left, Node *right);
Node *new_number(int value);
Node *new_name(char *value);
Node *new_function(char *name, Node *type_info, Node *parameters, Node *body);
Node *new_block(Node *statements);
Node *new_return(Node *expression);
Node *new_type_info(Type type);

char *node_to_string(Node *node);
void destroy_node(Node *node);

#endif
