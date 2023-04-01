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
  T_LBRAC = 1 << 5,
  T_RBRAC = 1 << 7,
  T_LCBRAC = 1 << 8,
  T_RCBRAC = 1 << 9,
  T_COLON = 1 << 10,
  T_NAME = 1 << 11,
  T_INT = 1 << 12,
  T_FLOAT = 1 << 13,
  T_CHAR = 1 << 14,
  T_STRING = 1 << 15,
} TokenType;

typedef enum
{
  K_VAR = 0,
  K_DEF = 1 << 0,
  K_IF = 1 << 1,
  K_ELIF = 1 << 2,
  K_ELSE = 1 << 3,
  K_FOR = 1 << 4,
  K_BREAK = 1 << 5,
  K_CONTINUE = 1 << 6,
  K_TYPE = 1 << 7,
  K_RETURN = 1 << 8,
} Keyword;

typedef enum
{
  N_INT = 0,
  N_FLOAT = 1 << 0,
  N_CHAR = 1 << 1,
  N_STRING = 1 << 2,
} TypeNames;

typedef struct Token
{
  int tokenType;
  char *buf;
  int len;
} Token;

Token *newToken(TokenType type, char *buf, int len);
void destroyToken(Token *token);
void printToken(Token *token);

#endif
