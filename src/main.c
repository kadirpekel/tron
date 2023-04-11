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
  Parser *p = new_parser(stdin);
  Node *ast = parse(p);
  dispose_parser(p);

  Llvm *llvm = new_llvm();

  llvm_visit(llvm, ast);
  llvm_dump(llvm, stdout);

  dispose_llvm(llvm);
  dispose_node(ast);
}
