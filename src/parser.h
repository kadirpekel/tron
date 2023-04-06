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
#include "scope.h"

#define IF "if"
#define WHILE "while"
#define FUNCTION "func"
#define VAR "var"
#define RETURN "return"

typedef struct ParserState
{
    LexState *ls;
    Scope *scope;
    Token *token;
} ParserState;

void init_parser(ParserState *ps, LexState *ls);

Expression *parse_term(ParserState *ps);
Expression *parse_factor(ParserState *ps);
Expression *parse_expression(ParserState *ps);
Node *parse_statement(ParserState *ps);
Node *parse(ParserState *ps, void (*visit_node)(Node *));

#endif
