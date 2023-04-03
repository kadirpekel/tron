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

#include "mNode.h"

Node *newNode(NodeType nodeType, void *data)
{
    Node *node = malloc(sizeof(Node));
    node->nodeType = nodeType;
    node->data = data;
    return node;
}

Node *newAssignment(char *name, Node *expression)
{

    Assignment *assignment = malloc(sizeof(Assignment));
    assignment->name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy(assignment->name, name);
    assignment->expression = expression;
    return newNode(N_ASSIGNMENT, assignment);
}

Node *newExpression(char *op, Node *left, Node *right)
{
    Expression *expression = malloc(sizeof(Expression));
    expression->op = malloc((strlen(op) + 1) * sizeof(char));
    strcpy(expression->op, op);
    expression->left = left;
    expression->right = right;
    return newNode(N_EXPRESSION, expression);
}

Node *newNumber(int value)
{
    Number *number = malloc(sizeof(Number));
    number->value = value;
    return newNode(N_NUMBER, number);
}

Node *newName(char *value)
{
    Name *name = malloc(sizeof(Name));
    name->value = malloc((strlen(value) + 1) * sizeof(char));
    strcpy(name->value, value);
    return newNode(N_NAME, name);
}

char *nodeToString(Node *node)
{
    char *nodeTypeName;
    switch (node->nodeType)
    {
    case N_ASSIGNMENT:
        nodeTypeName = "ASSIGNMENT";
        break;
    case N_EXPRESSION:
        nodeTypeName = "EXPRESSION";
        break;
    case N_NUMBER:
        nodeTypeName = "NUMBER";
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

void destroyNode(Node *node)
{
    if (!node)
    {
        return;
    }

    // Check type of node and free associated resources accordingly

    free(node->data);
    free(node);
}
