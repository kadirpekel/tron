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

#ifndef MLEXER_H_
#define MLEXER_H_

#include <stdio.h>
#include "constants.h"
#include "token.h"

typedef struct LexState
{
  FILE *file;
  char buffer[MAX_BUFFER_SIZE];
  int length;
  int token_type;
  char c;
  int line;
  int col;
} LexState;

void init_lex_state(LexState *ls, FILE *file);
Token *lex(LexState *ls);

#endif