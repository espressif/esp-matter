/*
 * Copyright (c) 2014-2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Most functions can be inlined, and definitions are in common_functions.h.
 * Define COMMON_FUNCTIONS_FN before including it to generate external definitions.
 */
#define COMMON_FUNCTIONS_FN extern

#include "common_inlined_functions.h"

/* WI_SUN-467
 * common_functions.c was split to help mitigate an issue with IAR linker and inlining.
 * Normally the linker should behave as explained by the note below, but it does not.
 * https://www.iar.com/knowledge/support/technical-notes/compiler/linker-error-undefined-external-for-inline-functions/
 */
