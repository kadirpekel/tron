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

Llvm *new_llvm(char *module_name)
{
    Llvm *llvm = malloc(sizeof(Llvm));
    if (module_name == NULL)
    {
        module_name = "default";
    }

    llvm->context = LLVMContextCreate();
    llvm->module = LLVMModuleCreateWithNameInContext(module_name, llvm->context);
    llvm->builder = LLVMCreateBuilderInContext(llvm->context);
    return llvm;
}

void dispose_llvm(Llvm *llvm)
{
    LLVMDisposeBuilder(llvm->builder);
    LLVMDisposeModule(llvm->module);
    LLVMContextDispose(llvm->context);
    free(llvm);
}
