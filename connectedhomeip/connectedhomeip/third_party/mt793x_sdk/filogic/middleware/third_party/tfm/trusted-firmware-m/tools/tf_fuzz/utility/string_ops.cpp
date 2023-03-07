/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "string_ops.hpp"

using namespace std;

// Replace first occurrence of find_str within orig of replace_str:
size_t find_replace_1st (const string &find_str, const string &replace_str,
                         string &orig) {
    size_t where = 0;
    where = orig.find(find_str, where);
    if (where != string::npos) {
        orig.replace(where, find_str.length(), replace_str);
    }
    return where;
}

// Replace all occurrences of find_str in "this" string, with replace_str:
size_t find_replace_all (const string &find_str, const string &replace_str,
                         string &orig) {
    size_t where = 0;
    do {
        where = orig.find(find_str, where);
        if (where != string::npos) {
            orig.replace(where, find_str.length(), replace_str);
        }
    } while (where != string::npos);
    return where;
}
