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

typedef enum
{
  T_NOMATCH = 0,
  T_EOF = 1 << 0,
  T_SPACE = 1 << 1,
  T_COMMENT = 1 << 2,
  T_LPAREN = 1 << 3,
  T_RPAREN = 1 << 4,
  T_NAME = 1 << 5,
  T_ADD = 1 << 6,
  T_SUB = 1 << 7,
  T_MUL = 1 << 8,
  T_DIV = 1 << 9,
  T_ASSIGN = 1 << 10,
  T_SEMICOLON = 1 << 11,
  T_NUMBER = 1 << 12,
  T_STRING = 1 << 13
} TokenType;

typedef struct Token
{
  int tokenType;
  char *buf;
  int len;
} Token;

Token *new_token(TokenType type, char *buf, int len);
void destroy_token(Token *token);
void print_token(Token *token);

#endif
