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

#include "llvm.h"

void llvm_visit_variable(void *state, Variable *variable)
{
    printf("Variable %s\n", variable->name);
}

void llvm_visit_function(void *state, Function *function)
{
    printf("Function %s\n", function->name);
}

void llvm_visit_if(void *state, If *if_)
{
    printf("If\n");
}

void llvm_visit_while(void *state, While *while_)
{
    printf("While\n");
}

void llvm_visit_call(void *state, Call *call)
{
    printf("Call %s\n", call->name);
}

void llvm_visit_assignment(void *state, Assignment *assignment)
{
    printf("Assignment %s\n", assignment->name);
}

void llvm_visit_return(void *state, Return *return_)
{
    printf("Return\n");
}

void llvm_visit(void *state, Node *node)
{
    switch (node->node_type)
    {
    case N_VARIABLE:
        llvm_visit_variable(state, (Variable *)node->data);
        break;
    case N_FUNCTION:
        llvm_visit_function(state, (Function *)node->data);
        break;
    case N_IF:
        llvm_visit_if(state, (If *)node->data);
        break;
    case N_WHILE:
        llvm_visit_while(state, (While *)node->data);
        break;
    case N_CALL:
        llvm_visit_call(state, (Call *)node->data);
        break;
    case N_ASSIGNMENT:
        llvm_visit_assignment(state, (Assignment *)node->data);
        break;
    case N_RETURN:
        llvm_visit_return(state, (Return *)node->data);
        break;
    default:
        break;
    }
}

Llvm *new_llvm()
{
    Llvm *llvm = malloc(sizeof(Llvm));
    llvm->context = LLVMContextCreate();
    llvm->module = LLVMModuleCreateWithNameInContext("default", llvm->context);
    llvm->builder = LLVMCreateBuilderInContext(llvm->context);
    llvm->backend = new_backend(llvm_visit);
    return llvm;
}

void dispose_llvm(Llvm *llvm)
{
    LLVMDisposeBuilder(llvm->builder);
    LLVMDisposeModule(llvm->module);
    LLVMContextDispose(llvm->context);
    dispose_backend(llvm->backend);
    free(llvm);
}
