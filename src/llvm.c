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

LLVMValueRef llvm_visit_integer(Llvm *llvm, Integer *integer)
{
    return LLVMConstInt(LLVMInt32TypeInContext(llvm->context), integer->value, 0);
}

LLVMValueRef llvm_visit_float(Llvm *llvm, Float *floating_point)
{
    return LLVMConstReal(LLVMFloatTypeInContext(llvm->context), floating_point->value);
}

LLVMValueRef llvm_visit_call_expression(Llvm *llvm, Call *call)
{
    LLVMValueRef callee = LLVMGetNamedFunction(llvm->module, call->name);
    return LLVMBuildCall2(llvm->builder, LLVMTypeOf(callee), callee, NULL, 0, call->name);
}

LLVMValueRef llvm_visit_name(Llvm *llvm, Name *name)
{
    LLVMValueRef var_ptr = LLVMGetNamedGlobal(llvm->module, name->value);
    if (var_ptr == NULL)
    {
        fprintf(stderr, "Variable not found: %s\n", name->value);
        exit(EXIT_FAILURE);
    }

    LLVMTypeRef var_type = LLVMGetElementType(LLVMTypeOf(var_ptr));
    return LLVMBuildLoad2(llvm->builder, var_type, var_ptr, name->value);
}

LLVMValueRef llvm_visit_expression(Llvm *llvm, Expression *expression)
{

    if (expression == NULL)
    {
        return NULL;
    }

    LLVMValueRef left = llvm_visit_expression(llvm, expression->left);
    LLVMValueRef right = llvm_visit_expression(llvm, expression->right);

    LLVMValueRef result;

    if (left != NULL && right != NULL)
    {
        printf("%d \n", expression->token->token_type);
        // Both left and right set so this is a binary expression
        switch (expression->token->token_type)
        {
        case T_ADD:
            result = LLVMBuildAdd(llvm->builder, left, right, "add");
            break;
        case T_SUB:
            result = LLVMBuildSub(llvm->builder, left, right, "sub");
            break;
        case T_MUL:
            result = LLVMBuildMul(llvm->builder, left, right, "mul");
            break;
        case T_DIV:
            result = LLVMBuildSDiv(llvm->builder, left, right, "sdiv");
            break;
        default:
            fprintf(stderr, "Invalid expression\n");
            exit(EXIT_FAILURE);
            break;
        }
    }
    else
    {
        if (left != NULL)
        {
            // Only left expression set so this is left to right unary expression
            result = NULL;
        }
        else if (right != NULL)
        {
            // Only right expression set so this is right to left unary expression
            result = NULL;
        }
        else
        {
            // This is a leaf expression where it can be either:
            // constant literal (int, float)
            // function call
            // variable name

            Node *node = expression->node;
            switch (node->node_type)
            {
            case N_INTEGER:
                result = llvm_visit_integer(llvm, (Integer *)node->data);
                break;
            case N_FLOAT:
                result = llvm_visit_float(llvm, (Float *)node->data);
                break;
            case N_CALL:
                result = llvm_visit_call_expression(llvm, (Call *)node->data);
                break;
            case N_NAME:
                result = llvm_visit_name(llvm, (Name *)node->data);
                break;
            default:
                fprintf(stderr, "Unsupported node type in this context");
                exit(EXIT_FAILURE);
                break;
            }
        }
    }

    return result;
}

void llvm_visit_variable(Llvm *llvm, Variable *variable)
{
    LLVMValueRef current_function = (LLVMValueRef)llvm->function;
    LLVMTypeRef var_type = get_llvm_type(llvm, variable->type_info);

    LLVMValueRef allocated_var;

    if (current_function != NULL)
    {
        LLVMBasicBlockRef entry_block = LLVMGetEntryBasicBlock(current_function);
        LLVMPositionBuilderAtEnd(llvm->builder, entry_block);
        allocated_var = LLVMBuildAlloca(llvm->builder, var_type, variable->name);
    }
    else
    {
        allocated_var = LLVMAddGlobal(llvm->module, var_type, variable->name);
        LLVMSetLinkage(allocated_var, LLVMExternalLinkage);
    }
    if (variable->expression)
    {
        LLVMValueRef expr_value = llvm_visit_expression(llvm, variable->expression);
        if (current_function != NULL)
        {
            LLVMBuildStore(llvm->builder, expr_value, allocated_var);
        }
        else
        {
            LLVMSetInitializer(allocated_var, LLVMConstInt(var_type, LLVMConstIntGetZExtValue(expr_value), 0));
        }
    }
    else
    {
        LLVMSetInitializer(allocated_var, LLVMConstInt(var_type, 0, 0));
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
    printf("If - Not Implemented Yet\n");
}

void llvm_visit_while(Llvm *llvm, While *while_)
{
    printf("While - Not Implemented Yet\n");
}

void llvm_visit_call(Llvm *llvm, Call *call)
{
    printf("Call %s - Not Implemented Yet\n", call->name);
}

void llvm_visit_assignment(Llvm *llvm, Assignment *assignment)
{
    printf("Assignment %s - Not Implemented Yet\n", assignment->name);
}

void llvm_visit_return(Llvm *llvm, Return *return_)
{
    printf("Return - Not Implemented Yet\n");
}

void llvm_visit_statement(Llvm *llvm, Node *node)
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

void llvm_visit(Llvm *llvm, Node *node)
{
    Node *current = node;
    while (current)
    {
        llvm_visit_statement(llvm, current);
        current = current->next;
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
    LLVMDumpModule(llvm->module);
}

void dispose_llvm(Llvm *llvm)
{
    LLVMDisposeBuilder(llvm->builder);
    LLVMDisposeModule(llvm->module);
    LLVMContextDispose(llvm->context);
    free(llvm);
}
