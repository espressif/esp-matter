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
 *      This file defines test constants for the Nest Labs byte
 *      ordering test suite.
 *
 */

#ifndef NLBYTEORDER_TEST_H
#define NLBYTEORDER_TEST_H

#define MAGIC16         0x0123U
#define MAGIC32         0x01234567UL
#define MAGIC64         0x0123456789ABCDEFULL

#define MAGIC_SWAP16    0x2301U
#define MAGIC_SWAP32    0x67452301UL
#define MAGIC_SWAP64    0xEFCDAB8967452301ULL

int nlbyteorder(void);

#endif /* NLBYTEORDER_TEST_H */
