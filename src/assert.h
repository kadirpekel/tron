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

#ifndef MASSERT_H_
#define MASSERT_H_

#include <stdio.h>

// Helper Macros

#define fatal(argv...) \
    printf(argv);      \
    printf("\n");      \
    fflush(stderr);    \
    exit(1);
#define error(argv...) \
    printf(argv);      \
    printf("\n");      \
    fflush(stderr);    \
    exit(1);
#define log(argv...) \
    printf(argv);    \
    printf("\n");    \
    fflush(stdout);

#endif
