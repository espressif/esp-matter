/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* The .hpp files in this project being so self-referential, this file forward-
   declares all classes before any are attempted to be used. */

#ifndef CLASS_FORWARDS_HPP
#define CLASS_FORWARDS_HPP

using namespace std;

// data_blocks.hpp:
class expect_info;

// template_line.hpp:
class template_line;
class sst_template_line;
class key_template_line;
class policy_template_line;

// sst_template_line.hpp:
class set_sst_template_line;
class remove_sst_template_line;
class read_sst_template_line;

// crypto_template_line.hpp:
class set_key_template_line;
class remove_key_template_line;
class read_key_template_line;
class set_policy_template_line;
class read_policy_template_line;

// security.hpp:
class security;
class security_hash;

// psa_call.hpp:
class psa_call;

// sst_call.hpp:
class sst_call;
class sst_set_call;
class sst_get_call;
class sst_remove_call;

// crypto_call.hpp:
class crypto_call;
class policy_set_call;
class policy_get_call;
class set_key_call;
class get_key_info_call;
class destroy_key_call;

// psa_asset.hpp:
class psa_asset;

// sst_asset.hpp:
class sst_asset;

// crypto_asset.hpp:
class crypto_asset;
class key_asset;
class policy_asset;
class key_asset;

// boilerplate.hpp"
//enum class boilerplate_texts;  not really a "class," and no need to forward-reference it anyway
class boilerplate;

// gibberish.hpp:
class gibberish;

// tf_fuzz.hpp:
class tf_fuzz_info;

#endif  // #ifndef CLASS_FORWARDS_HPP
