/**
 *    Copyright 2017 Nest Labs Inc. All Rights Reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file defines private macros and interfaces.
 */

#ifndef NLIO_PRIVATE_H
#define NLIO_PRIVATE_H

/*
 * If we are compiling under clang, GCC, or any such compatible
 * compiler, in which -fno-builtins or -ffreestanding might be
 * asserted, thereby eliminating built-in function optimization, we
 * may STILL want to leverage built-ins.
 *
 * Provide an internal convenience macro to do so.
 */

/**
 *  @def __nlIOHasBuiltin
 *
 *  @brief
 *     Determines whether or not the compiler in effect has support
 *     for the specified built-in function.
 *
 */
#ifdef __clang__
#define __nlIOHasBuiltin(...) __has_builtin(__VA_ARGS__)
#elif defined __GNUC__
#define __nlIOHasBuiltin(...) 1
#else
#define __nlIOHasBuiltin(...) 0
#endif /* __clang__ */

#endif /* NLIO_PRIVATE_H */
