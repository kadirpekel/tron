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

#include <stdio.h>
#include <stdlib.h>

#include "llvm.h"

void visit_variable(Variable *variable)
{
    printf("Variable: %s\n", variable->name);
}

void visit_function(Function *function)
{
    printf("Function %s\n", function->name);
}

void visit_if(If *if_)
{
    printf("If\n");
}

void visit_while(While *while_)
{
    printf("While\n");
}

void visit_call(Call *call)
{
    printf("Call %s\n", call->name);
}

void visit_assignment(Assignment *assignment)
{
    printf("Assignment %s\n", assignment->name);
}

void visit_return(Return *return_)
{
    printf("Return\n");
}

Llvm *new_llvm()
{
    Llvm *llvm = malloc(sizeof(Llvm));
    llvm->context = LLVMContextCreate();
    llvm->module = LLVMModuleCreateWithNameInContext("default", llvm->context);
    llvm->builder = LLVMCreateBuilderInContext(llvm->context);
    llvm->backend.visit_function = visit_function;
    llvm->backend.visit_variable = visit_variable;
    llvm->backend.visit_if = visit_if;
    llvm->backend.visit_while = visit_while;
    llvm->backend.visit_assignment = visit_assignment;
    llvm->backend.visit_call = visit_call;
    llvm->backend.visit_return = visit_return;
    return llvm;
}

void dispose_llvm(Llvm *llvm)
{
    LLVMDisposeBuilder(llvm->builder);
    LLVMDisposeModule(llvm->module);
    LLVMContextDispose(llvm->context);
    free(llvm);
}
