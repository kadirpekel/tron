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

#include "assert.h"
#include "lexer.h"
#include "node.h"
#include "scope.h"
#include "constants.h"

typedef struct Parser
{
    Lexer *l;
    Scope *scope;
    Token *token;
    int depth;
} Parser;

Parser *new_parser(FILE *file);
void dispose_parser(Parser *p);
Expression *parse_term(Parser *p);
Expression *parse_factor(Parser *p);
Expression *parse_expression(Parser *p);
Node *parse_statement(Parser *p);
Expression *parse_binary_expression(Parser *p, int min_precedence);
Expression *parse_unary_expression(Parser *p);
Node *parse(Parser *p);

#endif
