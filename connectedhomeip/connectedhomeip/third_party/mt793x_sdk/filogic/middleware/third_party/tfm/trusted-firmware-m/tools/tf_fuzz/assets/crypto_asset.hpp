/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CRYPTO_ASSET_HPP
#define CRYPTO_ASSET_HPP

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>


/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "psa_asset.hpp"
*/


using namespace std;

class crypto_asset : public psa_asset
{
public:
    // Data members:
    // Methods:
        crypto_asset (void);  // (constructor)
        ~crypto_asset (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class policy_asset : public crypto_asset
{
public:
    // Data members:
        string policy_usage;  // for now just strings;  maybe future tap TF-M(?) value list
        string key_type;  // DES, AES, RAW, vendor, none, etc.
        string policy_algorithm;
        vector<key_asset*> keys;  // keys that use this policy
    // Methods:
        policy_asset (void);  // (constructor)
        ~policy_asset (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class key_asset : public crypto_asset
{
public:
    // Data members:
        vector<policy_asset*>::iterator the_policy_asset;
            /* The policy for this key.  Note that psa_make_key() lets us create
             a key without associating a policy with it.  In that case, this will
             be null, and the attributes below apply.  Later, psa_set_key_policy
             lets us associate a policy with a key, at which point this becomes
             non-null and the following attributes no longer apply. */
        string key_type;  // DES, AES, RAW, vendor, none, etc.
        string usage;  // for now just strings;  maybe future tap TF-M(?) value list
        string alg;  // these only apply if the string was created without a policy
        string lifetime_str;  // similarly, the text representation of the key's lifetime
        string handle_str;  // the text name of the key's "handle"
    // Methods:
        bool set_key_id (int id_n);  // checks key-ID value, returns true==success
        key_asset (void);  // (constructor)
        ~key_asset (void);

protected:
    // Data members:
        uint64_t key_id;
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif  // CRYPTO_ASSET_HPP
