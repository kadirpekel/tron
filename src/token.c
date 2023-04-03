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

#include "token.h"

Token *new_token(TokenType tokenType, char *buf, int len)
{
    Token *token = malloc(sizeof(Token));
    token->len = len;
    token->buf = malloc((len + 1) * sizeof(char));
    token->buf[len] = '\0';
    token->tokenType = tokenType;
    strncpy(token->buf, buf, len);
    if (token->tokenType == T_NAME && strcmp(token->buf, "int") == 0)
    {
        token->tokenType = T_TYPE;
        token->type = TYPE_INT;
    }
    else if (token->tokenType == T_NAME && strcmp(token->buf, "float") == 0)
    {
        token->tokenType = T_TYPE;
        token->type = TYPE_FLOAT;
    }
    return token;
}

void destroy_token(Token *token)
{
    free(token->buf);
    free(token);
}

void print_token(Token *token)
{
    printf("[%d] %.*s\n", token->tokenType, token->len, token->buf);
}