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

typedef enum NodeType
{
    N_NUMBER = 0,
    N_NAME = 1 << 0,
    N_EXPRESSION = 1 << 1,
    N_VARIABLE = 1 << 2,

} NodeType;

typedef struct Node
{
    NodeType node_type;
    void *data;
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
    Type type;
    Node *expression;
} Variable;

Node *new_node(NodeType nodeType, void *data);
Node *new_variable(char *name, Type type, Node *expression);
Node *new_expression(char *op, Node *left, Node *right);
Node *new_number(int value);
Node *new_name(char *value);

char *node_to_string(Node *node);
void destroy_node(Node *node);

#endif
