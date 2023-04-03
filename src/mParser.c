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

#include "mParser.h"

void nextToken(ParserState *ps)
{
    Token *token = lex(ps->ls);
    while ((token->tokenType & (T_SPACE | T_COMMENT)) != 0)
    {
        destroyToken(token);
        token = lex(ps->ls);
    }
    ps->token = token;
}

void initParser(ParserState *ps, LexState *ls)
{
    ps->ls = ls;
    nextToken(ps);
}

void parseError(ParserState *ps, char *msg)
{
    fprintf(stderr, "Syntax error: %s: '%.*s' on line %d, column %d\n",
            msg,
            ps->token->len,
            ps->token->buf,
            ps->ls->line,
            ps->ls->col);
    exit(EXIT_FAILURE);
}

Token *acceptToken(ParserState *ps, TokenType tokenTypes)
{
    Token *token = NULL;
    if ((ps->token->tokenType & tokenTypes) != 0)
    {
        token = ps->token;
        nextToken(ps);
    }
    return token;
}

Token *expectToken(ParserState *ps, TokenType tokenTypes)
{
    Token *token;
    if ((token = acceptToken(ps, tokenTypes)) != NULL)
    {
        return token;
    }
    parseError(ps, "Unexpected token");
    return NULL;
}

Token *acceptKeyword(ParserState *ps, char *keyword)
{
    Token *token = NULL;
    if ((ps->token->tokenType & T_NAME) != 0 && strcmp(ps->token->buf, keyword) == 0)
    {
        token = acceptToken(ps, T_NAME);
    }
    return token;
}

Node *parseExpression(ParserState *ps)
{
    Node *left = parseTerm(ps);
    Token *opToken;
    if ((opToken = acceptToken(ps, T_ADD | T_SUB)) != NULL)
    {
        Node *right = parseExpression(ps);
        Node *node = newExpression(opToken->buf, left, right);
        destroyToken(opToken);
        return node;
    }
    return left;
}

Node *parseTerm(ParserState *ps)
{
    Node *left = parseFactor(ps);
    Token *opToken;
    if ((opToken = acceptToken(ps, T_MUL | T_DIV)) != NULL)
    {
        Node *node = newExpression(opToken->buf, left, parseTerm(ps));
        destroyToken(opToken);
        return node;
    }
    return left;
}

Node *parseFactor(ParserState *ps)
{
    if (ps->token->tokenType == T_LPAREN)
    {
        destroyToken(expectToken(ps, T_LPAREN));
        Node *node = parseExpression(ps);
        destroyToken(expectToken(ps, T_RPAREN));
        return node;
    }
    else
    {
        Node *node = NULL;
        Token *leafToken = expectToken(ps, T_NUMBER | T_NAME);
        if (leafToken->tokenType == T_NUMBER)
        {
            node = newNumber(atoi(leafToken->buf));
        }
        else if (leafToken->tokenType == T_NAME)
        {
            node = newName(leafToken->buf);
        }
        destroyToken(leafToken);
        return node;
    }
}

Node *parseAssignment(ParserState *ps)
{
    Node *node = NULL;
    Token *var;
    if ((var = acceptKeyword(ps, "var")) != NULL)
    {
        Token *nameToken = expectToken(ps, T_NAME);
        destroyToken(expectToken(ps, T_ASSIGN));
        Node *expression = parseExpression(ps);
        destroyToken(expectToken(ps, T_SEMICOLON));
        node = newAssignment(nameToken->buf, expression);
        destroyToken(nameToken);
        destroyToken(var);
    }
    return node;
}

Node *parseStatement(ParserState *ps)
{
    return parseAssignment(ps);
}

Node *parse(ParserState *ps)
{
    return parseStatement(ps);
}