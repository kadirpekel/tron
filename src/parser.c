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
    ps->scope = new_scope(NULL);
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

Node *parse_type_infos(ParserState *ps)
{
    Node *node = NULL;
    Token *type_token;
    if ((type_token = accept_token(ps, T_TYPE)) != NULL)
    {
        node = new_type_info(type_token->type);
        destroy_token(type_token);
    }

    return node;
}

Node *parse_call(ParserState *ps, Symbol *symbol)
{
    Node *node = NULL;
    Token *lparen_token = NULL;

    if ((lparen_token = accept_token(ps, T_LPAREN)) != NULL)
    {
        Node *arguments = parse_expression(ps);

        Token *commaToken;
        while ((commaToken = accept_token(ps, T_COMMA)) != NULL)
        {
            arguments = parse_expression(ps);
            destroy_token(commaToken);
        }

        node = new_call(symbol, arguments);
        destroy_token(expect_token(ps, T_RPAREN));
        destroy_token(lparen_token);
    }
    return node;
}

Node *parse_expression(ParserState *ps)
{
    Node *left = parse_term(ps);
    Token *opToken;
    if ((opToken = accept_token(ps, T_ADD | T_SUB)) != NULL)
    {
        Node *right = parse_expression(ps);
        Node *node = new_expression(opToken->buffer, left, right);
        destroy_token(opToken);
        return node;
    }
    return left;
}

Node *parse_term(ParserState *ps)
{
    Node *left = parse_factor(ps);
    Token *opToken;
    if ((opToken = accept_token(ps, T_MUL | T_DIV)) != NULL)
    {
        Node *node = new_expression(opToken->buffer, left, parse_term(ps));
        destroy_token(opToken);
        return node;
    }
    return left;
}

Node *parse_factor(ParserState *ps)
{
    if (ps->token->token_type == T_LPAREN)
    {
        destroy_token(expect_token(ps, T_LPAREN));
        Node *node = parse_expression(ps);
        destroy_token(expect_token(ps, T_RPAREN));
        return node;
    }
    else
    {
        Node *node = NULL;
        Token *leaf_token;
        if ((leaf_token = accept_token(ps, T_NUMBER | T_NAME)) != NULL)
        {
            if (leaf_token->token_type == T_NUMBER)
            {
                node = new_number(atoi(leaf_token->buffer));
            }
            else if (leaf_token->token_type == T_NAME)
            {
                Symbol *symbol = lookup_symbol(ps->scope, leaf_token->buffer);
                if (symbol != NULL)
                {
                    if (symbol->symbol_type == SYMBOL_FUNCTION)
                    {
                        node = parse_call(ps, symbol);
                    }
                    else if (symbol->symbol_type == SYMBOL_VARIABLE)
                    {
                        node = new_name(leaf_token->buffer);
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
        return node;
    }
}

Node *parse_param(ParserState *ps)
{
    Node *node = NULL;
    Token *name_token;

    if ((name_token = accept_token(ps, T_NAME)) != NULL)
    {
        Node *type_info = NULL;
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

        Token *assign_token = NULL;
        Node *expression = NULL;
        if ((assign_token = accept_token(ps, T_ASSIGN)) != NULL)
        {
            expression = parse_expression(ps);
            destroy_token(assign_token);
        }

        if (type_info == NULL)
        {
            type_info = new_type_info(TYPE_INFER);
        }
        node = new_variable(name_token->buffer, type_info, expression);
        Variable *variable = (Variable *)node->data;
        insert_symbol(ps->scope, variable->name, SYMBOL_VARIABLE, variable->type_info->data);
        destroy_token(name_token);
    }
    return node;
}

Node *parse_variable(ParserState *ps)
{
    Node *node = NULL;
    Token *var_token;
    if ((var_token = accept_keyword(ps, "var")) != NULL)
    {
        node = parse_param(ps);
        if (node == NULL)
        {
            parse_error(ps, "Variable not initialized");
        }
        destroy_token(expect_token(ps, T_SEMICOLON));
    }
    return node;
}

Node *parse_return(ParserState *ps)
{
    Node *node = NULL;
    Token *return_token;
    if ((return_token = accept_keyword(ps, "return")) != NULL)
    {
        node = new_return(parse_expression(ps));
        destroy_token(expect_token(ps, T_SEMICOLON));
        destroy_token(return_token);
    }
    return node;
}

Node *parse_inner_block(ParserState *ps)
{
    Node *node = NULL;

    if ((node = parse_return(ps)) != NULL)
        ;
    else if ((node = parse_statement(ps)) != NULL)
        ;
    return node;
}

Node *parse_block(ParserState *ps)
{
    destroy_token(expect_token(ps, T_LCBRACET));
    Node *node = NULL;
    Node *statement = parse_inner_block(ps);
    Node *current = statement;
    while (current != NULL)
    {
        current->next = parse_inner_block(ps);
        current = current->next;
    }
    node = new_block(statement);
    destroy_token(expect_token(ps, T_RCBRACET));
    return node;
}

Node *parse_parameters(ParserState *ps)
{
    Node *param = parse_param(ps);
    Node *current = param;
    Token *comma_token = accept_token(ps, T_COMMA);
    while (comma_token != NULL)
    {
        current->next = parse_param(ps);
        current = current->next;
        comma_token = accept_token(ps, T_COMMA);
    }
    return param;
}

Node *parse_function(ParserState *ps)
{
    Node *node = NULL;
    Token *def_token;

    if ((def_token = accept_keyword(ps, "def")) != NULL)
    {
        Token *name_token = expect_token(ps, T_NAME);

        destroy_token(expect_token(ps, T_LPAREN));
        Node *parameters = parse_parameters(ps);
        destroy_token(expect_token(ps, T_RPAREN));

        Node *type_info = NULL;
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
        Scope *parent = ps->scope;
        ps->scope = new_scope(parent);
        Node *body = parse_block(ps);
        ps->scope = parent;
        if (type_info == NULL)
        {
            type_info = new_type_info(TYPE_INFER);
        }
        node = new_function(name_token->buffer, type_info, parameters, body);
        Function *function = (Function *)node->data;
        insert_symbol(ps->scope, function->name, SYMBOL_FUNCTION, function->type_info->data);
        destroy_token(name_token);
        destroy_token(def_token);
    }
    return node;
}

Node *parse_assignment(ParserState *ps, Symbol *symbol)
{
    Node *node = NULL;
    Token *assign_token = NULL;
    if ((assign_token = accept_token(ps, T_ASSIGN)) != NULL)
    {
        Node *expression = parse_expression(ps);
        if (expression == NULL)
        {
            parse_error(ps, "Expression required");
        }
        node = new_assignment(symbol, expression);
        destroy_token(assign_token);
    }
    return node;
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
                node = parse_assignment(ps, symbol);
            }
            else if (symbol->symbol_type == SYMBOL_FUNCTION)
            {
                node = parse_call(ps, symbol);
            }
        }
        else
        {
            parse_error(ps, "Symbol not found");
        }

        destroy_token(expect_token(ps, T_SEMICOLON));
        destroy_token(name_token);
    }
    return node;
}

Node *parse_statement(ParserState *ps)
{
    Node *node;

    if ((node = parse_variable(ps)) != NULL)
        ;
    else if ((node = parse_function(ps)) != NULL)
        ;
    else if ((node = parse_namebiguity(ps)) != NULL)
        ;

    return node;
}

Node *parse(ParserState *ps)
{
    return parse_statement(ps);
}