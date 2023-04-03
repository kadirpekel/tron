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

#include "mAssert.h"
#include "mToken.h"

typedef enum NodeType
{
    N_NUMBER = 0,
    N_NAME = 1 << 0,
    N_EXPRESSION = 1 << 1,
    N_ASSIGNMENT = 1 << 2,

} NodeType;

typedef struct Node
{
    NodeType nodeType;
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

typedef struct Assignment
{
    char *name;
    Node *expression;
} Assignment;

Node *newNode(NodeType nodeType, void *data);
Node *newAssignment(char *name, Node *expression);
Node *newExpression(char *op, Node *left, Node *right);
Node *newNumber(int value);
Node *newName(char *value);

char *nodeToString(Node *node);
void destroyNode(Node *node);

#endif
