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
#include "type.h"

typedef enum
{
  T_EOF = 1 << 0,
  T_SPACE = 1 << 1,
  T_LPAREN = 1 << 2,
  T_RPAREN = 1 << 3,
  T_LBRACKET = 1 << 4,
  T_RBRACKET = 1 << 5,
  T_DOT = 1 << 6,
  T_COMMA = 1 << 7,
  T_COLON = 1 << 8,
  T_SHORT_ASSIGN = 1 << 9,
  T_SEMICOLON = 1 << 10,
  T_ADD = 1 << 11,
  T_ADD_ASSIGN = 1 << 12,
  T_SUB = 1 << 13,
  T_SUB_ASSIGN = 1 << 14,
  T_ARROW = 1 << 15,
  T_MUL = 1 << 16,
  T_MUL_ASSIGN = 1 << 17,
  T_DIV = 1 << 18,
  T_DIV_ASSIGN = 1 << 19,
  T_MOD = 1 << 20,
  T_MOD_ASSIGN = 1 << 21,
  T_AND_ASSIGN = 1 << 22,
  T_BIT_CLEAR = 1 << 23,
  T_BIT_CLEAR_ASSIGN = 1 << 24,
  T_LOGICAL_AND = 1 << 25,
  T_BITWISE_AND = 1 << 26,
  T_OR_ASSIGN = 1 << 27,
  T_LOGICAL_OR = 1 << 28,
  T_BITWISE_OR = 1 << 29,
  T_XOR_ASSIGN = 1 << 30,
  T_BITWISE_XOR = 1 << 31,
  T_LTE = 1 << 32,
  T_LSHIFT = 1 << 33,
  T_LSHIFT_ASSIGN = 1 << 34,
  T_CHAN_RECEIVE = 1 << 35,
  T_LT = 1 << 36,
  T_GTE = 1 << 37,
  T_RSHIFT = 1 << 38,
  T_RSHIFT_ASSIGN = 1 << 39,
  T_GT = 1 << 40,
  T_EQ = 1 << 41,
  T_ASSIGN = 1 << 42,
  T_NEQ = 1 << 43,
  T_NOT = 1 << 44,
  T_COMMENT = 1 << 45,
  T_NAME = 1 << 46,
  T_INTEGER = 1 << 47,
  T_FLOAT = 1 << 48,
  T_STRING = 1 << 49,
  T_LBRACE = 1 << 50,
  T_RBRACE = 1 << 51,
} TokenType;

typedef struct Token
{
  TokenType token_type;
  Type type;
  char *buffer;
  int length;
} Token;

Token *new_token(TokenType type, char *buf, int len);
void destroy_token(Token *token);
void print_token(Token *token);

#endif
