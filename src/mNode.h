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

#ifndef MNODE_H_
#define MNODE_H_

#include <stdio.h>

#include "mAssert.h"
#include "mToken.h"

typedef struct Node
{
    Token *value;
    struct Node *next;
    struct Node *sub;
} Node;

Node *newNode(Token *token);
void destroyNode(Node *node);
void printNode(Node *node);

#endif
