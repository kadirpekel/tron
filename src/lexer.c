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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>

#include "lexer.h"

// Lexical Analyzer

void next(LexState *ls)
{
  ls->c = fgetc(ls->file);

  ls->col++;
  if (ls->c == '\n')
  {
    ls->line++;
    ls->col = 0;
  }
}

void accept(LexState *ls, int type)
{
  ls->buffer[ls->length++] = ls->c;
  ls->token_type = type;
  next(ls);
}

Token *reset(LexState *ls)
{
  Token *token = new_token(ls->token_type, ls->buffer, ls->length);
  ls->length = 0;
  return token;
}

void init_lex_state(LexState *ls, FILE *file)
{
  ls->length = 0;
  ls->col = 0;
  ls->line = 0;
  ls->token_type = T_NOMATCH;
  ls->file = file;
  next(ls);
}

Token *lex(LexState *ls)
{

  if (ls->c == EOF)
  {
    accept(ls, T_EOF);
  }
  else if (isalpha(ls->c))
  {
    accept(ls, T_NAME);
    while (isalpha(ls->c) || isdigit(ls->c) || ls->c == '_')
    {
      accept(ls, T_NAME);
    };
  }
  else if (isdigit(ls->c))
  {
    accept(ls, T_NUMBER);
    while (isdigit(ls->c) || ls->c == '.')
    {
      accept(ls, T_NUMBER);
    };
  }
  else if (isspace(ls->c))
  {
    accept(ls, T_SPACE);
    while (isspace(ls->c))
    {
      accept(ls, T_SPACE);
    }
  }
  else if (ls->c == '"')
  {
    next(ls);
    while (ls->c != '"')
    {
      accept(ls, T_STRING);
    }
    next(ls);
  }
  else if (ls->c == '(')
  {
    accept(ls, T_LPAREN);
  }
  else if (ls->c == ')')
  {
    accept(ls, T_RPAREN);
  }
  else if (ls->c == '{')
  {
    accept(ls, T_LCBRACET);
  }
  else if (ls->c == '}')
  {
    accept(ls, T_RCBRACET);
  }
  else if (ls->c == '[')
  {
    accept(ls, T_LBRACET);
  }
  else if (ls->c == ']')
  {
    accept(ls, T_RBRACET);
  }
  else if (ls->c == '#')
  {
    accept(ls, T_COMMENT);
    while (ls->c != '\n')
    {
      accept(ls, T_COMMENT);
    }
  }
  else if (ls->c == '+')
  {
    accept(ls, T_ADD);
  }
  else if (ls->c == '-')
  {
    accept(ls, T_SUB);
  }
  else if (ls->c == '*')
  {
    accept(ls, T_MUL);
  }
  else if (ls->c == '/')
  {
    accept(ls, T_DIV);
  }
  else if (ls->c == '=')
  {
    accept(ls, T_ASSIGN);
  }
  else if (ls->c == ';')
  {
    accept(ls, T_SEMICOLON);
  }
  else if (ls->c == ':')
  {
    accept(ls, T_COLON);
  }
  else if (ls->c == ',')
  {
    accept(ls, T_COMMA);
  }
  else
  {
    accept(ls, T_NOMATCH);
  }

  return reset(ls);
}