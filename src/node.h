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
    N_INTEGER,
    N_NAME,
    N_EXPRESSION,
    N_VARIABLE,
    N_ASSIGNMENT,
    N_FUNCTION,
    N_CALL,
    N_RETURN,
    N_BLOCK,
    N_IF,
    N_WHILE,
    N_FLOAT,
} NodeType;

typedef struct Node
{
    NodeType node_type;
    void *data;
    struct Node *next;
} Node;

typedef struct Block
{
    Node *statements;
} Block;

typedef struct Integer
{
    int value;
} Integer;

typedef struct Float
{
    float value;
} Float;

typedef struct Name
{
    char *value;
} Name;

typedef struct Expression
{
    Token *token;
    struct Expression *next;
    struct Expression *left;
    struct Expression *right;
    TypeInfo *type_info;
    Node *node;
} Expression;

typedef struct If
{
    Expression *condition;
    Block *body;
    struct If *next;
} If;

typedef struct While
{
    Expression *condition;
    Block *body;
} While;

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
void dispose_node(Node *node);
Variable *new_variable(char *name, TypeInfo *type_info, Expression *expression);
Assignment *new_assignment(char *name, TypeInfo *type_info, Expression *expression);
Call *new_call(char *name, TypeInfo *type_info, Expression *expression);
Expression *new_expression(Token *token, Expression *left, Expression *right, Node *node, TypeInfo *type_info);
Integer *new_integer(int value);
Float *new_float(float value);
Name *new_name(char *value);
Function *new_function(char *name, TypeInfo *type_info, Variable *params, Block *body);
Block *new_block(Node *statements);
Return *new_return(Expression *expression);
TypeInfo *new_type_info(Type type);
If *new_if(Expression *condition, Block *body);
While *new_while(Expression *condition, Block *body);

#endif
