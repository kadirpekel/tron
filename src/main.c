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
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "parser.h"

void visit_node(Node *node)
{
  char *nodeString = node_to_string(node);
  printf("%s\n", nodeString);
  free(nodeString);
}

int main(int argc, char **argv)
{
  LexState ls;
  init_lex_state(&ls, stdin);

  ParserState ps;
  init_parser(&ps, &ls);

  Node *node = parse(&ps, visit_node);

  // Do sth with entire ast here if you like

  destroy_node(node);
}
