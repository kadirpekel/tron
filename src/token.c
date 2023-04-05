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

Token *new_token(TokenType token_type, char *buf, int len)
{
    Token *token = malloc(sizeof(Token));
    token->length = len;
    token->buffer = malloc((len + 1) * sizeof(char));
    token->buffer[len] = '\0';
    token->token_type = token_type;
    strncpy(token->buffer, buf, len);
    return token;
}

void destroy_token(Token *token)
{
    free(token->buffer);
    free(token);
}

void print_token(Token *token)
{
    printf("[%d] %.*s\n", token->token_type, token->length, token->buffer);
}