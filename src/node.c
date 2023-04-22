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
#include <string.h>

#include "node.h"

Node *new_node(NodeType nodeType, void *data)
{
    Node *node = malloc(sizeof(Node));
    node->node_type = nodeType;
    node->data = data;
    node->next = NULL;
    return node;
}

Variable *new_variable(char *name, TypeInfo *type_info, Assignment *assignment)
{
    Variable *variable = malloc(sizeof(Variable));
    variable->name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy(variable->name, name);
    variable->assignment = assignment;
    variable->type_info = type_info;
    return variable;
}

Assignment *new_assignment(char *name, TypeInfo *type_info, Expression *expression)
{
    Assignment *assignment = malloc(sizeof(Assignment));
    assignment->name = malloc((strlen(name) + 1) * sizeof(char));
    assignment->type_info = type_info;
    strcpy(assignment->name, name);
    assignment->expression = expression;
    return assignment;
}

Call *new_call(char *name, TypeInfo *type_info, Expression *expression)
{
    Call *call = malloc(sizeof(Call));
    call->name = malloc((strlen(name) + 1) * sizeof(char));
    call->type_info = type_info;
    strcpy(call->name, name);
    call->expression = expression;
    return call;
}

Expression *new_expression(Token *token, Expression *left, Expression *right, Node *node, TypeInfo *type_info)
{
    Expression *expression = malloc(sizeof(Expression));
    expression->token = token;
    expression->left = left;
    expression->right = right;
    expression->node = node;
    expression->type_info = type_info;
    expression->next = NULL;
    return expression;
}

Return *new_return(Expression *expression)
{
    Return *return_ = malloc(sizeof(Return));
    return_->expression = expression;
    return return_;
}

Integer *new_integer(int value)
{
    Integer *integer = malloc(sizeof(Integer));
    integer->value = value;
    return integer;
}

Float *new_float(float value)
{
    Float *float_ = malloc(sizeof(Float));
    float_->value = value;
    return float_;
}

TypeInfo *new_type_info(Type type)
{
    TypeInfo *type_info = malloc(sizeof(TypeInfo));
    type_info->type = type;
    type_info->next = NULL;
    return type_info;
}

Name *new_name(char *value)
{
    Name *name = malloc(sizeof(Name));
    name->value = malloc((strlen(value) + 1) * sizeof(char));
    strcpy(name->value, value);
    return name;
}

Function *new_function(char *name, TypeInfo *type_info, Variable *params, Block *body)
{
    Function *function = malloc(sizeof(Function));
    function->name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy(function->name, name);
    function->type_info = type_info;
    function->params = params;
    function->body = body;
    return function;
}

If *new_if(Expression *condition, Block *body)
{
    If *if_ = malloc(sizeof(If));
    if_->condition = condition;
    if_->body = body;
    if_->next = NULL;
    return if_;
}

While *new_while(Expression *condition, Block *body)
{
    While *while_ = malloc(sizeof(While));
    while_->condition = condition;
    while_->body = body;
    return while_;
}

Block *new_block(Node *statements)
{
    Block *block = malloc(sizeof(Block));
    block->statements = statements;
    return block;
}

void dispose_variable(Variable *variable)
{
    // Free associated resources up here

    free(variable);
}

void dispose_expression(Expression *expression)
{
    // Free associated resources up here

    free(expression);
}

void dispose_integer(Integer *integer)
{
    // Free associated resources up here

    free(integer);
}

void dispose_float(Float *float_)
{
    // Free associated resources up here

    free(float_);
}

void dispose_assignment(Assignment *assignment)
{
    // Free associated resources up here

    free(assignment);
}

void dispose_call(Call *call)
{
    // Free associated resources up here

    free(call);
}

void dispose_function(Function *function)
{
    // Free associated resources up here

    free(function);
}

void dispose_return(Return *return_)
{
    // Free associated resources up here

    free(return_);
}
void dispose_if(If *if_)
{
    // Free associated resources up here

    free(if_);
}
void dispose_while(While *while_)
{
    // Free associated resources up here

    free(while_);
}

void dispose_one(Node *node)
{
    if (!node)
    {
        return;
    }

    switch (node->node_type)
    {
    case N_VARIABLE:
        dispose_variable((Variable *)node->data);
        break;
    case N_EXPRESSION:
        dispose_expression((Expression *)node->data);
        break;
    case N_INTEGER:
        dispose_integer((Integer *)node->data);
        break;
    case N_FLOAT:
        dispose_float((Float *)node->data);
        break;
    case N_ASSIGNMENT:
        dispose_assignment((Assignment *)node->data);
        break;
    case N_CALL:
        dispose_call((Call *)node->data);
        break;
    case N_FUNCTION:
        dispose_function((Function *)node->data);
        break;
    case N_RETURN:
        dispose_return((Return *)node->data);
        break;
    case N_IF:
        dispose_if((If *)node->data);
        break;
    case N_WHILE:
        dispose_while((While *)node->data);
        break;
    default:
        fprintf(stderr, "Unexpected node type");
        exit(EXIT_FAILURE);
    }

    free(node);
}

void dispose_node(Node *node)
{
    Node *current = node;
    while (current)
    {
        dispose_one(current);
        current = current->next;
    }
}
