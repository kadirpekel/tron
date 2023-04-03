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

#ifndef MPARSER_H_
#define MPARSER_H_

#include <stdio.h>

#include "assert.h"
#include "lexer.h"
#include "node.h"

typedef struct ParserState
{
    LexState *ls;
    Token *token;
} ParserState;

void init_parser(ParserState *ps, LexState *ls);
Node *parse(ParserState *ps);

Node *parse_term(ParserState *ps);
Node *parse_factor(ParserState *ps);
Node *parse_expression(ParserState *ps);

#endif