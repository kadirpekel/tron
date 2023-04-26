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

TypeInfo *dup_type_info(TypeInfo *type_info)
{
    if (type_info == NULL)
    {
        return NULL;
    }
    TypeInfo *dup = memdup(type_info, sizeof(TypeInfo));
    dup->next = dup_type_info(type_info->next);
    return dup;
}

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
    variable->name = strdup(name);
    variable->assignment = assignment;
    variable->type_info = type_info;
    variable->next = NULL;
    return variable;
}

Assignment *new_assignment(char *name, TypeInfo *type_info, Expression *expression)
{
    Assignment *assignment = malloc(sizeof(Assignment));
    assignment->name = strdup(name);
    assignment->type_info = type_info;
    assignment->expression = expression;
    return assignment;
}

Call *new_call(char *name, TypeInfo *type_info, Expression *expression)
{
    Call *call = malloc(sizeof(Call));
    call->name = strdup(name);
    call->type_info = type_info;
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
    name->value = strdup(value);
    return name;
}

Function *new_function(char *name, TypeInfo *type_info, Variable *params, Block *body)
{
    Function *function = malloc(sizeof(Function));
    function->name = strdup(name);
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

void dispose_block(Block *block)
{
    if (block == NULL)
    {
        return;
    }
    dispose_node(block->statements);
    free(block);
}

void dispose_type_info(TypeInfo *type_info)
{
    if (type_info == NULL)
    {
        return;
    }
    dispose_type_info(type_info->next);
    free(type_info);
}

void dispose_variable(Variable *variable)
{
    if (variable == NULL)
    {
        return;
    }
    dispose_variable(variable->next);
    dispose_assignment(variable->assignment);
    dispose_type_info(variable->type_info);
    free(variable->name);
    free(variable);
}

void dispose_expression(Expression *expression)
{
    if (expression == NULL)
    {
        return;
    }
    dispose_expression(expression->next);
    dispose_expression(expression->left);
    dispose_expression(expression->right);
    dispose_node(expression->node);
    dispose_token(expression->token);
    free(expression);
}

void dispose_integer(Integer *integer)
{
    if (integer == NULL)
    {
        return;
    }
    free(integer);
}

void dispose_float(Float *float_)
{
    if (float_ == NULL)
    {
        return;
    }
    free(float_);
}

void dispose_assignment(Assignment *assignment)
{
    if (assignment == NULL)
    {
        return;
    }
    dispose_type_info(assignment->type_info);
    dispose_expression(assignment->expression);
    free(assignment->name);
    free(assignment);
}

void dispose_call(Call *call)
{
    if (call == NULL)
    {
        return;
    }
    dispose_expression(call->expression);
    dispose_type_info(call->type_info);
    free(call->name);
    free(call);
}

void dispose_function(Function *function)
{
    if (function == NULL)
    {
        return;
    }
    dispose_variable(function->params);
    dispose_block(function->body);
    dispose_type_info(function->type_info);
    free(function->name);
    free(function);
}

void dispose_return(Return *return_)
{
    if (return_ == NULL)
    {
        return;
    }
    dispose_expression(return_->expression);
    free(return_);
}

void dispose_if(If *if_)
{
    if (if_ == NULL)
    {
        return;
    }
    dispose_if(if_->next);
    dispose_expression(if_->condition);
    dispose_block(if_->body);
    free(if_);
}
void dispose_while(While *while_)
{
    if (while_ == NULL)
    {
        return;
    }
    dispose_expression(while_->condition);
    dispose_block(while_->body);
    free(while_);
}
void dispose_name(Name *name)
{
    free(name->value);
    free(name);
}

void dispose_one(Node *node)
{
    if (!node)
    {
        return;
    }

    switch (node->node_type)
    {
    case N_NAME:
        dispose_name((Name *)node->data);
        break;
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
    if (node == NULL)
    {
        return;
    }
    dispose_node(node->next);
    dispose_one(node);
}
