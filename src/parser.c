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
    while ((token->tokenType & (T_SPACE | T_COMMENT)) != 0)
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
            ps->token->len,
            ps->token->buf,
            ps->ls->line,
            ps->ls->col);
    exit(EXIT_FAILURE);
}

Token *accept_token(ParserState *ps, TokenType tokenTypes)
{
    Token *token = NULL;
    if ((ps->token->tokenType & tokenTypes) != 0)
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
    if ((ps->token->tokenType & T_NAME) != 0 && strcmp(ps->token->buf, keyword) == 0)
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
        Node *node = new_expression(opToken->buf, left, right);
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
        Node *node = new_expression(opToken->buf, left, parse_term(ps));
        destroy_token(opToken);
        return node;
    }
    return left;
}

Node *parse_factor(ParserState *ps)
{
    if (ps->token->tokenType == T_LPAREN)
    {
        destroy_token(expect_token(ps, T_LPAREN));
        Node *node = parse_expression(ps);
        destroy_token(expect_token(ps, T_RPAREN));
        return node;
    }
    else
    {
        Node *node = NULL;
        Token *leafToken = expect_token(ps, T_NUMBER | T_NAME);
        if (leafToken->tokenType == T_NUMBER)
        {
            node = new_number(atoi(leafToken->buf));
        }
        else if (leafToken->tokenType == T_NAME)
        {
            node = new_name(leafToken->buf);
        }
        destroy_token(leafToken);
        return node;
    }
}

Node *parse_variable(ParserState *ps)
{
    Node *node = NULL;
    Token *var;
    if ((var = accept_keyword(ps, "var")) != NULL)
    {
        Token *nameToken = expect_token(ps, T_NAME);
        destroy_token(expect_token(ps, T_ASSIGN));
        Node *expression = parse_expression(ps);
        destroy_token(expect_token(ps, T_SEMICOLON));
        node = new_variable(nameToken->buf, expression);
        destroy_token(nameToken);
        destroy_token(var);
    }
    return node;
}

Node *parse_statement(ParserState *ps)
{
    return parse_variable(ps);
}

Node *parse(ParserState *ps)
{
    return parse_statement(ps);
}