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

/*
+--------------------------------------------------------------------+
| Category            | Operator | Description                      |
+--------------------------------------------------------------------+
| Arithmetic          | +        | Addition                         |
|                     | -        | Subtraction                      |
|                     | *        | Multiplication                   |
|                     | /        | Division                         |
|                     | %        | Remainder                        |
+--------------------------------------------------------------------+
| Assignment          | =        | Assignment                       |
|                     | +=       | Addition assignment              |
|                     | -=       | Subtraction assignment           |
|                     | *=       | Multiplication assignment        |
|                     | /=       | Division assignment              |
|                     | %=       | Remainder assignment             |
|                     | &=       | Bitwise AND assignment           |
|                     | |=       | Bitwise OR assignment            |
|                     | ^=       | Bitwise XOR assignment           |
|                     | &^=      | Bitwise clear (AND NOT) assign.  |
|                     | <<=      | Left shift assignment            |
|                     | >>=      | Right shift assignment           |
+--------------------------------------------------------------------+
| Comparison          | ==       | Equal                            |
|                     | !=       | Not equal                        |
|                     | <        | Less than                        |
|                     | <=       | Less than or equal to            |
|                     | >        | Greater than                     |
|                     | >=       | Greater than or equal to         |
+--------------------------------------------------------------------+
| Logical             | &&       | Logical AND                      |
|                     | ||       | Logical OR                       |
|                     | !        | Logical NOT                      |
+--------------------------------------------------------------------+
| Bitwise             | &        | Bitwise AND                      |
|                     | |        | Bitwise OR                       |
|                     | ^        | Bitwise XOR                      |
|                     | &^       | Bitwise clear (AND NOT)          |
|                     | <<       | Left shift                       |
|                     | >>       | Right shift                      |
+--------------------------------------------------------------------+
| Unary               | &        | Address of                       |
|                     | *        | Dereference                      |
|                     | <-       | Channel send and receive         |
|                     | ++       | Increment                        |
|                     | --       | Decrement                        |
|                     | +        | Unary plus                       |
|                     | -        | Unary minus                      |
+--------------------------------------------------------------------+
*/

typedef enum TokenType
{
  T_NOMATCH = 0,
  T_EOF,

  // Arithmetic Operators
  T_ADD, // +
  T_SUB, // -
  T_MUL, // *
  T_DIV, // /
  T_REM, // %

  // Increment and Decrement Operators
  T_INC, // ++
  T_DEC, // --

  // Bitwise Operators
  T_AND,       // &
  T_OR,        // |
  T_XOR,       // ^
  T_BIT_CLEAR, // &^
  T_SHL,       // <<
  T_SHR,       // >>

  // Comparison Operators
  T_EQ,  // ==
  T_NEQ, // !=
  T_LT,  // <
  T_LTE, // <=
  T_GT,  // >
  T_GTE, // >=

  // Logical Operators
  T_LOGICAL_AND, // &&
  T_LOGICAL_OR,  // ||
  T_LOGICAL_NOT, // !

  // Assignment Operators
  T_ASSIGN,           // =
  T_ADD_ASSIGN,       // +=
  T_SUB_ASSIGN,       // -=
  T_MUL_ASSIGN,       // *=
  T_DIV_ASSIGN,       // /=
  T_REM_ASSIGN,       // %=
  T_AND_ASSIGN,       // &=
  T_OR_ASSIGN,        // |=
  T_XOR_ASSIGN,       // ^=
  T_SHL_ASSIGN,       // <<=
  T_SHR_ASSIGN,       // >>=
  T_BIT_CLEAR_ASSIGN, // &^=

  // Other
  T_LPAREN,    // (
  T_RPAREN,    // )
  T_LBRACKET,  // [
  T_RBRACKET,  // ]
  T_LBRACE,    // {
  T_RBRACE,    // }
  T_DOT,       // .
  T_COMMA,     // ,
  T_COLON,     // :
  T_SEMICOLON, // ;

  // Non-terminal tokens
  T_COMMENT,
  T_STRING,
  T_SPACE,
  T_INTEGER,
  T_FLOAT,
  T_NAME

} TokenType;

static TokenType PRECEDENCE_TABLE[][8] = {
    {T_LOGICAL_OR},
    {T_LOGICAL_AND},
    {T_EQ, T_NEQ, T_LT, T_LTE, T_GT, T_GTE},
    {T_ADD, T_SUB, T_OR, T_AND, T_XOR},
    {T_MUL, T_DIV, T_REM, T_SHL, T_SHR, T_AND, T_BIT_CLEAR}};

typedef struct Token
{
  TokenType token_type;
  Type type;
  char *buffer;
  int length;
} Token;

Token *new_token(TokenType type, char *buf, int len);
void dispose_token(Token *token);
void print_token(Token *token);

#endif
