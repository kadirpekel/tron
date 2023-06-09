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

#include "parser.h"
#include "llvm.h"

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    fprintf(stderr, "Usage: tron <input_file> <output_file>\n");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(argv[1], "r");

  Parser *p = new_parser(file);
  Node *ast = parse(p);

  Llvm *llvm = new_llvm();

  llvm_visit(llvm, ast);
  llvm_validate(llvm);
  llvm_dump(llvm, stdout);
  llvm_compile(llvm, argv[2]);

  dispose_llvm(llvm);

  dispose_node(ast);
  dispose_parser(p);
  fclose(file);
}
