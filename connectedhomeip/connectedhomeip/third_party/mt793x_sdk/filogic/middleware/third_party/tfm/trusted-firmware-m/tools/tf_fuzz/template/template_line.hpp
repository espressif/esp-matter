/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Objects typed to subclasses of the these classes are constructed and filled in
   by the parser as it parses template lines.  There is not necessarily a one-to-one
   correspondence between the template lines and either the PSA commands generated
   nor PSA assets they manipulate.  PSA assets (which persist through the test) and
   PSA commands are therefore tracked in separate objects, but referenced here. */

#ifndef TEMPLATE_LINE_HPP
#define TEMPLATE_LINE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <new>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
*/


using namespace std;

class template_line
{
public:
    // Data members:  // low value to hide these behind setters and getters
        // Everything about the test -- accumulated PSA commands, assets, etc.:
        tf_fuzz_info *test_state;
        /* Note:  The following are pointers to the psa_asset and psa_call currently
           being worked with.  These objects these are first placed on the appropriate
           vector in test_state of such objects, and then referenced here. */
        psa_asset_type asset_type;  // SST vs. key vs. policy (etc.)
        expect_info expect;  // everything about expected results
        set_data_info set_data;  // everything about setting PSA-asset data
        asset_name_id_info asset_id;  // everything about the asset(s) for this line
        long call_ser_no;  // unique identifer for psa_call tracker object
        long asset_ser_no;  // same for psa_asset objects
            /* Note:  Regarding the above serial numbers, we can't save away iterators
                      nor pointers, because STL vectors could get relocated.  Must
                      explicitly search immediately prior to using these objects! */
        psa_asset_usage random_asset;
            /* if asked to use some random asset from active or deleted, this says
               which.  psa_asset_usage::all if not using this feature. */
        asset_search how_asset_found;
        bool assign_data_var_specified;
        string assign_data_var;  // name of variable to dump (assign) data into
        bool print_data;  // true to print asset data to test log
        bool hash_data;  // true to hash data for later comparison
        /* Vectors of asset names or IDs.  These are used to create several similar
           PSA calls from a single template line. */
        bool is_remove;  // true if this template line is to remove an asset
    // Methods (mostly for calling from within yyparse()):
        virtual bool copy_template_to_asset (void) = 0;
        virtual bool copy_template_to_call (void) = 0;
        virtual void setup_call (set_data_info set_data, bool random_data,
                                 bool fill_in_template, bool create_call,
                                 template_line *temLin, tf_fuzz_info *rsrc) = 0;
        template_line (tf_fuzz_info *resources);  // (constructor)
        virtual ~template_line (void);

protected:  // a lot simpler to just let subclasses access these directly
    // Data members:
        // Parallel vectors of PSA-asset info, by asset identified:
        /* Not all template lines involve expected data, but all that do use it,
           use it the same, so include it here. */
        string asset_name;  // parsed from template, assigned to psa_asset object
        string flags_string;  // creation flags
    // Methods:

private:
    // Data members:
    // Methods:
};


/* Note:  The following are sub-classed from template_line (above), and then further
          subclassed in sst_template_line.*pp, crypto_template_line.*pp, etc.  Concep-
          tually, these subclasses might be more logically put in those sub-classing
          files, but this gives a more-even balance of file size and complexity. */

class sst_template_line : public template_line
{
public:
    // Data members:
        // SST-asset info:
        // PSA-call info:
    // Methods:
        sst_template_line (tf_fuzz_info *resources);  // (constructor)
        ~sst_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class key_template_line : public template_line
{
public:
    // Data members:
        string handle_str;  // the text name of the key's "handle"
        string key_type;  // DES, AES, RAW, vendor, none, etc.
        string lifetime_str;  // similarly, the text representation of the key's lifetime
        string expected_n_bits;
           // for get_key_info call (possibly others) exected key size in bits
    // Methods:
        string make_id_based_name (uint64_t id_n, string &name);  // create ID-based asset name
        key_template_line (tf_fuzz_info *resources);  // (constructor)
        ~key_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class policy_template_line : public template_line
{
public:
    // Data members:
        string policy_usage;
        string policy_algorithm;
    // Methods:
        policy_template_line (tf_fuzz_info *resources);  // (constructor)
        ~policy_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
        string make_id_based_name (uint64_t id_n, string &name);  // create ID-based asset name
};


class security_template_line : public template_line
{
public:
    // Data members:  // low value to hide these behind setters and getters
        // Everything about the test -- accumulated PSA commands, assets, etc.:
    // Methods (for calling from within yyparse()):
        security_template_line (tf_fuzz_info *resources);  // (constructor)
        ~security_template_line (void);

protected:  // a lot simpler to just let subclasses access these directly
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


#endif  // TEMPLATE_LINE_HPP
