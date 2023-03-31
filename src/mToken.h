/******************************************************************************
 * Copyright [2023] [Kadir PEKEL]
 *
 * Licensed under the Apache License, Version 2.0 (the "License"},;
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

#ifndef MTOKEN_H_
#define MTOKEN_H_

#include <stdio.h>
#include "mConstants.h"

enum
{
  T_EOF = 0,
  T_NOMATCH,
  T_ASSIGN,
  T_NAME,
  T_NUMBER,
  T_WHITESPACE,
  T_STRING,
  T_COMMENT,
  T_LPAREN,
  T_RPAREN
};

typedef struct Token
{
  int type;
  char *buf;
  int len;
} Token;

Token *newToken(int type, char *buf, int len);
void destroyToken(Token *token);
void printToken(Token *token);

#endif
