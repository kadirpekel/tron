/******************************************************************************
 * Copyright [2023] [Kadir PEKEL]
 *
 * Licensed under the Apache License, Version 2.0 (the "License\n");
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
#include <stdarg.h>

#include "parser.h"

void next_token(Parser *p)
{
    Token *token = lex(p->l);
    while (token->token_type == T_SPACE || token->token_type == T_COMMENT)
    {
        dispose_token(token);
        token = lex(p->l);
    }
    p->token = token;
}

Parser *new_parser(FILE *file)
{
    Parser *p = malloc(sizeof(Parser));
    p->l = new_lexer(file);
    ScopeInfo *scope_info = new_scope_info(NULL, false);
    p->scope = push_scope(NULL, SCOPE_ROOT, scope_info, (void (*)(void *))dispose_scope_info, (void (*)(void *))dispose_type_info);
    p->depth = 0;

    // Global builtins
    // Types
    insert_symbol(p->scope, SYMBOL_TYPE, "int", new_type_info(TYPE_INT));
    insert_symbol(p->scope, SYMBOL_TYPE, "float", new_type_info(TYPE_FLOAT));
    // Functions
    insert_symbol(p->scope, SYMBOL_FUNCTION, "print_int", new_type_info(TYPE_INT));

    next_token(p);
    return p;
}

void dispose_parser(Parser *p)
{
    dispose_lexer(p->l);
    pop_scope(p->scope);
    free(p);
}

void parse_error(Parser *p, char *msg)
{
    fprintf(stderr, "Syntax Error <%d:%d> %s\n",
            p->l->line,
            p->l->col,
            msg);
    exit(EXIT_FAILURE);
}

Token *accept_token(Parser *p, int num_types, ...)
{
    Token *token = NULL;
    va_list args;
    va_start(args, num_types);
    for (int i = 0; i < num_types; i++)
    {
        TokenType token_type = va_arg(args, TokenType);
        if (p->token->token_type == token_type)
        {
            token = p->token;
            next_token(p);
            break;
        }
    }
    va_end(args);
    return token;
}

Token *expect_token(Parser *p, int count, ...)
{
    va_list args;
    Token *token;

    va_start(args, count);
    for (int i = 0; i < count; i++)
    {
        TokenType tokenType = va_arg(args, TokenType);
        if ((token = accept_token(p, tokenType)) != NULL)
        {
            va_end(args);
            return token;
        }
    }
    va_end(args);

    parse_error(p, "Unexpected token");
    return NULL;
}

Token *accept_keyword(Parser *p, char *keyword)
{
    Token *token = NULL;

    if ((p->token->token_type & T_NAME) != 0 && strcmp(p->token->buffer, keyword) == 0)
    {
        token = accept_token(p, 1, T_NAME);
    }
    return token;
}

Symbol *accept_type(Parser *p)
{
    if ((p->token->token_type & T_NAME) != 0)
    {
        Symbol *symbol = lookup_symbol(p->scope, p->token->buffer);
        if (symbol != NULL && symbol->type == SYMBOL_TYPE)
        {

            accept_token(p, 1, T_NAME);
            return symbol;
        }
    }
    return NULL;
}

void enter_scope(Parser *p, ScopeType scope_type, ScopeInfo *scope_info)
{
    p->scope = push_scope(p->scope, scope_type, scope_info, NULL, NULL);
    p->depth++;
}

void exit_scope(Parser *p)
{
    p->scope = pop_scope(p->scope);
    p->depth--;
}

Expression *parse_array(Parser *p)
{
    dispose_token(expect_token(p, 1, T_LBRACE));

    Expression *elements = parse_expressions(p);

    dispose_token(expect_token(p, 1, T_RBRACE));

    TypeInfo *type_info;
    if (elements != NULL)
    {
        type_info = dup_type_info(elements->type_info);

        Expression *current = elements;
        int size = 0;
        while (current != NULL)
        {
            size++;
            current = current->next;
        }
        type_info->array_info = new_array_info(size);
    }
    else
    {
        type_info = new_type_info(TYPE_INFER);
        type_info->array_info = new_array_info(-1);
    }

    return new_expression(
        NULL,
        NULL,
        NULL,
        new_node(N_ARRAY, elements),
        type_info);
}

TypeInfo *parse_type_info(Parser *p)
{
    TypeInfo *type_info = NULL;

    Symbol *symbol;
    if ((symbol = accept_type(p)) != NULL)
    {
        type_info = dup_type_info(symbol->info);

        ArrayInfo *current;
        Token *lbracket_token;
        while ((lbracket_token = accept_token(p, 1, T_LBRACKET)))
        {
            if (current == NULL)
            {
                current = new_array_info(-1);
                type_info->array_info = current;
            }
            else
            {
                current->next = new_array_info(-1);
                current = current->next;
            }

            Token *integer_token;
            if ((integer_token = accept_token(p, 1, T_INTEGER)))
            {
                current->size = atoi(integer_token->buffer);
            }
            dispose_token(expect_token(p, 1, T_RBRACKET));
            dispose_token(lbracket_token);
        }
    }
    return type_info;
}

TypeInfo *parse_type_infos(Parser *p)
{
    TypeInfo *type_info = NULL;
    Token *lparen_token = NULL;

    if ((lparen_token = accept_token(p, 1, T_LPAREN)) != NULL)
    {
        type_info = parse_type_info(p);
        if (type_info == NULL)
        {
            parse_error(p, "Type info is missing");
        }

        Token *commaToken;
        TypeInfo *current = type_info;
        while ((commaToken = accept_token(p, 1, T_COMMA)) != NULL)
        {
            TypeInfo *next = parse_type_info(p);
            if (next == NULL)
            {
                parse_error(p, "Type info is missing");
            }
            current->next = next;
            current = current->next;
            dispose_token(commaToken);
        }

        dispose_token(expect_token(p, 1, T_RPAREN));
        dispose_token(lparen_token);
    }
    else
    {
        type_info = parse_type_info(p);
    }

    return type_info;
}

Call *parse_call(Parser *p, Symbol *symbol)
{
    Call *call = NULL;
    Token *lparen_token = NULL;

    if ((lparen_token = accept_token(p, 1, T_LPAREN)) != NULL)
    {
        Expression *expression = parse_expression(p);

        Token *commaToken;
        Expression *current = expression;
        while ((commaToken = accept_token(p, 1, T_COMMA)) != NULL)
        {
            Expression *next = parse_expression(p);
            if (next == NULL)
            {
                parse_error(p, "Expression is missing");
            }
            current->next = next;
            current = next;
            dispose_token(commaToken);
        }

        TypeInfo *call_type_info = dup_type_info(symbol->info);
        call = new_call(symbol->name, call_type_info, expression);
        dispose_token(expect_token(p, 1, T_RPAREN));
        dispose_token(lparen_token);
    }
    return call;
}

Expression *parse_unary_expression(Parser *p)
{
    Token *opToken;
    if ((opToken = accept_token(p, 3, T_XOR, T_LOGICAL_NOT, T_SUB)) != NULL)
    {
        Expression *operand = parse_unary_expression(p);
        if (operand == NULL)
        {
            parse_error(p, "Operand is missing");
        }
        TypeInfo *type_info = dup_type_info(operand->type_info);
        Expression *expression = new_expression(
            opToken,
            operand,
            NULL,
            NULL,
            type_info);
        return expression;
    }

    return parse_factor(p);
}

Expression *parse_expression(Parser *p)
{
    return parse_binary_expression(p, 0);
}

Expression *parse_binary_expression(Parser *p, int min_precedence)
{
    Expression *left = parse_unary_expression(p);

    while (1)
    {
        Token *op_token = p->token;
        int precedence = -1;
        int i;
        for (i = 0; i < sizeof(PRECEDENCE_TABLE) / sizeof(PRECEDENCE_TABLE[0]); i++)
        {
            int num_types = sizeof(PRECEDENCE_TABLE[i]) / sizeof(PRECEDENCE_TABLE[i][0]);

            int j;
            for (j = 0; j < num_types; j++)
            {
                if (PRECEDENCE_TABLE[i][j] == op_token->token_type)
                {
                    precedence = i;
                    break;
                }
            }

            if (precedence >= 0)
            {
                break;
            }
        }

        if (precedence < min_precedence)
        {
            break;
        }

        next_token(p);

        Expression *right = parse_binary_expression(p, precedence + 1);
        if (right == NULL)
        {
            parse_error(p, "Expected expression after binary operator");
        }
        TypeInfo *type_info = dup_type_info(left->type_info);
        left = new_expression(op_token, left, right, NULL, type_info);
    }

    return left;
}

Expression *parse_expressions(Parser *p)
{
    Expression *expression = parse_expression(p);

    if (expression != NULL)
    {
        Token *commaToken;
        Expression *current = expression;
        while ((commaToken = accept_token(p, 1, T_COMMA)) != NULL)
        {
            Expression *next = parse_expression(p);
            if (next == NULL)
            {
                parse_error(p, "Expression is missing");
            }
            current->next = next;
            current = current->next;
            dispose_token(commaToken);
        }
    }

    return expression;
}

Expression *parse_factor(Parser *p)
{
    Expression *expression = NULL;

    if (p->token->token_type == T_LPAREN)
    {
        dispose_token(expect_token(p, 1, T_LPAREN));
        expression = parse_expression(p);
        dispose_token(expect_token(p, 1, T_RPAREN));
    }
    else if (p->token->token_type == T_LBRACE)
    {
        expression = parse_array(p);
    }
    else
    {
        Token *leaf_token;
        if ((leaf_token = accept_token(p, 3, T_INTEGER, T_FLOAT, T_NAME)) != NULL)
        {
            if (leaf_token->token_type == T_INTEGER)
            {
                expression = new_expression(
                    leaf_token,
                    NULL,
                    NULL,
                    new_node(N_INTEGER, new_integer(atoi(leaf_token->buffer))),
                    new_type_info(TYPE_INT));
            }
            else if (leaf_token->token_type == T_FLOAT)
            {
                expression = new_expression(
                    leaf_token,
                    NULL,
                    NULL,
                    new_node(N_FLOAT, new_float(atof(leaf_token->buffer))),
                    new_type_info(TYPE_FLOAT));
            }
            else if (leaf_token->token_type == T_NAME)
            {
                Symbol *symbol = lookup_symbol(p->scope, leaf_token->buffer);
                TypeInfo *expression_type_info = dup_type_info(symbol->info);
                if (symbol != NULL)
                {
                    if (symbol->type == SYMBOL_FUNCTION)
                    {
                        Call *call = parse_call(p, symbol);
                        if (call == NULL)
                        {
                            parse_error(p, "Function call missing");
                        }

                        expression = new_expression(
                            leaf_token,
                            NULL,
                            NULL,
                            new_node(N_CALL, call),
                            expression_type_info);
                    }
                    else if (symbol->type == SYMBOL_VARIABLE || symbol->type == SYMBOL_ARG)
                    {

                        expression = new_expression(
                            leaf_token,
                            NULL,
                            NULL,
                            new_node(N_NAME, new_name(leaf_token->buffer)),
                            expression_type_info);
                    }
                    else
                    {
                        parse_error(p, "Invalid symbol found");
                    }
                }
                else
                {
                    parse_error(p, "Symbol not found");
                }
            }
        }
    }
    return expression;
}

Assignment *parse_assignment(Parser *p, Symbol *symbol)
{
    Assignment *assignment = NULL;
    Token *assign_token = NULL;
    if ((assign_token = accept_token(p, 1, T_ASSIGN)) != NULL)
    {
        Expression *expression = parse_expression(p);
        if (expression == NULL)
        {
            parse_error(p, "Expression required");
        }

        TypeInfo *type_info = (TypeInfo *)symbol->info;

        if (type_info->type == TYPE_INFER)
        {
            type_info->type = expression->type_info->type;
        }
        else
        {
            if (type_info->type != expression->type_info->type)
            {
                parse_error(p, "Variable type does not match with expression type");
            }
        }
        TypeInfo *assignment_type_info = dup_type_info(type_info);
        assignment = new_assignment(symbol->name, assignment_type_info, expression);
        dispose_token(assign_token);
    }
    return assignment;
}

Variable *parse_param(Parser *p, SymbolType symbol_type)
{
    Variable *param = NULL;
    Token *name_token;

    if ((name_token = accept_token(p, 1, T_NAME)) != NULL)
    {
        TypeInfo *type_info = NULL;

        Token *colon_token;
        if ((colon_token = accept_token(p, 1, T_COLON)) != NULL)
        {
            type_info = parse_type_info(p);

            if (type_info == NULL)
            {
                parse_error(p, "Type info is missing");
            }
            dispose_token(colon_token);
        }
        else
        {
            type_info = new_type_info(TYPE_INFER);
        }

        Symbol *symbol = insert_symbol(p->scope, symbol_type, name_token->buffer, type_info);
        if (symbol == NULL)
        {
            parse_error(p, "Symbol already exists");
        }

        Assignment *assignment = parse_assignment(p, symbol);
        if (type_info->type == TYPE_INFER && assignment == NULL)
        {
            parse_error(p, "Variable type can not be resolved");
        }

        TypeInfo *variable_type_info = dup_type_info(type_info);
        param = new_variable(name_token->buffer, variable_type_info, assignment);
        dispose_token(name_token);
    }
    return param;
}

Variable *parse_variable(Parser *p)
{
    Variable *param = NULL;
    Token *var_token;
    if ((var_token = accept_keyword(p, VAR)) != NULL)
    {
        param = parse_param(p, SYMBOL_VARIABLE);
        if (param == NULL)
        {
            parse_error(p, "Variable not initialized");
        }
        dispose_token(expect_token(p, 1, T_SEMICOLON));
    }
    return param;
}

Break *parse_break(Parser *p)
{
    Break *break_ = NULL;
    Token *break_token;
    if ((break_token = accept_keyword(p, BREAK)) != NULL)
    {
        Function *while_ref = find_enclosing_scope_info(p->scope, SCOPE_WHILE);
        if (while_ref == NULL)
        {
            parse_error(p, "Break statements are only allowed in loop scopes");
        }
        break_ = new_break();
        dispose_token(expect_token(p, 1, T_SEMICOLON));
        dispose_token(break_token);
    }
    return break_;
}

Continue *parse_continue(Parser *p)
{
    Continue *continue_ = NULL;
    Token *continue_token;
    if ((continue_token = accept_keyword(p, CONTINUE)) != NULL)
    {
        ScopeInfo *scope_info = find_enclosing_scope_info(p->scope, SCOPE_WHILE);
        if (scope_info == NULL)
        {
            parse_error(p, "Continue statements are only allowed in loop scopes");
        }
        continue_ = new_continue();
        dispose_token(expect_token(p, 1, T_SEMICOLON));
        dispose_token(continue_token);
    }
    return continue_;
}

Return *parse_return(Parser *p)
{
    Return *return_ = NULL;
    Token *return_token;
    if ((return_token = accept_keyword(p, RETURN)) != NULL)
    {
        ScopeInfo *scope_info = find_enclosing_scope_info(p->scope, SCOPE_FUNCTION);
        if (scope_info == NULL)
        {
            parse_error(p, "Return statements are not allowed at root level");
        }

        return_ = new_return(parse_expression(p));

        if (scope_info->function->type_info->type == TYPE_INFER)
        {
            if (return_->expression->type_info->type == TYPE_INFER)
            {
                parse_error(p, "Can not infer the return type");
            }
            else
            {
                TypeInfo *function_type_info = dup_type_info(return_->expression->type_info);
                dispose_type_info(scope_info->function->type_info);
                scope_info->function->type_info = function_type_info;
            }
        }
        else
        {
            if (return_->expression->type_info->type != scope_info->function->type_info->type)
            {
                parse_error(p, "Returned type should match the enclosing function type");
            }
        }

        dispose_token(expect_token(p, 1, T_SEMICOLON));
        dispose_token(return_token);
    }
    return return_;
}

Block *parse_block(Parser *p)
{
    dispose_token(expect_token(p, 1, T_LBRACE));
    Block *block = new_block(parse(p));
    dispose_token(expect_token(p, 1, T_RBRACE));
    return block;
}

Variable *parse_params(Parser *p, SymbolType symbol_type)
{
    Variable *param = parse_param(p, symbol_type);
    Variable *current = param;
    Token *comma_token = accept_token(p, 1, T_COMMA);
    while (comma_token != NULL)
    {
        current->next = parse_param(p, symbol_type);
        current = current->next;
        comma_token = accept_token(p, 1, T_COMMA);
    }
    return param;
}

Function *parse_function(Parser *p)
{
    Function *function = NULL;
    Token *def_token;

    if ((def_token = accept_keyword(p, FUNCTION)) != NULL)
    {
        if (p->scope->parent != NULL)
        {
            parse_error(p, "Functions are only allowed at root level");
        }

        Token *name_token = expect_token(p, 1, T_NAME);

        function = new_function(name_token->buffer, NULL, NULL, NULL);
        enter_scope(p, SCOPE_FUNCTION, new_scope_info(function, false));

        dispose_token(expect_token(p, 1, T_LPAREN));
        function->params = parse_params(p, SYMBOL_ARG);
        dispose_token(expect_token(p, 1, T_RPAREN));

        Token *colon_token = NULL;
        if ((colon_token = accept_token(p, 1, T_COLON)) != NULL)
        {
            function->type_info = parse_type_info(p);

            if (function->type_info == NULL)
            {
                parse_error(p, "Type info is missing");
            }
            dispose_token(colon_token);
        }

        if (function->type_info == NULL)
        {
            function->type_info = new_type_info(TYPE_INFER);
        }

        TypeInfo *symbol_type_info = dup_type_info(function->type_info);
        if (!insert_symbol(p->scope->parent, SYMBOL_FUNCTION, function->name, symbol_type_info))
        {
            parse_error(p, "Symbol already exists");
        }

        function->body = parse_block(p);
        exit_scope(p);

        if (function->body == NULL)
        {
            parse_error(p, "Function body is missing");
        }

        dispose_token(name_token);
        dispose_token(def_token);
    }
    return function;
}

If *parse_single_if(Parser *p)
{
    If *if_ = NULL;
    Token *if_token;

    if ((if_token = accept_keyword(p, IF)) != NULL)
    {

        ScopeInfo *scope_info = find_enclosing_scope_info(p->scope, SCOPE_FUNCTION);
        if (scope_info == NULL)
        {
            parse_error(p, "If statements are not allowed at root level");
        }

        dispose_token(expect_token(p, 1, T_LPAREN));
        Expression *condition = parse_expression(p);
        if (condition == NULL)
        {
            parse_error(p, "Condition expression is missing");
        }
        dispose_token(expect_token(p, 1, T_RPAREN));

        enter_scope(p, SCOPE_IF, new_scope_info(NULL, false));
        Block *body = parse_block(p);
        exit_scope(p);

        if (body == NULL)
        {
            parse_error(p, "Condition body is missing");
        }

        if_ = new_if(condition, body);

        dispose_token(if_token);
    }
    return if_;
}

If *parse_if(Parser *p)
{
    If *if_ = parse_single_if(p);

    if (if_ != NULL)
    {

        If *current = if_;

        Token *else_token;
        while ((else_token = accept_keyword(p, ELSE)) != NULL)
        {

            If *next = parse_single_if(p);

            if (next == NULL)
            {
                enter_scope(p, SCOPE_IF, new_scope_info(NULL, false));
                Block *body = parse_block(p);
                exit_scope(p);
                if (body != NULL)
                {
                    current->next = new_if(NULL, body);
                    break;
                }
                else
                {
                    parse_error(p, "Else statements is missing");
                }
            }

            current->next = next;
            current = next;
        }
    }

    return if_;
}

While *parse_while(Parser *p)
{
    While *while_ = NULL;
    Token *while_token;

    if ((while_token = accept_keyword(p, WHILE)) != NULL)
    {
        ScopeInfo *scope_info = find_enclosing_scope_info(p->scope, SCOPE_FUNCTION);
        if (scope_info == NULL)
        {
            parse_error(p, "While statements are not allowed at root level");
        }

        dispose_token(expect_token(p, 1, T_LPAREN));
        Expression *condition = parse_expression(p);
        if (condition == NULL)
        {
            parse_error(p, "Condition is missing");
        }
        dispose_token(expect_token(p, 1, T_RPAREN));

        enter_scope(p, SCOPE_WHILE, new_scope_info(NULL, true));
        Block *body = parse_block(p);
        exit_scope(p);

        if (body == NULL)
        {
            parse_error(p, "Body is missing");
        }
        while_ = new_while(condition, body);
        dispose_token(while_token);
    }
    return while_;
}

Node *parse_namebiguity(Parser *p)
{
    Node *node = NULL;
    Token *name_token;

    if ((name_token = accept_token(p, 1, T_NAME)) != NULL)
    {
        Symbol *symbol = lookup_symbol(p->scope, name_token->buffer);

        if (symbol != NULL)
        {
            if (symbol->type == SYMBOL_VARIABLE || symbol->type == SYMBOL_ARG)
            {
                Assignment *assignment = parse_assignment(p, symbol);
                if (assignment == NULL)
                {
                    parse_error(p, "Variable assignment missing");
                }
                dispose_token(expect_token(p, 1, T_SEMICOLON));
                node = new_node(N_ASSIGNMENT, assignment);
            }
            else if (symbol->type == SYMBOL_FUNCTION)
            {
                Call *call = parse_call(p, symbol);
                if (call == NULL)
                {
                    parse_error(p, "Function call missing");
                }
                dispose_token(expect_token(p, 1, T_SEMICOLON));
                node = new_node(N_CALL, call);
            }
            else
            {
                parse_error(p, "Invalid symbol");
            }
        }
        else
        {
            parse_error(p, "Symbol not found");
        }

        dispose_token(name_token);
    }
    return node;
}

Node *parse_statement(Parser *p)
{
    Function *function = parse_function(p);
    if (function != NULL)
    {
        return new_node(N_FUNCTION, function);
    }

    If *if_ = parse_if(p);
    if (if_ != NULL)
    {
        return new_node(N_IF, if_);
    }

    While *while_ = parse_while(p);
    if (while_ != NULL)
    {
        return new_node(N_WHILE, while_);
    }

    Break *break_ = parse_break(p);
    if (break_ != NULL)
    {
        return new_node(N_BREAK, break_);
    }

    Continue *continue_ = parse_continue(p);
    if (continue_ != NULL)
    {
        return new_node(N_CONTINUE, continue_);
    }

    Return *return_ = parse_return(p);
    if (return_ != NULL)
    {
        return new_node(N_RETURN, return_);
    }

    Variable *variable = parse_variable(p);
    if (variable != NULL)
    {
        return new_node(N_VARIABLE, variable);
    }

    Node *node = parse_namebiguity(p);
    if (node != NULL)
    {
        return node;
    }

    return NULL;
}

Node *parse(Parser *p)
{
    Node *node = parse_statement(p);
    Node *current = node;
    while (current)
    {
        current->next = parse_statement(p);
        current = current->next;
    }
    return node;
}