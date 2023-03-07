/**
 *    Copyright 2012-2016 Nest Labs Inc. All Rights Reserved.
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
 *      This file defines test constants for the Nest Labs memory-mapped
 *      I/O test suite.
 *
 */

#ifndef NLIO_TEST_H
#define NLIO_TEST_H

#define MAGIC_8         0xA1U
#define MAGIC_16        0xB1B2U
#define MAGIC_32        0xC1C2C4C8U
#define MAGIC_64        0xD1D2D4D8E1E2E1E8U

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    long double m;
} _uint128_t;

#ifdef __cplusplus
}
#endif

#endif /* NLIO_TEST_H */
