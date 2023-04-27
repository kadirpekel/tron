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

#include <stdbool.h>
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>
#include <llvm-c/Transforms/Vectorize.h>
#include <llvm-c/BitWriter.h>

#include "scope.h"
#include "node.h"

typedef struct LlvmSymbolInfo
{
    LLVMTypeRef type;
    LLVMValueRef value;
} LlvmSymbolInfo;

typedef struct LlvmScopeInfo
{
    LLVMValueRef function_ref;
    LLVMBasicBlockRef break_block;
    LLVMBasicBlockRef continue_block;
} LlvmScopeInfo;

typedef struct Llvm
{
    LLVMContextRef context;
    LLVMModuleRef module;
    LLVMBuilderRef builder;
    Scope *scope;
} Llvm;

Llvm *new_llvm();
LlvmSymbolInfo *new_llvm_symbol_info(LLVMTypeRef type, LLVMValueRef value);
LlvmScopeInfo *new_llvm_scope_info(LLVMValueRef function_ref, LLVMBasicBlockRef break_block, LLVMBasicBlockRef continue_block);
void llvm_visit(Llvm *llvm, Node *node);
LLVMValueRef llvm_visit_expression(Llvm *llvm, Expression *expression);
void llvm_dump(Llvm *llvm, FILE *out);
void llvm_compile(Llvm *llvm, char *output);
void llvm_validate(Llvm *llvm);
void dispose_llvm(Llvm *llvm);
void dispose_llvm_symbol_info(LlvmSymbolInfo *llvm_symbol_info);
void dispose_llvm_scope_info(LlvmScopeInfo *llvm_scope_info);

#endif
