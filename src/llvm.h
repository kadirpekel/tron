/******************************************************************************
 * Copyright [2023] [Kadir PEKEL]
 *
 * Licensed under the Apache License, Version 2.0 (the "License"},;
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

#ifndef MLLVM_H_
#define MLLVM_H_

#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Transforms/Vectorize.h>
#include <llvm-c/BitWriter.h>

#include "scope.h"
#include "node.h"

typedef struct LlvmInfo
{
    LLVMTypeRef type;
    LLVMValueRef value;
} LlvmInfo;

typedef struct Llvm
{
    LLVMContextRef context;
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    Scope *scope;
} Llvm;

Llvm *new_llvm();
LlvmInfo *new_llvm_info(LLVMTypeRef type, LLVMValueRef value);
void llvm_visit(Llvm *llvm, Node *node);
LLVMValueRef llvm_visit_expression(Llvm *llvm, Expression *expression);
void llvm_dump(Llvm *llvm, FILE *out);
void llvm_compile(Llvm *llvm);
void llvm_validate(Llvm *llvm);
void dispose_llvm(Llvm *llvm);
void dispose_llvm_info(LlvmInfo *llvm_info);

#endif
