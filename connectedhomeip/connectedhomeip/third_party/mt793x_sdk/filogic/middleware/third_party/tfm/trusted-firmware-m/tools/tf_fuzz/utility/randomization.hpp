/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef RANDOMIZATION_HPP
#define RANDOMIZATION_HPP

#include <string>

using namespace std;

string rand_key_usage (void);

string rand_key_algorithm (void);

string rand_key_type (void);

#endif  /* #ifndef RANDOMIZATION_HPP */
