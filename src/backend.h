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

#ifndef MBACKEND_H_
#define MBACKEND_H_

#include "node.h"

typedef struct Backend
{
    void (*visit_variable)(Variable *variable);
    void (*visit_call)(Call *call);
    void (*visit_function)(Function *function);
    void (*visit_while)(While *while_);
    void (*visit_if)(If *if_);
    void (*visit_return)(Return *return_);
    void (*visit_assignment)(Assignment *assignment_);
} Backend;

#endif
