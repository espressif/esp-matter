/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PSA_CALL_HPP
#define PSA_CALL_HPP

#include <string>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "tf_fuzz.hpp"
*/


using namespace std;

class psa_call
{
public:
    /* Data members -- not all PSA calls have/need these, but they need to be acces-
       sible polymorphically via a psa_call iterator: */
        expect_info expect;  // everything about expected results
        set_data_info set_data;  // everything about setting PSA-asset-data values
        string assign_data_var;  // name of variable to dump (assign) data into
        asset_name_id_info asset_id;  // everything about the asset(s) for this line
        bool assign_data_var_specified;
        // Expected-result info:
        bool print_data;  // true to print asset data to test log
        bool hash_data;  // true to hash data for later comparison
        string id_string;  // not all PSA calls involve an ID, but a diverse set do
        long asset_ser_no;  // unique ID for psa asset needed to find data string
        long call_ser_no;  // unique serial# for this psa_call (see note in tf_fuzz.hpp)
        asset_search how_asset_found;
        tf_fuzz_info *test_state;  // the big blob with pointers to everything going on
        string flags_string;
            // creation flags, nominally for SST but have to be in a vector of base-class
    // Methods:
        virtual void fill_in_prep_code (void) = 0;
        virtual void fill_in_command (void) = 0;
        void write_out_prep_code (ofstream &test_file);
        void write_out_command (ofstream &test_file);
        void write_out_check_code (ofstream &test_file);
        psa_call (tf_fuzz_info *test_state, long &asset_ser_no,
                  asset_search how_asset_found);  // (constructor)
        ~psa_call (void);

protected:
    // Data members:
        string prep_code;  // declarations and such prior to all of the calls
        string call_code;  // for the call itself
        string check_code;  // for the code to check success of the call
        static long unique_id_counter;  // counts off unique IDs for assets
    // Methods:
        virtual void calc_result_code (void) = 0;

private:
    // Data members:
    // Methods:
};


class sst_call : public psa_call
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
    // Methods:
        sst_call (tf_fuzz_info *test_state, long &asset_ser_no,
                  asset_search how_asset_found);  // (constructor)
        ~sst_call (void);

protected:
    // Data members:
    // Methods:
        void calc_result_code (void);

private:
    // Data members:
    // Methods:
};

class crypto_call : public psa_call
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
        key_asset the_key_asset;  // are these used (yet)?
        policy_asset the_policy_asset;
            // the policy (if any) involved in this call
    // Methods:
        crypto_call (tf_fuzz_info *test_state, long &asset_ser_no,
                    asset_search how_asset_found);  // (constructor)
        ~crypto_call (void);

protected:
    // Data members:
    // Methods:
        void calc_result_code (void);
           // for now, the method-overide buck stops here, but that'll probably change

private:
    // Data members:
    // Methods:
};

class security_call : public psa_call
   /* Strictly speaking, these don't really correspond to PSA calls, so it's a little
      iffy to subclass them from psa_call.  However, the calling patterns work out
      right. */
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
    // Methods:
        security_call (tf_fuzz_info *test_state, long &asset_ser_no,
                       asset_search how_asset_found);  // (constructor)
        ~security_call (void);

protected:
    // Data members:
    // Methods:
        void calc_result_code (void);
           // Should never be invoked, since security calls generate no PSA calls.

private:
    // Data members:
    // Methods:
};

#endif  // PSA_CALL_HPP
