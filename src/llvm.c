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

LLVMTypeRef get_llvm_type(Llvm *llvm, TypeInfo *type_info)
{
    LLVMTypeRef var_type;
    switch (type_info->type)
    {
    case TYPE_INT:
        var_type = LLVMInt32TypeInContext(llvm->context);
        break;
    case TYPE_FLOAT:
        var_type = LLVMFloatTypeInContext(llvm->context);
        break;
    default:
        fprintf(stderr, "Unsupported type for variable: %d\n", type_info->type);
        exit(1);
    }
    return var_type;
}

void llvm_visit_variable(Llvm *llvm, Variable *variable)
{
    LLVMValueRef current_function = (LLVMValueRef)llvm->function;
    LLVMTypeRef var_type = get_llvm_type(llvm, variable->type_info);

    if (current_function != NULL)
    {
        LLVMBasicBlockRef entry_block = LLVMGetEntryBasicBlock(current_function);
        LLVMBuilderRef tmp_builder = LLVMCreateBuilderInContext(llvm->context);

        LLVMValueRef first_instr = LLVMGetFirstInstruction(entry_block);
        if (first_instr)
        {
            LLVMPositionBuilderBefore(tmp_builder, first_instr);
        }
        else
        {
            LLVMPositionBuilderAtEnd(tmp_builder, entry_block);
        }

        LLVMBuildAlloca(tmp_builder, var_type, variable->name);
        LLVMDisposeBuilder(tmp_builder);
    }
    else
    {
        LLVMValueRef global_var = LLVMAddGlobal(llvm->module, var_type, variable->name);
        LLVMSetInitializer(global_var, LLVMConstInt(var_type, 0, 0));
        LLVMSetLinkage(global_var, LLVMExternalLinkage);
    }
}

void llvm_visit_function(Llvm *llvm, Function *function)
{
    LLVMValueRef parent_function = llvm->function;

    LLVMTypeRef func_type = LLVMFunctionType(LLVMInt32TypeInContext(llvm->context), NULL, 0, 0);
    LLVMValueRef llvm_function = LLVMAddFunction(llvm->module, function->name, func_type);

    llvm->function = llvm_function;

    LLVMBasicBlockRef entry_block = LLVMAppendBasicBlockInContext(llvm->context, llvm_function, "entry");
    LLVMPositionBuilderAtEnd(llvm->builder, entry_block);

    Node *stmt = function->body->statements;
    while (stmt)
    {
        llvm_visit(llvm, stmt);
        stmt = stmt->next;
    }
    LLVMValueRef ret_value = LLVMConstInt(LLVMInt32TypeInContext(llvm->context), 0, 0);
    LLVMBuildRet(llvm->builder, ret_value);

    llvm->function = parent_function;
}

void llvm_visit_if(Llvm *llvm, If *if_)
{
    printf("If\n");
}

void llvm_visit_while(Llvm *llvm, While *while_)
{
    printf("While\n");
}

void llvm_visit_call(Llvm *llvm, Call *call)
{
    printf("Call %s\n", call->name);
}

void llvm_visit_assignment(Llvm *llvm, Assignment *assignment)
{
    printf("Assignment %s\n", assignment->name);
}

void llvm_visit_return(Llvm *llvm, Return *return_)
{
    printf("Return\n");
}

void llvm_visit(Llvm *llvm, Node *node)
{
    switch (node->node_type)
    {
    case N_VARIABLE:
        llvm_visit_variable(llvm, (Variable *)node->data);
        break;
    case N_FUNCTION:
        llvm_visit_function(llvm, (Function *)node->data);
        break;
    case N_IF:
        llvm_visit_if(llvm, (If *)node->data);
        break;
    case N_WHILE:
        llvm_visit_while(llvm, (While *)node->data);
        break;
    case N_CALL:
        llvm_visit_call(llvm, (Call *)node->data);
        break;
    case N_ASSIGNMENT:
        llvm_visit_assignment(llvm, (Assignment *)node->data);
        break;
    case N_RETURN:
        llvm_visit_return(llvm, (Return *)node->data);
        break;
    default:
        fprintf(stderr, "Unexpected node type: %d", node->node_type);
        exit(EXIT_FAILURE);
        break;
    }
}

Llvm *new_llvm()
{
    Llvm *llvm = malloc(sizeof(Llvm));
    llvm->context = LLVMContextCreate();
    llvm->module = LLVMModuleCreateWithNameInContext("default", llvm->context);
    llvm->builder = LLVMCreateBuilderInContext(llvm->context);
    return llvm;
}

void llvm_dump(Llvm *llvm, FILE *out)
{
    char *ir = LLVMPrintModuleToString(llvm->module);
    fprintf(out, "%s", ir);
    LLVMDisposeMessage(ir);
}

void dispose_llvm(Llvm *llvm)
{
    LLVMDisposeBuilder(llvm->builder);
    LLVMDisposeModule(llvm->module);
    LLVMContextDispose(llvm->context);
    free(llvm);
}
