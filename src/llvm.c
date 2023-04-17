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

LlvmSymbolTable *new_llvm_symbol_table()
{
    LlvmSymbolTable *table = malloc(sizeof(LlvmSymbolTable));
    table->head = NULL;
    return table;
}

LlvmSymbolTableStack *new_llvm_symbol_table_stack(LLVMValueRef function)
{
    LlvmSymbolTableStack *stack = malloc(sizeof(LlvmSymbolTableStack));
    stack->table = new_llvm_symbol_table();
    stack->function = function;
    stack->next = NULL;
    return stack;
}

LlvmSymbol *find_symbol(LlvmSymbolTable *table, char *name)
{
    for (LlvmSymbol *sym = table->head; sym != NULL; sym = sym->next)
    {
        if (strcmp(sym->name, name) == 0)
        {
            return sym;
        }
    }
    return NULL;
}

void push_symbol(Llvm *llvm, char *name, LLVMValueRef value, LLVMTypeRef type)
{
    if (find_symbol(llvm->stack->table, name) != NULL)
    {
        fatal("Symbol already defined in current stack");
    }

    LlvmSymbol *new_symbol = malloc(sizeof(LlvmSymbol));
    new_symbol->name = strdup(name);
    new_symbol->value = value;
    new_symbol->type = type;
    new_symbol->next = llvm->stack->table->head;
    llvm->stack->table->head = new_symbol;
}

void push_scope(Llvm *llvm, LLVMValueRef llvm_function)
{
    if (llvm_function == NULL && llvm->stack != NULL)
    {
        llvm_function = llvm->stack->function;
    }

    LlvmSymbolTableStack *new_stack = new_llvm_symbol_table_stack(llvm_function);
    new_stack->next = llvm->stack;
    llvm->stack = new_stack;
}

void dispose_llvm_symbol_table(LlvmSymbolTable *table)
{
    LlvmSymbol *sym = table->head;
    while (sym != NULL)
    {
        LlvmSymbol *next = sym->next;
        free(sym->name);
        free(sym);
        sym = next;
    }
}

void pop_scope(Llvm *llvm)
{
    LlvmSymbolTableStack *top = llvm->stack;
    llvm->stack = top->next;
    dispose_llvm_symbol_table(top->table);
    free(top);
}

LlvmSymbol *find_in_stack(Llvm *llvm, char *name)
{
    for (LlvmSymbolTableStack *node = llvm->stack; node != NULL; node = node->next)
    {
        LlvmSymbol *value = find_symbol(node->table, name);
        if (value != NULL)
        {
            return value;
        }
    }
    return NULL;
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
    LlvmSymbol *sym = find_in_stack(llvm, call->name);
    if (sym == NULL)
    {
        fatal("Symbol not found: %s\n", call->name);
    }
    return LLVMBuildCall2(llvm->builder, sym->type, sym->value, NULL, 0, call->name);
}

LLVMValueRef llvm_visit_name(Llvm *llvm, Name *name)
{
    LlvmSymbol *sym = find_in_stack(llvm, name->value);
    if (sym == NULL)
    {
        fatal("Symbol not found: %s\n", name->value);
    }
    return LLVMBuildLoad2(llvm->builder, sym->type, sym->value, name->value);
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
    LlvmSymbol *sym = find_in_stack(llvm, assignment->name);
    if (sym == NULL)
    {
        fatal("Symbol not found");
    }

    LLVMValueRef expr_value = llvm_visit_expression(llvm, assignment->expression);

    if (is_global_variable(sym->value))
    {
        if (LLVMIsAConstant(expr_value))
        {
            LLVMSetInitializer(sym->value, expr_value);
        }
        else
        {
            fatal("Global variables must be initialized with a constant expression");
        }
    }
    else
    {
        LLVMBuildStore(llvm->builder, expr_value, sym->value);
    }
}

void llvm_visit_variable(Llvm *llvm, Variable *variable)
{
    LLVMValueRef current_function = NULL;
    if (llvm->stack != NULL && llvm->stack->function)
    {
        current_function = llvm->stack->function;
    }

    LLVMTypeRef type = get_llvm_type(llvm, variable->type_info);

    LLVMValueRef value;

    if (current_function != NULL)
    {
        value = LLVMBuildAlloca(llvm->builder, type, variable->name);
    }
    else
    {
        value = LLVMAddGlobal(llvm->module, type, variable->name);
        LLVMSetLinkage(value, LLVMExternalLinkage);
    }

    push_symbol(llvm, variable->name, value, type);

    if (variable->assignment)
    {
        llvm_visit_assignment(llvm, variable->assignment);
    }
}

LLVMBasicBlockRef llvm_visit_block(Llvm *llvm, Block *block, LLVMBasicBlockRef llvm_block)
{
    LLVMValueRef llvm_function = LLVMGetBasicBlockParent(llvm_block);
    if (block->statements != NULL)
    {
        push_scope(llvm, llvm_function);
        llvm_visit(llvm, block->statements);
        pop_scope(llvm);
    }
    return llvm_block;
}

void llvm_visit_function(Llvm *llvm, Function *function)
{
    LLVMTypeRef type = LLVMFunctionType(get_llvm_type(llvm, function->type_info), NULL, 0, 0);
    LLVMValueRef value = LLVMAddFunction(llvm->module, function->name, type);
    push_symbol(llvm, function->name, value, type);
    LLVMBasicBlockRef llvm_block = LLVMAppendBasicBlockInContext(llvm->context, value, "entry");
    LLVMPositionBuilderAtEnd(llvm->builder, llvm_block);
    llvm_visit_block(llvm, function->body, llvm_block);
}

void llvm_visit_if(Llvm *llvm, If *if_)
{
    LLVMValueRef function = llvm->stack->function;
    LLVMBasicBlockRef current_block = LLVMGetInsertBlock(llvm->builder);

    LLVMBasicBlockRef if_exit = NULL;

    while (if_)
    {
        LLVMBasicBlockRef if_check = LLVMAppendBasicBlockInContext(llvm->context, function, "if_check");
        LLVMBasicBlockRef if_body = LLVMAppendBasicBlockInContext(llvm->context, function, "if_body");

        if_exit = LLVMAppendBasicBlockInContext(llvm->context, function, "if_exit");

        // Add a branch instruction to the current if_check block
        LLVMPositionBuilderAtEnd(llvm->builder, current_block);
        LLVMBuildBr(llvm->builder, if_check);

        LLVMPositionBuilderAtEnd(llvm->builder, if_check);

        if (if_->condition)
        {
            LLVMValueRef cond_value = llvm_visit_expression(llvm, if_->condition);
            LLVMBuildCondBr(llvm->builder, cond_value, if_body, if_exit);
        }
        else
        {
            LLVMBuildBr(llvm->builder, if_body);
        }

        LLVMPositionBuilderAtEnd(llvm->builder, if_body);
        llvm_visit_block(llvm, if_->body, if_body);
        LLVMBuildBr(llvm->builder, if_exit);

        current_block = if_exit;
        if_ = if_->next;
    }

    LLVMPositionBuilderAtEnd(llvm->builder, if_exit);
}

void llvm_visit_while(Llvm *llvm, While *while_)
{
    LLVMValueRef function = llvm->stack->function;
    LLVMBasicBlockRef current_block = LLVMGetInsertBlock(llvm->builder);

    LLVMBasicBlockRef while_check_block = LLVMAppendBasicBlockInContext(llvm->context, function, "while_check");
    LLVMBasicBlockRef while_body_block = LLVMAppendBasicBlockInContext(llvm->context, function, "while_body");
    LLVMBasicBlockRef while_exit_block = LLVMAppendBasicBlockInContext(llvm->context, function, "while_exit");

    LLVMPositionBuilderAtEnd(llvm->builder, current_block);
    LLVMBuildBr(llvm->builder, while_check_block);

    LLVMPositionBuilderAtEnd(llvm->builder, while_check_block);
    LLVMValueRef cond_value = llvm_visit_expression(llvm, while_->condition);
    LLVMBuildCondBr(llvm->builder, cond_value, while_body_block, while_exit_block);

    LLVMPositionBuilderAtEnd(llvm->builder, while_body_block);
    llvm_visit(llvm, while_->body->statements);
    LLVMBuildBr(llvm->builder, while_check_block);

    LLVMPositionBuilderAtEnd(llvm->builder, while_exit_block);
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
    default:
        fprintf(stderr, "Unexpected node type: %d\n", node->node_type);
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
    // LLVMContextSetOpaquePointers(llvm->context, 0);
    llvm->module = LLVMModuleCreateWithNameInContext("default", llvm->context);
    llvm->builder = LLVMCreateBuilderInContext(llvm->context);
    push_scope(llvm, NULL);
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

void llvm_compile(Llvm *llvm)
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

    if (LLVMTargetMachineEmitToFile(target_machine, llvm->module, "output.o",
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