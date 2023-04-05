/******************************************************************************
 * Copyright [2023] [Kadir PEKEL]
 *
 * Licensed under the Apache License, Version 2.0 (the "License\n");
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
#include <stdbool.h>

#include "parser.h"

void next_token(ParserState *ps)
{
    Token *token = lex(ps->ls);
    while ((token->token_type & (T_SPACE | T_COMMENT)) != 0)
    {
        destroy_token(token);
        token = lex(ps->ls);
    }
    ps->token = token;
}

void init_parser(ParserState *ps, LexState *ls)
{
    ps->ls = ls;
    ps->scope = new_scope(NULL, NULL);
    next_token(ps);
}

void parse_error(ParserState *ps, char *msg)
{
    fprintf(stderr, "Syntax Error <%d:%d> %s\n",
            ps->ls->line,
            ps->ls->col,
            msg);
    exit(EXIT_FAILURE);
}

Token *accept_token(ParserState *ps, TokenType tokenTypes)
{
    Token *token = NULL;
    if ((ps->token->token_type & tokenTypes) != 0)
    {
        token = ps->token;
        next_token(ps);
    }
    return token;
}

Token *expect_token(ParserState *ps, TokenType tokenTypes)
{
    Token *token;
    if ((token = accept_token(ps, tokenTypes)) != NULL)
    {
        return token;
    }
    parse_error(ps, "Unexpected token");
    return NULL;
}

Token *accept_keyword(ParserState *ps, char *keyword)
{
    Token *token = NULL;
    if ((ps->token->token_type & T_NAME) != 0 && strcmp(ps->token->buffer, keyword) == 0)
    {
        token = accept_token(ps, T_NAME);
    }
    return token;
}

Symbol *accept_type(ParserState *ps)
{
    if ((ps->token->token_type & T_NAME) != 0)
    {
        Symbol *symbol = lookup_symbol(ps->scope, ps->token->buffer);
        if (symbol != NULL && symbol->symbol_type == SYMBOL_TYPE)
        {

            accept_token(ps, T_NAME);
            return symbol;
        }
    }
    return NULL;
}

TypeInfo *parse_type_info(ParserState *ps)
{
    TypeInfo *type_info = NULL;
    Symbol *symbol;
    if ((symbol = accept_type(ps)) != NULL)
    {
        type_info = new_type_info(symbol->type_info->type);
    }
    return type_info;
}

TypeInfo *parse_type_infos(ParserState *ps)
{
    TypeInfo *type_info = NULL;
    Token *lparen_token = NULL;

    if ((lparen_token = accept_token(ps, T_LPAREN)) != NULL)
    {
        type_info = parse_type_info(ps);
        if (type_info == NULL)
        {
            parse_error(ps, "Type info is missing");
        }

        Token *commaToken;
        TypeInfo *current = type_info;
        while ((commaToken = accept_token(ps, T_COMMA)) != NULL)
        {
            TypeInfo *next = parse_type_info(ps);
            if (next == NULL)
            {
                parse_error(ps, "Type info is missing");
            }
            current->next = next;
            current = current->next;
            destroy_token(commaToken);
        }

        destroy_token(expect_token(ps, T_RPAREN));
        destroy_token(lparen_token);
    }
    else
    {
        type_info = parse_type_info(ps);
    }

    return type_info;
}

Call *parse_call(ParserState *ps, Symbol *symbol)
{
    Call *call = NULL;
    Token *lparen_token = NULL;

    if ((lparen_token = accept_token(ps, T_LPAREN)) != NULL)
    {
        Expression *expression = parse_expression(ps);

        Token *commaToken;
        Expression *current = expression;
        while ((commaToken = accept_token(ps, T_COMMA)) != NULL)
        {
            Expression *next = parse_expression(ps);
            if (next == NULL)
            {
                parse_error(ps, "Expression is missing");
            }
            current->next = next;
            current = next;
            destroy_token(commaToken);
        }

        call = new_call(symbol->name, symbol->type_info, expression);
        destroy_token(expect_token(ps, T_RPAREN));
        destroy_token(lparen_token);
    }
    return call;
}

Expression *parse_expression(ParserState *ps)
{
    Expression *left = parse_term(ps);
    Token *opToken;
    if ((opToken = accept_token(ps, T_ADD | T_SUB)) != NULL)
    {
        Expression *right = parse_expression(ps);
        Expression *expresion = new_expression(opToken->buffer, left, right, L_NA, NULL);
        destroy_token(opToken);
        return expresion;
    }
    return left;
}

Expression *parse_expressions(ParserState *ps)
{
    Expression *expression = parse_expression(ps);

    if (expression != NULL)
    {
        Token *commaToken;
        Expression *current = expression;
        while ((commaToken = accept_token(ps, T_COMMA)) != NULL)
        {
            Expression *next = parse_expression(ps);
            if (next == NULL)
            {
                parse_error(ps, "Expression is missing");
            }
            current->next = next;
            current = current->next;
            destroy_token(commaToken);
        }
    }

    return expression;
}

Expression *parse_term(ParserState *ps)
{
    Expression *left = parse_factor(ps);
    Token *opToken;
    if ((opToken = accept_token(ps, T_MUL | T_DIV)) != NULL)
    {
        Expression *expression = new_expression(opToken->buffer, left, parse_term(ps), L_NA, NULL);
        destroy_token(opToken);
        return expression;
    }
    return left;
}

Expression *parse_factor(ParserState *ps)
{
    Expression *expression = NULL;

    if (ps->token->token_type == T_LPAREN)
    {
        destroy_token(expect_token(ps, T_LPAREN));
        expression = parse_expression(ps);
        destroy_token(expect_token(ps, T_RPAREN));
    }
    else
    {
        Token *leaf_token;
        if ((leaf_token = accept_token(ps, T_INTEGER | T_FLOAT | T_NAME)) != NULL)
        {
            if (leaf_token->token_type == T_INTEGER)
            {
                expression = new_expression(leaf_token->buffer, NULL, NULL, L_INTEGER, new_integer(atoi(leaf_token->buffer)));
            }
            else if (leaf_token->token_type == T_FLOAT)
            {
                expression = new_expression(leaf_token->buffer, NULL, NULL, L_FLOAT, new_float(atof(leaf_token->buffer)));
            }
            else if (leaf_token->token_type == T_NAME)
            {
                Symbol *symbol = lookup_symbol(ps->scope, leaf_token->buffer);
                if (symbol != NULL)
                {
                    if (symbol->symbol_type == SYMBOL_FUNCTION)
                    {
                        Call *call = parse_call(ps, symbol);
                        if (call == NULL)
                        {
                            parse_error(ps, "Function call missing");
                        }
                        expression = new_expression(leaf_token->buffer, NULL, NULL, L_CALL, call);
                    }
                    else if (symbol->symbol_type == SYMBOL_VARIABLE)
                    {
                        expression = new_expression(leaf_token->buffer, NULL, NULL, L_NAME, new_name(leaf_token->buffer));
                    }
                    else
                    {
                        parse_error(ps, "Invalid symbol found");
                    }
                }
                else
                {
                    parse_error(ps, "Symbol not found");
                }
            }
            destroy_token(leaf_token);
        }
    }
    return expression;
}

Variable *parse_param(ParserState *ps)
{
    Variable *param = NULL;
    Token *name_token;

    if ((name_token = accept_token(ps, T_NAME)) != NULL)
    {
        TypeInfo *type_info = NULL;
        Expression *expression = NULL;

        Token *colon_token;
        if ((colon_token = accept_token(ps, T_COLON)) != NULL)
        {
            type_info = parse_type_info(ps);

            if (type_info == NULL)
            {
                parse_error(ps, "Type info is missing");
            }
            destroy_token(colon_token);
        }
        else
        {
            type_info = new_type_info(TYPE_INFER);
        }

        Token *assign_token;
        if ((assign_token = accept_token(ps, T_ASSIGN)) != NULL)
        {
            expression = parse_expression(ps);
            if (expression == NULL)
            {
                parse_error(ps, "Assignment requires expression");
            }
            destroy_token(assign_token);
        }

        param = new_variable(name_token->buffer, type_info, expression);
        insert_symbol(ps->scope, param->name, SYMBOL_VARIABLE, param->type_info);
        destroy_token(name_token);
    }
    return param;
}

Variable *parse_variable(ParserState *ps)
{
    Variable *param = NULL;
    Token *var_token;
    if ((var_token = accept_keyword(ps, "var")) != NULL)
    {
        param = parse_param(ps);
        if (param == NULL)
        {
            parse_error(ps, "Variable not initialized");
        }
        destroy_token(expect_token(ps, T_SEMICOLON));
    }
    return param;
}

Return *parse_return(ParserState *ps)
{
    Return *return_ = NULL;
    Token *return_token;
    if ((return_token = accept_keyword(ps, "return")) != NULL)
    {
        return_ = new_return(parse_expressions(ps));
        destroy_token(expect_token(ps, T_SEMICOLON));
        destroy_token(return_token);
    }
    return return_;
}

Block *parse_block(ParserState *ps)
{
    destroy_token(expect_token(ps, T_LCBRACET));
    Node *statement = parse(ps, NULL);
    Node *current = statement;
    while (current != NULL)
    {
        current->next = parse(ps, NULL);
        current = current->next;
    }
    Block *block = new_block(statement);
    destroy_token(expect_token(ps, T_RCBRACET));
    return block;
}

Variable *parse_params(ParserState *ps)
{
    Variable *param = parse_param(ps);
    Variable *current = param;
    Token *comma_token = accept_token(ps, T_COMMA);
    while (comma_token != NULL)
    {
        current->next = parse_param(ps);
        current = current->next;
        comma_token = accept_token(ps, T_COMMA);
    }
    return param;
}

Function *parse_function(ParserState *ps)
{
    Function *function = NULL;
    Token *def_token;

    if ((def_token = accept_keyword(ps, "def")) != NULL)
    {
        Token *name_token = expect_token(ps, T_NAME);

        destroy_token(expect_token(ps, T_LPAREN));
        Variable *params = parse_params(ps);
        destroy_token(expect_token(ps, T_RPAREN));

        TypeInfo *type_info = NULL;
        Token *colon_token = NULL;
        if ((colon_token = accept_token(ps, T_COLON)) != NULL)
        {
            type_info = parse_type_infos(ps);
            if (type_info == NULL)
            {
                parse_error(ps, "Type info is missing");
            }
            destroy_token(colon_token);
        }

        if (type_info == NULL)
        {
            type_info = new_type_info(TYPE_INFER);
        }

        function = new_function(name_token->buffer, type_info, params, NULL);

        Scope *parent = ps->scope;
        ps->scope = new_scope(parent, function);
        function->body = parse_block(ps);
        ps->scope = parent;

        if (function->body == NULL)
        {
            parse_error(ps, "Function body is missing");
        }

        insert_symbol(ps->scope, function->name, SYMBOL_FUNCTION, function->type_info);
        destroy_token(name_token);
        destroy_token(def_token);
    }
    return function;
}

Assignment *parse_assignment(ParserState *ps, Symbol *symbol)
{
    Assignment *assignment = NULL;
    Token *assign_token = NULL;
    if ((assign_token = accept_token(ps, T_ASSIGN)) != NULL)
    {
        Expression *expression = parse_expression(ps);
        if (expression == NULL)
        {
            parse_error(ps, "Expression required");
        }
        assignment = new_assignment(symbol->name, symbol->type_info, expression);
        destroy_token(assign_token);
    }
    return assignment;
}

Node *parse_namebiguity(ParserState *ps)
{
    Node *node = NULL;
    Token *name_token;

    if ((name_token = accept_token(ps, T_NAME)) != NULL)
    {
        Symbol *symbol = lookup_symbol(ps->scope, name_token->buffer);

        if (symbol != NULL)
        {
            if (symbol->symbol_type == SYMBOL_VARIABLE)
            {
                Assignment *assignment = parse_assignment(ps, symbol);
                if (assignment == NULL)
                {
                    parse_error(ps, "Variable assignment missing");
                }
                destroy_token(expect_token(ps, T_SEMICOLON));
                node = new_node(N_ASSIGNMENT, assignment);
            }
            else if (symbol->symbol_type == SYMBOL_FUNCTION)
            {
                Call *call = parse_call(ps, symbol);
                if (call == NULL)
                {
                    parse_error(ps, "Function call missing");
                }
                destroy_token(expect_token(ps, T_SEMICOLON));
                node = new_node(N_CALL, call);
            }
        }
        else
        {
            parse_error(ps, "Symbol not found");
        }

        destroy_token(name_token);
    }
    return node;
}

Node *parse_statement(ParserState *ps)
{

    Function *function = parse_function(ps);
    if (function != NULL)
    {
        return new_node(N_FUNCTION, function);
    }

    if (ps->scope->function != NULL)
    {
        Return *return_ = parse_return(ps);
        if (return_ != NULL)
        {
            return new_node(N_RETURN, return_);
        }
    }

    Variable *variable = parse_variable(ps);
    if (variable != NULL)
    {
        return new_node(N_VARIABLE, variable);
    }

    Node *node = parse_namebiguity(ps);
    if (node != NULL)
    {
        return node;
    }

    return NULL;
}

Node *parse(ParserState *ps, void (*visit_node)(Node *))
{

    Node *node = parse_statement(ps);

    Node *current = node;
    while (current)
    {
        if (visit_node != NULL)
        {
            visit_node(current);
        }
        current->next = parse_statement(ps);
        current = current->next;
    }

    return node;
}