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

#include "mToken.h"

Token *newToken(TokenType type, char *buf, int len)
{
    Token *token = malloc(sizeof(Token));
    token->buf = malloc((len) * sizeof(char));
    token->len = len;
    token->tokenType = type;
    strncpy(token->buf, buf, len);
    return token;
}

void destroyToken(Token *token)
{
    free(token->buf);
    free(token);
}

void printToken(Token *token)
{
    printf("[%d] %.*s\n", token->tokenType, token->len, token->buf);
}