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
    next_token(ps);
}

void parse_error(ParserState *ps, char *msg)
{
    fprintf(stderr, "Syntax error: %s: '%.*s' on line %d, column %d\n",
            msg,
            ps->token->length,
            ps->token->buffer,
            ps->ls->line,
            ps->ls->col);
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
        Token *leaf_token = expect_token(ps, T_NUMBER | T_NAME);
        if (leaf_token->token_type == T_NUMBER)
        {
            node = new_number(atoi(leaf_token->buffer));
        }
        else if (leaf_token->token_type == T_NAME)
        {
            node = new_name(leaf_token->buffer);
        }
        destroy_token(leaf_token);
        return node;
    }
}

Node *parse_variable(ParserState *ps)
{
    Node *node = NULL;
    Token *var_token;
    if ((var_token = accept_keyword(ps, "var")) != NULL)
    {
        Token *name_token = expect_token(ps, T_NAME);

        Type type = TYPE_INFER;
        Token *colon_token = NULL;
        if ((colon_token = accept_token(ps, T_COLON)) != NULL)
        {
            Token *type_token = expect_token(ps, T_TYPE);
            type = type_token->type;
            destroy_token(type_token);
            destroy_token(colon_token);
        }

        Token *assign_token = NULL;
        Node *expression = NULL;
        if ((assign_token = accept_token(ps, T_ASSIGN)) != NULL)
        {
            expression = parse_expression(ps);
            destroy_token(assign_token);
        }

        destroy_token(expect_token(ps, T_SEMICOLON));
        node = new_variable(name_token->buffer, type, expression);
        destroy_token(name_token);
        destroy_token(var_token);
    }
    return node;
}

Node *parse_namebiguity(ParserState *ps)
{
    Node *node = NULL;
    Token *name_token;
    insert_symbol("foo", SYMBOL_FUNCTION, TYPE_INT);

    if ((name_token = accept_token(ps, T_NAME)) != NULL)
    {
        Symbol *symbol = lookup_symbol(name_token->buffer);
        if (symbol != NULL)
        {
            if (symbol->symbol_type == SYMBOL_VARIABLE)
            {
                Token *assign_token = NULL;
                Node *expression = NULL;
                if ((assign_token = accept_token(ps, T_ASSIGN)) != NULL)
                {
                    expression = parse_expression(ps);
                    destroy_token(assign_token);
                }
                destroy_token(expect_token(ps, T_SEMICOLON));
                node = new_assignment(symbol, expression);
            }
            else if (symbol->symbol_type == SYMBOL_FUNCTION)
            {
                Token *lparen_token = NULL;
                Node *arguments = NULL;
                if ((lparen_token = accept_token(ps, T_LPAREN)) != NULL)
                {
                    arguments = parse_expression(ps);

                    Token *commaToken;
                    while ((commaToken = accept_token(ps, T_COMMA)) != NULL)
                    {
                        arguments = parse_expression(ps);
                        destroy_token(commaToken);
                    }
                    destroy_token(expect_token(ps, T_RPAREN));
                    destroy_token(lparen_token);
                }

                destroy_token(expect_token(ps, T_SEMICOLON));
                node = new_call(symbol, arguments);
            }
        }

        destroy_token(name_token);

        if (node == NULL)
        {
            parse_error(ps, "Symbol not found");
        }
    }
    return node;
}

Node *parse_statement(ParserState *ps)
{
    Node *node;

    if ((node = parse_variable(ps)) != NULL)
        ;
    else if ((node = parse_namebiguity(ps)) != NULL)
        ;

    return node;
}

Node *parse(ParserState *ps)
{
    return parse_statement(ps);
}