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
#include <string.h>

#include "llvm.h"

LlvmScopeInfo *new_llvm_scope_info(LLVMValueRef function_ref, LLVMBasicBlockRef break_block, LLVMBasicBlockRef continue_block)
{
    LlvmScopeInfo *llvm_scope_info = malloc(sizeof(LlvmScopeInfo));
    llvm_scope_info->function_ref = function_ref;
    llvm_scope_info->break_block = break_block;
    llvm_scope_info->continue_block = continue_block;
    llvm_scope_info->interrupt_block = NULL;
    return llvm_scope_info;
}

void dispose_llvm_scope_info(LlvmScopeInfo *llvm_scope_info)
{
    free(llvm_scope_info);
}

LlvmSymbolInfo *new_llvm_symbol_info(LLVMTypeRef type, LLVMValueRef value)
{
    LlvmSymbolInfo *llvm_symbol_info = malloc(sizeof(LlvmSymbolInfo));
    llvm_symbol_info->type = type;
    llvm_symbol_info->value = value;
    return llvm_symbol_info;
}

void dispose_llvm_symbol_info(LlvmSymbolInfo *llvm_symbol_info)
{
    free(llvm_symbol_info);
}

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

LLVMValueRef llvm_visit_call(Llvm *llvm, Call *call)
{
    Symbol *symbol = lookup_symbol(llvm->scope, call->name);
    if (symbol == NULL)
    {
        fatal("Symbol not found: %s\n", call->name);
    }

    int num_args = 0;
    Expression *arg = call->expression;
    while (arg != NULL)
    {
        num_args++;
        arg = arg->next;
    }
    LLVMValueRef *args = malloc(num_args * sizeof(LLVMValueRef));
    int i = 0;
    arg = call->expression;
    while (arg != NULL)
    {
        args[i++] = llvm_visit_expression(llvm, arg);
        arg = arg->next;
    }

    LlvmSymbolInfo *llvm_symbol_info = (LlvmSymbolInfo *)symbol->info;
    return LLVMBuildCall2(llvm->builder, llvm_symbol_info->type, llvm_symbol_info->value, args, num_args, call->name);
}

LLVMValueRef llvm_visit_name(Llvm *llvm, Name *name)
{
    Symbol *symbol = lookup_symbol(llvm->scope, name->value);
    if (symbol == NULL)
    {
        fatal("Symbol not found: %s\n", name->value);
    }
    LlvmSymbolInfo *llvm_symbol_info = symbol->info;
    return LLVMBuildLoad2(llvm->builder, llvm_symbol_info->type, llvm_symbol_info->value, name->value);
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
        case T_REM:
            result = LLVMBuildSRem(llvm->builder, left, right, "srem");
            break;
        case T_SHL:
            result = LLVMBuildShl(llvm->builder, left, right, "shl");
            break;
        case T_SHR:
            result = LLVMBuildLShr(llvm->builder, left, right, "lshr");
            break;
        case T_AND:
            result = LLVMBuildAnd(llvm->builder, left, right, "and");
            break;
        case T_OR:
            result = LLVMBuildOr(llvm->builder, left, right, "or");
            break;
        case T_XOR:
            result = LLVMBuildXor(llvm->builder, left, right, "xor");
            break;
        case T_BIT_CLEAR:
            result = LLVMBuildAnd(llvm->builder, left, LLVMBuildNot(llvm->builder, right, "not"), "bitclear");
            break;
        case T_EQ:
            result = LLVMBuildICmp(llvm->builder, LLVMIntEQ, left, right, "eq");
            break;
        case T_NEQ:
            result = LLVMBuildICmp(llvm->builder, LLVMIntNE, left, right, "neq");
            break;
        case T_LT:
            result = LLVMBuildICmp(llvm->builder, LLVMIntSLT, left, right, "lt");
            break;
        case T_LTE:
            result = LLVMBuildICmp(llvm->builder, LLVMIntSLE, left, right, "lte");
            break;
        case T_GT:
            result = LLVMBuildICmp(llvm->builder, LLVMIntSGT, left, right, "gt");
            break;
        case T_GTE:
            result = LLVMBuildICmp(llvm->builder, LLVMIntSGE, left, right, "gte");
            break;
        case T_LOGICAL_AND:
            result = LLVMBuildAnd(llvm->builder, left, right, "logical_and");
            break;
        case T_LOGICAL_OR:
            result = LLVMBuildOr(llvm->builder, left, right, "logical_or");
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
            switch (expression->token->token_type)
            {
            case T_INC:
                result = LLVMBuildAdd(llvm->builder, left, LLVMConstInt(LLVMInt32Type(), 1, 0), "inc");
                break;
            case T_DEC:
                result = LLVMBuildSub(llvm->builder, left, LLVMConstInt(LLVMInt32Type(), 1, 0), "dec");
                break;
            default:
                fprintf(stderr, "Invalid left unary expression\n");
                exit(EXIT_FAILURE);
                break;
            }
        }
        else if (right != NULL)
        {
            // Only right expression set so this is right to left unary expression
            switch (expression->token->token_type)
            {
            case T_SUB:
                result = LLVMBuildNeg(llvm->builder, right, "neg");
                break;
            default:
                fprintf(stderr, "Invalid right unary expression\n");
                exit(EXIT_FAILURE);
                break;
            }
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
                result = llvm_visit_call(llvm, (Call *)node->data);
                break;
            case N_NAME:
                result = llvm_visit_name(llvm, (Name *)node->data);
                break;
            default:
                fprintf(stderr, "Unsupported node type in this context\n");
                exit(EXIT_FAILURE);
                break;
            }
        }
    }

    return result;
}

int is_global_variable(LLVMValueRef value)
{
    return LLVMIsAGlobalValue(value) && LLVMGetGlobalParent(value);
}

void llvm_visit_assignment(Llvm *llvm, Assignment *assignment)
{
    Symbol *symbol = lookup_symbol(llvm->scope, assignment->name);
    if (symbol == NULL)
    {
        fatal("Symbol not found");
    }

    LlvmSymbolInfo *llvm_symbol_info = symbol->info;
    LLVMValueRef expr_value = llvm_visit_expression(llvm, assignment->expression);

    if (is_global_variable(llvm_symbol_info->value))
    {
        if (LLVMIsAConstant(expr_value))
        {
            LLVMSetInitializer(llvm_symbol_info->value, expr_value);
        }
        else
        {
            fatal("Global variables must be initialized with a constant expression");
        }
    }
    else
    {
        LLVMBuildStore(llvm->builder, expr_value, llvm_symbol_info->value);
    }
}

void llvm_visit_variable(Llvm *llvm, Variable *variable)
{
    LLVMValueRef function_ref = find_enclosing_scope_info(llvm->scope, SCOPE_FUNCTION);
    LLVMTypeRef type = get_llvm_type(llvm, variable->type_info);
    LLVMValueRef value;

    if (function_ref != NULL)
    {
        value = LLVMBuildAlloca(llvm->builder, type, variable->name);
    }
    else
    {
        value = LLVMAddGlobal(llvm->module, type, variable->name);
        LLVMSetLinkage(value, LLVMExternalLinkage);
    }
    insert_symbol(llvm->scope, SYMBOL_VARIABLE, variable->name, new_llvm_symbol_info(type, value));
    if (variable->assignment)
    {
        llvm_visit_assignment(llvm, variable->assignment);
    }
}

LLVMBasicBlockRef llvm_visit_block(Llvm *llvm, ScopeType scope_type, Block *block, LLVMBasicBlockRef llvm_block, LLVMBasicBlockRef llvm_exit_block, LlvmScopeInfo *llvm_scope_info)
{
    if (block->statements != NULL)
    {
        llvm->scope = push_scope(llvm->scope, scope_type, llvm_scope_info, NULL, NULL);
        llvm_visit(llvm, block->statements);
        if (llvm_scope_info->interrupt_block != NULL)
        {
            LLVMBuildBr(llvm->builder, llvm_scope_info->interrupt_block);
        }
        else if (llvm_exit_block != NULL)
        {
            LLVMBuildBr(llvm->builder, llvm_exit_block);
        }
        llvm->scope = pop_scope(llvm->scope);
    }
    return llvm_block;
}

void llvm_visit_function(Llvm *llvm, Function *function)
{
    LLVMTypeRef type = LLVMFunctionType(get_llvm_type(llvm, function->type_info), NULL, 0, 0);
    LLVMValueRef value = LLVMAddFunction(llvm->module, function->name, type);
    insert_symbol(llvm->scope, SYMBOL_FUNCTION, function->name, new_llvm_symbol_info(type, value));
    LLVMBasicBlockRef llvm_block = LLVMAppendBasicBlockInContext(llvm->context, value, "entry");
    LLVMPositionBuilderAtEnd(llvm->builder, llvm_block);
    LlvmScopeInfo *llvm_scope_info = new_llvm_scope_info(value, NULL, NULL);
    llvm_visit_block(llvm, SCOPE_FUNCTION, function->body, llvm_block, NULL, llvm_scope_info);
}

void llvm_visit_if(Llvm *llvm, If *if_)
{
    LLVMBasicBlockRef current_block = LLVMGetInsertBlock(llvm->builder);
    LLVMValueRef function_ref = LLVMGetBasicBlockParent(current_block);
    LLVMBasicBlockRef next_block = LLVMGetNextBasicBlock(current_block);

    LLVMBasicBlockRef if_exit;
    if (next_block != NULL)
    {
        if_exit = LLVMInsertBasicBlockInContext(llvm->context, next_block, "if_exit");
    }
    else
    {
        if_exit = LLVMAppendBasicBlockInContext(llvm->context, function_ref, "if_exit");
    }

    LLVMBasicBlockRef if_check = NULL;
    while (if_ != NULL)
    {
        if (if_check == NULL)
        {
            if_check = LLVMInsertBasicBlockInContext(llvm->context, if_exit, "if_check");
            LLVMBuildBr(llvm->builder, if_check);
        }

        LLVMPositionBuilderAtEnd(llvm->builder, if_check);

        LLVMBasicBlockRef if_body = LLVMInsertBasicBlockInContext(llvm->context, if_exit, "if_body");

        if (if_->condition)
        {
            LLVMValueRef cond_value = llvm_visit_expression(llvm, if_->condition);
            if (if_->next)
            {
                if_check = LLVMInsertBasicBlockInContext(llvm->context, if_exit, "if_check");
                LLVMBuildCondBr(llvm->builder, cond_value, if_body, if_check);
            }
            else
            {
                LLVMBuildCondBr(llvm->builder, cond_value, if_body, if_exit);
            }
        }
        else
        {
            LLVMBuildBr(llvm->builder, if_body);
        }

        LLVMPositionBuilderAtEnd(llvm->builder, if_body);

        LlvmScopeInfo *llvm_scope_info = new_llvm_scope_info(NULL, NULL, NULL);
        llvm_visit_block(llvm, SCOPE_IF, if_->body, if_body, if_exit, llvm_scope_info);

        if_ = if_->next;
    }

    LLVMPositionBuilderAtEnd(llvm->builder, if_exit);
}

void llvm_visit_break(Llvm *llvm, Break *break_)
{
    LlvmScopeInfo *enclosing_llvm_scope_info = find_enclosing_scope_info(llvm->scope, SCOPE_WHILE);
    LlvmScopeInfo *llvm_scope_info = llvm->scope->info;
    llvm_scope_info->interrupt_block = enclosing_llvm_scope_info->break_block;
}

void llvm_visit_continue(Llvm *llvm, Continue *continue_)
{
    LlvmScopeInfo *enclosing_llvm_scope_info = find_enclosing_scope_info(llvm->scope, SCOPE_WHILE);
    LlvmScopeInfo *llvm_scope_info = llvm->scope->info;
    llvm_scope_info->interrupt_block = enclosing_llvm_scope_info->continue_block;
}

void llvm_visit_while(Llvm *llvm, While *while_)
{
    LLVMBasicBlockRef current_block = LLVMGetInsertBlock(llvm->builder);
    LLVMValueRef function_ref = LLVMGetBasicBlockParent(current_block);
    LLVMBasicBlockRef next_block = LLVMGetNextBasicBlock(current_block);

    LLVMBasicBlockRef while_exit;
    if (next_block != NULL)
    {
        while_exit = LLVMInsertBasicBlockInContext(llvm->context, next_block, "while_exit");
    }
    else
    {
        while_exit = LLVMAppendBasicBlockInContext(llvm->context, function_ref, "while_exit");
    }

    LLVMBasicBlockRef while_check = LLVMInsertBasicBlockInContext(llvm->context, while_exit, "while_check");
    LLVMBasicBlockRef while_body = LLVMInsertBasicBlockInContext(llvm->context, while_exit, "while_body");

    LLVMPositionBuilderAtEnd(llvm->builder, current_block);
    LLVMBuildBr(llvm->builder, while_check);

    LLVMPositionBuilderAtEnd(llvm->builder, while_check);
    LLVMValueRef cond_value = llvm_visit_expression(llvm, while_->condition);
    LLVMBuildCondBr(llvm->builder, cond_value, while_body, while_exit);

    LLVMPositionBuilderAtEnd(llvm->builder, while_body);
    LlvmScopeInfo *llvm_scope_info = new_llvm_scope_info(NULL, while_exit, while_check);
    llvm_visit_block(llvm, SCOPE_WHILE, while_->body, while_body, while_check, llvm_scope_info);
    LLVMPositionBuilderAtEnd(llvm->builder, while_exit);
}

void llvm_visit_return(Llvm *llvm, Return *return_)
{
    LLVMValueRef ret_value = llvm_visit_expression(llvm, return_->expression);
    LLVMBuildRet(llvm->builder, ret_value);
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
    case N_BREAK:
        llvm_visit_break(llvm, (Break *)node->data);
        break;
    case N_CONTINUE:
        llvm_visit_continue(llvm, (Continue *)node->data);
        break;
    default:
        fprintf(stderr, "Unexpected node type: %d\n", node->node_type);
        exit(EXIT_FAILURE);
        break;
    }
}

void llvm_visit(Llvm *llvm, Node *node)
{
    Node *current = node;
    LlvmScopeInfo *scope_info = llvm->scope->info;
    while (current != NULL)
    {
        if (scope_info->interrupt_block == NULL)
        {
            llvm_visit_statement(llvm, current);
        }
        current = current->next;
    }
}

Llvm *new_llvm()
{
    Llvm *llvm = malloc(sizeof(Llvm));
    llvm->context = LLVMContextCreate();
    // LLVMContextSetOpaquePointers(llvm->context, 0);
    llvm->module = LLVMModuleCreateWithNameInContext("default", llvm->context);
    llvm->builder = LLVMCreateBuilderInContext(llvm->context);

    LlvmScopeInfo *llvm_scope_info = new_llvm_scope_info(NULL, NULL, NULL);
    llvm->scope = push_scope(NULL, SCOPE_ROOT, llvm_scope_info, (void (*)(void *))dispose_llvm_scope_info, (void (*)(void *))dispose_llvm_symbol_info);

    LLVMTypeRef param_types[] = {LLVMInt32TypeInContext(llvm->context)};
    LLVMTypeRef type = LLVMFunctionType(LLVMInt32TypeInContext(llvm->context), param_types, 1, 0);
    LLVMValueRef value = LLVMAddFunction(llvm->module, "print_int", type);
    insert_symbol(llvm->scope, SYMBOL_FUNCTION, "print_int", new_llvm_symbol_info(type, value));

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
    pop_scope(llvm->scope);
    free(llvm);
}

void llvm_compile(Llvm *llvm, char *output)
{
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmPrinters();

    char *err;
    LLVMTargetRef target;
    LLVMTargetMachineRef target_machine;

    if (LLVMGetTargetFromTriple(LLVMGetDefaultTargetTriple(), &target, &err) != 0)
    {
        fatal("Could not get target information");
    }

    target_machine = LLVMCreateTargetMachine(target, LLVMGetDefaultTargetTriple(),
                                             "", "", LLVMCodeGenLevelDefault,
                                             LLVMRelocDefault, LLVMCodeModelDefault);
    if (!target_machine)
    {
        fatal("Could not create target machine");
    }

    if (LLVMTargetMachineEmitToFile(target_machine, llvm->module, output,
                                    LLVMObjectFile, &err) != 0)
    {
        fatal("Could not compile for the target machine");
    }
}

void llvm_validate(Llvm *llvm)
{
    char *error_msg = NULL;

    LLVMBool result = LLVMVerifyModule(llvm->module, LLVMAbortProcessAction, &error_msg);

    if (result != 0)
    {
        fatal("Module verification failed: %s\n", error_msg);
    }
}