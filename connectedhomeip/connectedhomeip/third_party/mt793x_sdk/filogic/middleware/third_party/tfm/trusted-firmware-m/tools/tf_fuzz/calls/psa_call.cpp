/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "class_forwards.hpp"

#include "boilerplate.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "string_ops.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"



/**********************************************************************************
   Methods of class psa_call follow:
**********************************************************************************/

//**************** psa_call methods ****************

psa_call::psa_call (tf_fuzz_info *test_state, long &call_ser_no,   // (constructor)
                    asset_search how_asset_found)
{
    this->test_state = test_state;
    this->how_asset_found = how_asset_found;
    set_data.string_specified = false;
    set_data.set ("");  // actual data
    assign_data_var.assign ("");  // name of variable assigned (dumped) to
    assign_data_var_specified = false;
    set_data.file_specified = false;
    set_data.file_path.assign ("");
    this->call_ser_no = call_ser_no = unique_id_counter++;
    // These will be set in the lower-level constructors, but...
    prep_code = call_code = check_code = "";
    print_data = hash_data = false;
}

psa_call::~psa_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

void psa_call::write_out_prep_code (ofstream &test_file)
{
    test_file << prep_code;
}

void psa_call::write_out_command (ofstream &test_file)
{
    test_file << call_code;
}

void psa_call::write_out_check_code (ofstream &test_file)
{
    if (!expect.pf_nothing) {
        test_file << check_code;
    } else {
        test_file << "    /* (No checks for this PSA call.) */" << endl;
    }
}

/**********************************************************************************
   End of methods of class psa_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_call follow:
**********************************************************************************/

/* calc_result_code() fills in the check_code string member with the correct
   result code (e.g., "PSA_SUCCESS" or whatever).

   This is a big part of where the target modeling -- error modeling -- occurs,
   so lots of room for further refinement here. */
void sst_call::calc_result_code (void)
{
    if (!expect.pf_nothing) {
        if (expect.pf_pass) {
            find_replace_all ("$expect",
                              test_state->bplate->bplate_string[sst_pass_string],
                              check_code);
        } else {
            if (expect.pf_specified) {
                find_replace_all ("$expect", expect.pf_result_string,
                                  check_code);
            } else {
                // Figure out what the message should read:
                switch (how_asset_found) {
                    case asset_search::found_active:
                    case asset_search::created_new:
                        find_replace_all ("$expect",
                                          test_state->bplate->
                                              bplate_string[sst_pass_string],
                                          check_code);
                        break;
                    case asset_search::found_deleted:
                    case asset_search::not_found:
                        find_replace_all ("$expect",
                                          test_state->bplate->
                                              bplate_string[sst_fail_removed],
                                          check_code);
                        break;
                    default:
                        find_replace_1st ("!=", "==",
                                          check_code);  // for now, just make sure...
                        find_replace_all ("$expect",
                                          test_state->bplate->
                                              bplate_string[sst_pass_string],
                                          check_code);  // ... it's *not* PSA_SUCCESS
                        break;
                }
            }
        }
    }
}

sst_call::sst_call (tf_fuzz_info *test_state, long &call_ser_no,   // (constructor)
                    asset_search how_asset_found)
                        : psa_call(test_state, call_ser_no, how_asset_found)
{
    return;  // just to have something to pin a breakpoint onto
}
sst_call::~sst_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class sst_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class crypto_call follow:
**********************************************************************************/

/* calc_result_code() fills in the check_code string member with the correct
   result code (e.g., "PSA_SUCCESS" or whatever).  This "modeling" needs to be
   improved and expanded upon *massively* more or less mirroring what is seen in
   .../test/suites/crypto/crypto_tests_common.c in the psa_key_interface_test()
   method, (starting around line 20ish). */
void crypto_call::calc_result_code (void)
{
    if (!expect.pf_nothing) {
        if (expect.pf_pass) {
            find_replace_1st ("$expect", "PSA_SUCCESS", check_code);
        } else {
            if (expect.pf_specified) {
                find_replace_1st ("$expect", expect.pf_result_string,
                                  check_code);
            } else {
                // Figure out what the message should read:
                switch (how_asset_found) {
                    case asset_search::found_active:
                    case asset_search::created_new:
                        find_replace_all ("$expect", "PSA_SUCCESS",
                                          check_code);
                        break;
                    case asset_search::found_deleted:
                        find_replace_all ("$expect", "PSA_ERROR_INVALID_HANDLE",
                                          check_code);
                        break;
                    default:
                        find_replace_1st ("!=", "==",
                                          check_code);  // for now, just make sure...
                        find_replace_all ("$expect", "PSA_SUCCESS",
                                          check_code);  // ... it's *not* PSA_SUCCESS
                        break;
                }
            }
        }
    }
}


crypto_call::crypto_call (tf_fuzz_info *test_state, long &call_ser_no,  // (constructor)
                          asset_search how_asset_found)
                             : psa_call(test_state, call_ser_no, how_asset_found)
{
    // Nothing further to initialize.
    return;  // just to have something to pin a breakpoint onto
}
crypto_call::~crypto_call (void)
{
    // Nothing further to delete.
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class crypto_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class security_call follow:
**********************************************************************************/

security_call::security_call (tf_fuzz_info *test_state, long &call_ser_no,  // (constructor)
                          asset_search how_asset_found)
                             : psa_call(test_state, call_ser_no, how_asset_found)
{
    // Nothing further to initialize.
    return;  // just to have something to pin a breakpoint onto
}
security_call::~security_call (void)
{
    // Nothing further to delete.
    return;  // just to have something to pin a breakpoint onto
}

/* calc_result_code() fills in the check_code string member with the correct result
   code (e.g., "PSA_SUCCESS" or whatever).

   Since there are no actual PSA calls associated with security calls (so far at least),
   this should never be invoked. */
void security_call::calc_result_code (void)
{
    // Currently should not be invoked.
    cerr << "\nError:  Internal:  Please report error #205 to TF-Fuzz developers." << endl;
    exit (205);
}

/**********************************************************************************
   End of methods of class security_call.
**********************************************************************************/


