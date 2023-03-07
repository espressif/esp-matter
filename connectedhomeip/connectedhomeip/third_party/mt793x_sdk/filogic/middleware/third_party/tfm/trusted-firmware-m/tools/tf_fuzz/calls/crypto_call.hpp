/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CRYPTO_CALL_HPP
#define CRYPTO_CALL_HPP

#include <string>
#include <vector>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "psa_call.hpp"
#include "crypto_asset.hpp"
*/


using namespace std;

class policy_call : public crypto_call
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
        string policy_usage;
        string policy_algorithm;
    // Methods:
        policy_call (tf_fuzz_info *test_state, long &asset_ser_no,
                    asset_search how_asset_found);  // (constructor)
        ~policy_call (void);

protected:
    // Data members:
    // Methods:
//        void calc_result_code (void);  for *now* keep this in crypto_call::

private:
    // Data members:
    // Methods:
};


class key_call : public crypto_call
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
        string handle_str;  // the text name of the key's "handle"
        string key_type;  // DES, AES, RAW, vendor, none, ...
        string lifetime_str;
            // similarly, textual representation of the key's lifetime
        string expected_n_bits;
           // for get_key_info call (possibly others) exected key size in bits
    // Methods:
        key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                    asset_search how_asset_found);  // (constructor)
        ~key_call (void);

protected:
    // Data members:
    // Methods:
//        void calc_result_code (void);  for *now* keep this in crypto_call::

private:
    // Data members:
    // Methods:
};


class policy_set_call : public policy_call
{
public:
    // Data members:
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        policy_set_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~policy_set_call (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class policy_get_call : public policy_call
{
public:
    // Data members:
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        policy_get_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~policy_get_call (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    string policy_name;
    // Methods:
};


class set_key_call : public key_call
{
public:
    // Data members:
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        set_key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~set_key_call (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class get_key_info_call : public key_call
{
public:
    // Data members:
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        get_key_info_call (tf_fuzz_info *test_state, long &asset_ser_no,
                           asset_search how_asset_found);  // (constructor)
        ~get_key_info_call (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class destroy_key_call : public key_call
{
public:
    // Data members:
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        destroy_key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                          asset_search how_asset_found);  // (constructor)
        ~destroy_key_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif  // CRYPTO_CALL_HPP
