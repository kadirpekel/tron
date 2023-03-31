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

#include "mLexer.h"

// Lexical Analyzer

int initLexState(LexState *s, FILE *file)
{
  s->len = 0;
  s->type = T_EOF;
  s->file = file;
  s->c = fgetc(file);

  return 1;
}

void accept(LexState *s, int type)
{
  s->buf[s->len++] = s->c;
  s->type = type;
  s->c = fgetc(s->file);
}

void nextToken(LexState *s)
{
  s->c = fgetc(s->file);
}

Token *lex(LexState *s)
{

  if (s->c == EOF)
  {
    accept(s, T_EOF);
  }
  else if (isalpha(s->c))
  {
    accept(s, T_NAME);
    while (isalpha(s->c) || isdigit(s->c) || s->c == '_')
    {
      accept(s, T_NAME);
    };
  }
  else if (isdigit(s->c))
  {
    accept(s, T_NUMBER);
    while (isdigit(s->c) || s->c == '.')
    {
      accept(s, T_NUMBER);
    };
  }
  else if (isspace(s->c))
  {
    accept(s, T_WHITESPACE);
    while (isspace(s->c))
    {
      accept(s, T_WHITESPACE);
    }
  }
  else if (s->c == '"')
  {
    nextToken(s);
    while (s->c != '"')
    {
      accept(s, T_STRING);
    }
    nextToken(s);
  }
  else if (s->c == '(')
  {
    accept(s, T_LPAREN);
  }
  else if (s->c == ')')
  {
    accept(s, T_RPAREN);
  }
  else if (s->c == '#')
  {
    accept(s, T_COMMENT);
    while (s->c != '\n')
    {
      accept(s, T_COMMENT);
    }
  }
  Token *token = newToken(s->type, s->buf, s->len);
  s->len = 0;
  return token;
}