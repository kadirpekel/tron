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

void lexer_error(LexState *ls, char *msg)
{
  fprintf(stderr, "Lexer Error <%d:%d> %s\n",
          ls->line,
          ls->col,
          msg);
  exit(EXIT_FAILURE);
}

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

void accept(LexState *ls, TokenType type)
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
  else if (ls->c == '(')
  {
    accept(ls, T_LPAREN);
  }
  else if (ls->c == ')')
  {
    accept(ls, T_RPAREN);
  }
  else if (ls->c == '[')
  {
    accept(ls, T_LBRACKET);
  }
  else if (ls->c == ']')
  {
    accept(ls, T_RBRACKET);
  }
  else if (ls->c == '{')
  {
    accept(ls, T_LBRACE);
  }
  else if (ls->c == '}')
  {
    accept(ls, T_RBRACE);
  }
  else if (ls->c == '.')
  {
    accept(ls, T_DOT);
  }
  else if (ls->c == ',')
  {
    accept(ls, T_COMMA);
  }
  else if (ls->c == ':')
  {
    accept(ls, T_COLON);
  }
  else if (ls->c == ';')
  {
    accept(ls, T_SEMICOLON);
  }
  else if (ls->c == '+')
  {
    accept(ls, T_ADD);

    if (ls->c == '+')
    {
      accept(ls, T_INC);
    }
    else if (ls->c == '=')
    {
      accept(ls, T_ADD_ASSIGN);
    }
  }
  else if (ls->c == '-')
  {
    accept(ls, T_SUB);

    if (ls->c == '-')
    {
      accept(ls, T_DEC);
    }
    else if (ls->c == '=')
    {
      accept(ls, T_SUB_ASSIGN);
    }
  }
  else if (ls->c == '*')
  {
    accept(ls, T_MUL);

    if (ls->c == '=')
    {
      accept(ls, T_MUL_ASSIGN);
    }
  }
  else if (ls->c == '/')
  {
    accept(ls, T_DIV);
    if (ls->c == '=')
    {
      accept(ls, T_DIV_ASSIGN);
    }
  }
  else if (ls->c == '%')
  {
    accept(ls, T_REM);

    if (ls->c == '=')
    {
      accept(ls, T_REM_ASSIGN);
    }
  }
  else if (ls->c == '!')
  {
    accept(ls, T_LOGICAL_NOT);
  }
  else if (ls->c == '&')
  {
    accept(ls, T_AND);

    if (ls->c == '=')
    {
      accept(ls, T_AND_ASSIGN);
    }
    else if (ls->c == '&')
    {
      accept(ls, T_LOGICAL_AND);
    }
    else if (ls->c == '^')
    {
      accept(ls, T_BIT_CLEAR);

      if (ls->c == '=')
      {
        accept(ls, T_BIT_CLEAR_ASSIGN);
      }
    }
  }
  else if (ls->c == '|')
  {
    accept(ls, T_OR);

    if (ls->c == '=')
    {
      accept(ls, T_OR_ASSIGN);
    }
    else if (ls->c == '|')
    {
      accept(ls, T_LOGICAL_OR);
    }
  }
  else if (ls->c == '^')
  {
    accept(ls, T_XOR);

    if (ls->c == '=')
    {
      accept(ls, T_XOR_ASSIGN);
    }
  }
  else if (ls->c == '<')
  {
    accept(ls, T_LT);
    if (ls->c == '<')
    {
      accept(ls, T_SHL);

      if (ls->c == '=')
      {
        accept(ls, T_SHL_ASSIGN);
      }
    }
    else if (ls->c == '=')
    {
      accept(ls, T_LTE);
    }
  }
  else if (ls->c == '>')
  {
    accept(ls, T_GT);

    if (ls->c == '>')
    {
      accept(ls, T_SHR);

      if (ls->c == '=')
      {
        accept(ls, T_SHR_ASSIGN);
      }
    }
    else if (ls->c == '=')
    {
      accept(ls, T_GTE);
    }
  }
  else if (ls->c == '=')
  {
    accept(ls, T_ASSIGN);

    if (ls->c == '=')
    {
      accept(ls, T_EQ);
    }
  }
  else if (ls->c == '#')
  {
    accept(ls, T_COMMENT);
    while (ls->c != '\n')
    {
      accept(ls, T_COMMENT);
    }
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
  else if (isdigit(ls->c))
  {
    TokenType token_type = T_INTEGER;
    accept(ls, token_type);
    while (isdigit(ls->c) || (token_type == T_INTEGER && ls->c == '.'))
    {
      if (ls->c == '.')
      {
        token_type = T_FLOAT;
      }
      accept(ls, token_type);
    };
  }
  else if (isalpha(ls->c))
  {
    accept(ls, T_NAME);
    while (isalpha(ls->c) || isdigit(ls->c) || ls->c == '_')
    {
      accept(ls, T_NAME);
    };
  }
  else
  {
    accept(ls, T_NOMATCH);
  }

  return reset(ls);
}