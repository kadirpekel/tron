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

#include "token.h"

Token *new_token(TokenType token_type, char *buf, int len)
{
    Token *token = malloc(sizeof(Token));
    token->token_type = token_type;
    token->buffer = strndup(buf, len);
    token->length = len;
    return token;
}

void dispose_token(Token *token)
{
    free(token->buffer);
    free(token);
}
