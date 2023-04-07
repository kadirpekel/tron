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

/*--------------------------------
| Operator | Token Name           |
|----------|----------------------|
| (        | T_LPAREN             |
| )        | T_RPAREN             |
| [        | T_LBRACKET           |
| ]        | T_RBRACKET           |
| {        | T_LBRACE             |
| }        | T_RBRACE             |
| .        | T_DOT                |
| ,        | T_COMMA              |
| :        | T_COLON              |
| ;        | T_SEMICOLON          |
| ...      | T_ELLIPSIS           |
| +        | T_ADD                |
| -        | T_SUB                |
| *        | T_MUL                |
| /        | T_DIV                |
| %        | T_MOD                |
| &        | T_BITWISE_AND        |
| |        | T_BITWISE_OR         |
| ^        | T_BITWISE_XOR        |
| &^       | T_BIT_CLEAR          |
| <<       | T_LSHIFT             |
| >>       | T_RSHIFT             |
| ==       | T_EQ                 |
| !=       | T_NEQ                |
| <        | T_LT                 |
| <=       | T_LTE                |
| >        | T_GT                 |
| >=       | T_GTE                |
| &&       | T_LOGICAL_AND        |
| ||       | T_LOGICAL_OR         |
| <-       | T_CHAN_RECEIVE       |
| !        | T_NOT                |
| =        | T_ASSIGN             |
| :=       | T_SHORT_ASSIGN       |
| +=       | T_ADD_ASSIGN         |
| -=       | T_SUB_ASSIGN         |
| *=       | T_MUL_ASSIGN         |
| /=       | T_DIV_ASSIGN         |
| %=       | T_MOD_ASSIGN         |
| &=       | T_AND_ASSIGN         |
| |=       | T_OR_ASSIGN          |
| ^=       | T_XOR_ASSIGN         |
| <<=      | T_LSHIFT_ASSIGN      |
| >>=      | T_RSHIFT_ASSIGN      |
| &^=      | T_BIT_CLEAR_ASSIGN   |
----------------------------------*/

typedef enum
{
  T_NOMATCH,
  T_EOF,
  T_SPACE,
  T_LPAREN,
  T_RPAREN,
  T_LBRACKET,
  T_RBRACKET,
  T_DOT,
  T_COMMA,
  T_COLON,
  T_SHORT_ASSIGN,
  T_SEMICOLON,
  T_ADD,
  T_ADD_ASSIGN,
  T_SUB,
  T_SUB_ASSIGN,
  T_ARROW,
  T_MUL,
  T_MUL_ASSIGN,
  T_DIV,
  T_DIV_ASSIGN,
  T_MOD,
  T_MOD_ASSIGN,
  T_AND_ASSIGN,
  T_BIT_CLEAR,
  T_BIT_CLEAR_ASSIGN,
  T_LOGICAL_AND,
  T_BITWISE_AND,
  T_OR_ASSIGN,
  T_LOGICAL_OR,
  T_BITWISE_OR,
  T_XOR_ASSIGN,
  T_BITWISE_XOR,
  T_LTE,
  T_LSHIFT,
  T_LSHIFT_ASSIGN,
  T_CHAN_RECEIVE,
  T_LT,
  T_GTE,
  T_RSHIFT,
  T_RSHIFT_ASSIGN,
  T_GT,
  T_EQ,
  T_ASSIGN,
  T_NEQ,
  T_NOT,
  T_COMMENT,
  T_NAME,
  T_INTEGER,
  T_FLOAT,
  T_STRING,
  T_LBRACE,
  T_RBRACE,
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
