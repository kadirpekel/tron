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

#include "node.h"

Node *new_node(NodeType nodeType, void *data)
{
    Node *node = malloc(sizeof(Node));
    node->node_type = nodeType;
    node->data = data;
    return node;
}

Node *new_variable(char *name, Type type, Node *expression)
{

    Variable *variable = malloc(sizeof(Variable));
    variable->name = malloc((strlen(name) + 1) * sizeof(char));
    variable->type = type;
    strcpy(variable->name, name);
    variable->expression = expression;
    insert_symbol(variable->name, SYMBOL_VARIABLE, type);
    return new_node(N_VARIABLE, variable);
}

Node *new_assignment(Symbol *symbol, Node *expression)
{
    Assignment *assignment = malloc(sizeof(Assignment));
    assignment->name = malloc((strlen(symbol->name) + 1) * sizeof(char));
    assignment->type = symbol->type;
    strcpy(assignment->name, symbol->name);
    assignment->expression = expression;
    return new_node(N_ASSIGNMENT, assignment);
}

Node *new_call(Symbol *symbol, Node *arguments)
{
    Call *call = malloc(sizeof(Call));
    call->name = malloc((strlen(symbol->name) + 1) * sizeof(char));
    call->type = symbol->type;
    strcpy(call->name, symbol->name);
    call->arguments = arguments;
    return new_node(N_CALL, call);
}

Node *new_expression(char *op, Node *left, Node *right)
{
    Expression *expression = malloc(sizeof(Expression));
    expression->op = malloc((strlen(op) + 1) * sizeof(char));
    strcpy(expression->op, op);
    expression->left = left;
    expression->right = right;
    return new_node(N_EXPRESSION, expression);
}

Node *new_number(int value)
{
    Number *number = malloc(sizeof(Number));
    number->value = value;
    return new_node(N_NUMBER, number);
}

Node *new_name(char *value)
{
    Name *name = malloc(sizeof(Name));
    name->value = malloc((strlen(value) + 1) * sizeof(char));
    strcpy(name->value, value);
    return new_node(N_NAME, name);
}

char *node_to_string(Node *node)
{
    char *nodeTypeName;
    switch (node->node_type)
    {
    case N_VARIABLE:
        nodeTypeName = "VARIABLE";
        break;
    case N_EXPRESSION:
        nodeTypeName = "EXPRESSION";
        break;
    case N_NUMBER:
        nodeTypeName = "NUMBER";
        break;
    case N_ASSIGNMENT:
        nodeTypeName = "ASSIGNMENT";
        break;
    case N_CALL:
        nodeTypeName = "CALL";
        break;
    default:
        fprintf(stderr, "Invalid node type");
        exit(EXIT_FAILURE);
    }

    char *result;
    int buffer_size = snprintf(NULL, 0, "NODE: %s", nodeTypeName);
    result = (char *)malloc(buffer_size + 1); // +1 for the null-terminator
    snprintf(result, buffer_size + 1, "NODE: %s", nodeTypeName);
    return result;
}

void destroy_node(Node *node)
{
    if (!node)
    {
        return;
    }

    // Check type of node and free associated resources accordingly

    free(node->data);
    free(node);
}
