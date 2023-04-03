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

#include "mAssert.h"
#include "mLexer.h"
#include "mNode.h"

typedef struct ParserState
{
    LexState *ls;
    Token *token;
} ParserState;

void initParser(ParserState *ps, LexState *ls);
Node *parse(ParserState *ps);

Node *parseTerm(ParserState *ps);
Node *parseFactor(ParserState *ps);
Node *parseExpression(ParserState *ps);

#endif
