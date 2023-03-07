/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef STRING_OPS_HPP
#define STRING_OPS_HPP

#include <cstddef>
#include <string>

using namespace std;

// Replace first occurrence of find_str within orig of replace_str:
size_t find_replace_1st (const string &find_str, const string &replace_str,
                         string &orig);

// Replace all occurrences of find_str in "this" string, with replace_str:
size_t find_replace_all (const string &find_str, const string &replace_str,
                         string &orig);

/* In both of the above string-replacement functions, the return value is start
   offset to the (last) occurrence of "find_str" within "orig." */

#endif  // #ifndef STRING_OPS_HPP
