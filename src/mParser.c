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

Node *newNode(Token *token)
{
    Node *node = malloc(sizeof(Node));
    node->value = token;
    return node;
}

void printNode(Node *node)
{
    printToken(node->value);
    Node *sub = node->sub;
    while (sub)
    {
        printToken(sub->value);
        sub = sub->next;
    }
}

Node *nextNode(LexState *s)
{
    Token *token = lex(s);
    while (token->type == T_WHITESPACE || token->type == T_COMMENT)
    {
        token = lex(s);
    }
    return newNode(token);
}

void destroyNode(Node *node)
{
    if (!node)
    {
        return;
    }
    destroyNode(node->sub);
    destroyNode(node->next);
    destroyToken(node->value);
    free(node);
}

Node *parse(LexState *s)
{
    Node *node = nextNode(s);

    if (node->value->type == T_LPAREN)
    {
        node = nextNode(s);
        Node *sub = parse(s);
        while (sub->value->type != T_RPAREN)
        {
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
            sub = parse(s);
        }
    }

    return node;
}