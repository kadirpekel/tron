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

#include "mParser.h"

void nextToken(ParserState *ps)
{
    Token *token = lex(ps->ls);
    while ((token->tokenType & (T_SPACE | T_COMMENT)) != 0)
    {
        token = lex(ps->ls);
    }
    ps->token = token;
}

void initParser(ParserState *ps, LexState *ls)
{
    ps->ls = ls;
    nextToken(ps);
}

Node *expect(ParserState *ps, int tokenTypes)
{
    if ((ps->token->tokenType & tokenTypes) != 0)
    {
        Node *node = newNode(ps->token);
        nextToken(ps);
        return node;
    }

    fprintf(stderr, "Syntax error: Unexpected token '%.*s' on line %d, column %d\n",
            ps->token->len,
            ps->token->buf,
            ps->ls->line,
            ps->ls->col);
    exit(EXIT_FAILURE);
}

Node *parse(ParserState *ps)
{
    if (ps->token->tokenType == T_LPAREN)
    {
        expect(ps, T_LPAREN);
        Node *node = expect(ps, T_NAME);

        while ((ps->token->tokenType & (T_RPAREN | T_NOMATCH | T_EOF)) == 0)
        {
            Node *sub = parse(ps);
            if (node->sub)
            {
                Node *curr = node->sub;
                while (curr->next)
                {
                    curr = sub->next;
                }
                curr->next = sub;
            }
            else
            {
                node->sub = sub;
            }
        }
        expect(ps, T_RPAREN);
        return node;
    }
    else
    {
        return expect(ps, T_NAME | T_INT | T_STRING | T_EOF);
    }
}