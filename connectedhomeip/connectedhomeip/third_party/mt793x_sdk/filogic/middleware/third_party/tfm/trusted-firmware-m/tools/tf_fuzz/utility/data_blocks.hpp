/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string>

/* These classes "cut down the clutter" by grouping together related data and
   associated methods (most importantly their constructors) used in template_
   line, psa_call, psa_asset (etc.). */

#ifndef DATA_BLOCKS_HPP
#define DATA_BLOCKS_HPP

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.  However these in particular are mostly axiomatic:  Not
   dependent upon other classes. */


using namespace std;


/**********************************************************************************
  Class expect_info is all about expected data and expected pass/fail information.
  The members are therefore broken down with prefixes pf_ (for pass/fail) or
  data_.  Pass/fail, is broadly:
  *  "Pass" == the test passes
  *  "Specified" == some specified failure (e.g., no such asset)
  *  "Nothing" == no expectation
  Expected data refers to psa-asset data values, generally after reading them.
  Currently, they are limited to character strings, but that will probably be
  generalized in the future.
**********************************************************************************/

class expect_info
{
public:
    // Data members:
        bool pf_nothing;  // true to not generate results-check(s)
        bool pf_pass;  // if !expect.pf_nothing, then pass is expected
        bool pf_specified;
            /* if !pf_nothing && !pf_pass, then
               true == expected result was specified
               false == tf_fuzz must model expected result */
        bool data_specified;  // (literal data specified)
        string data;  // expected data from reading an asset
        bool data_var_specified;
        string data_var;  // name of variable containing expected data
        bool pf_info_incomplete;
            /* In parsing the template, the expect information comes later than the
               rest of the call info.  This flag tells us to fill in the expect
               info when it comes available. */
        // Expected-result info:
        string pf_result_string;
            // if !pf_nothing && !pf_pass then this is expected result
    // Methods:
        expect_info (void);  // (default constructor)
        ~expect_info (void);  // (destructor)
        void set_pf_pass (void);
        void set_pf_nothing (void);
        void set_pf_error (string error);
        void copy_expect_to_call (psa_call *the_call);

protected:
    // Data members:
        bool data_matches_asset;
            /* true if template specifies expected data, and that expected data
               agrees with that in the asset */
};


/**********************************************************************************
  Class set_data_info addresses PSA-asset data values as affected, directly or
  indirctly/implicitly, by the template-line content.  "Directly," that is, by
  virtue of the template line stating verbatim what to set data to, or indirectly
  by virtue of telling TF-Fuzz to create random data for it.
**********************************************************************************/

class set_data_info
{
public:
    // Data members:
        bool string_specified;
            // true if a string of expected data is specified in template file
        bool random_data;  // true to generate random data for the asset
        bool file_specified;  // true if a file of expected data was specified
        bool literal_data_not_file;
            // true to use data strings rather than files as data source
        string file_path;  // path to file, if specified
    // Methods:
        set_data_info (void);  // (default constructor)
        ~set_data_info (void);  // (destructor)
        void set (string set_val);
        void set_calculated (string set_val);
        string get (void);
        bool set_file (string file_name);

protected:
    // Data members:
        string data;  // String describing asset data.
};


/**********************************************************************************
  Class asset_name_id_info groups together and acts upon all information related to the
  human names (as reflected in the code variable names, etc.) for PSA assets.
**********************************************************************************/

class asset_name_id_info
{
public:
    // Data members (not much value in "hiding" these behind getters)
        bool id_n_not_name;  // true to create a PSA asset by ID
        bool name_specified;  // true iff template supplied human name
        bool id_n_specified;  // true iff template supplied ID #
        vector<string> asset_name_vector;
        vector<int> asset_id_n_vector;
        uint64_t id_n;  // asset ID# (e.g., SST UID).
            /* Note:  This is just a holder to pass ID from template-line to call.
                      The IDs for a given template line are in asset_id.asset_id_n_vector. */
    // Methods:
        asset_name_id_info (void);  // (default constructor)
        ~asset_name_id_info (void);  // (destructor)
        void set_name (string set_val);
        void set_calc_name (string set_val);
        void set_just_name (string set_val);
        string get_name (void);
        void set_id_n (string set_val);
        void set_id_n (uint64_t set_val);
        string make_id_n_based_name (uint64_t id_n, string &name);
            // create UID-based asset name

protected:
    // Data members:
        string asset_name;  // parsed from template, assigned to psa_asset object
};



#endif // DATA_BLOCKS_HPP

