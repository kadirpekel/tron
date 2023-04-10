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

void lexer_error(Lexer *l, char *msg)
{
  fprintf(stderr, "Lexer Error <%d:%d> %s\n",
          l->line,
          l->col,
          msg);
  exit(EXIT_FAILURE);
}

void next(Lexer *l)
{
  l->c = fgetc(l->file);

  l->col++;
  if (l->c == '\n')
  {
    l->line++;
    l->col = 0;
  }
}

void accept(Lexer *l, TokenType type)
{
  l->buffer[l->length++] = l->c;
  l->token_type = type;
  next(l);
}

Token *reset(Lexer *l)
{
  Token *token = new_token(l->token_type, l->buffer, l->length);
  l->length = 0;
  return token;
}

Lexer *new_lexer(FILE *file)
{
  Lexer *l = malloc(sizeof(Lexer));
  l->length = 0;
  l->col = 0;
  l->line = 0;
  l->token_type = T_NOMATCH;
  l->file = file;
  next(l);
  return l;
}

void dispose_lexer(Lexer *l)
{
  free(l);
}

Token *lex(Lexer *l)
{
  if (l->c == EOF)
  {
    accept(l, T_EOF);
  }
  else if (l->c == '(')
  {
    accept(l, T_LPAREN);
  }
  else if (l->c == ')')
  {
    accept(l, T_RPAREN);
  }
  else if (l->c == '[')
  {
    accept(l, T_LBRACKET);
  }
  else if (l->c == ']')
  {
    accept(l, T_RBRACKET);
  }
  else if (l->c == '{')
  {
    accept(l, T_LBRACE);
  }
  else if (l->c == '}')
  {
    accept(l, T_RBRACE);
  }
  else if (l->c == '.')
  {
    accept(l, T_DOT);
  }
  else if (l->c == ',')
  {
    accept(l, T_COMMA);
  }
  else if (l->c == ':')
  {
    accept(l, T_COLON);
  }
  else if (l->c == ';')
  {
    accept(l, T_SEMICOLON);
  }
  else if (l->c == '+')
  {
    accept(l, T_ADD);

    if (l->c == '+')
    {
      accept(l, T_INC);
    }
    else if (l->c == '=')
    {
      accept(l, T_ADD_ASSIGN);
    }
  }
  else if (l->c == '-')
  {
    accept(l, T_SUB);

    if (l->c == '-')
    {
      accept(l, T_DEC);
    }
    else if (l->c == '=')
    {
      accept(l, T_SUB_ASSIGN);
    }
  }
  else if (l->c == '*')
  {
    accept(l, T_MUL);

    if (l->c == '=')
    {
      accept(l, T_MUL_ASSIGN);
    }
  }
  else if (l->c == '/')
  {
    accept(l, T_DIV);
    if (l->c == '=')
    {
      accept(l, T_DIV_ASSIGN);
    }
  }
  else if (l->c == '%')
  {
    accept(l, T_REM);

    if (l->c == '=')
    {
      accept(l, T_REM_ASSIGN);
    }
  }
  else if (l->c == '!')
  {
    accept(l, T_LOGICAL_NOT);
  }
  else if (l->c == '&')
  {
    accept(l, T_AND);

    if (l->c == '=')
    {
      accept(l, T_AND_ASSIGN);
    }
    else if (l->c == '&')
    {
      accept(l, T_LOGICAL_AND);
    }
    else if (l->c == '^')
    {
      accept(l, T_BIT_CLEAR);

      if (l->c == '=')
      {
        accept(l, T_BIT_CLEAR_ASSIGN);
      }
    }
  }
  else if (l->c == '|')
  {
    accept(l, T_OR);

    if (l->c == '=')
    {
      accept(l, T_OR_ASSIGN);
    }
    else if (l->c == '|')
    {
      accept(l, T_LOGICAL_OR);
    }
  }
  else if (l->c == '^')
  {
    accept(l, T_XOR);

    if (l->c == '=')
    {
      accept(l, T_XOR_ASSIGN);
    }
  }
  else if (l->c == '<')
  {
    accept(l, T_LT);
    if (l->c == '<')
    {
      accept(l, T_SHL);

      if (l->c == '=')
      {
        accept(l, T_SHL_ASSIGN);
      }
    }
    else if (l->c == '=')
    {
      accept(l, T_LTE);
    }
  }
  else if (l->c == '>')
  {
    accept(l, T_GT);

    if (l->c == '>')
    {
      accept(l, T_SHR);

      if (l->c == '=')
      {
        accept(l, T_SHR_ASSIGN);
      }
    }
    else if (l->c == '=')
    {
      accept(l, T_GTE);
    }
  }
  else if (l->c == '=')
  {
    accept(l, T_ASSIGN);

    if (l->c == '=')
    {
      accept(l, T_EQ);
    }
  }
  else if (l->c == '#')
  {
    accept(l, T_COMMENT);
    while (l->c != '\n')
    {
      accept(l, T_COMMENT);
    }
  }
  else if (isspace(l->c))
  {
    accept(l, T_SPACE);
    while (isspace(l->c))
    {
      accept(l, T_SPACE);
    }
  }
  else if (l->c == '"')
  {
    next(l);
    while (l->c != '"')
    {
      accept(l, T_STRING);
    }
    next(l);
  }
  else if (isdigit(l->c))
  {
    TokenType token_type = T_INTEGER;
    accept(l, token_type);
    while (isdigit(l->c) || (token_type == T_INTEGER && l->c == '.'))
    {
      if (l->c == '.')
      {
        token_type = T_FLOAT;
      }
      accept(l, token_type);
    };
  }
  else if (isalpha(l->c))
  {
    accept(l, T_NAME);
    while (isalpha(l->c) || isdigit(l->c) || l->c == '_')
    {
      accept(l, T_NAME);
    };
  }
  else
  {
    accept(l, T_NOMATCH);
  }

  return reset(l);
}