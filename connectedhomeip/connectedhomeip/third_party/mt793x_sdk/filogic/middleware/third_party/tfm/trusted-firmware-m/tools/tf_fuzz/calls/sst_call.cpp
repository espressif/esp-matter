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
#include "sst_call.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"



/**********************************************************************************
   Methods of class sst_set_call follow:
**********************************************************************************/

sst_set_call::sst_set_call (tf_fuzz_info *test_state,    // (constructor)
                            long &call_ser_no,
                            asset_search how_asset_found)
                                : sst_call(test_state, call_ser_no, how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");  // will fill in, depending upon template line content
    call_code.assign (test_state->bplate->bplate_string[set_sst_call]);
    check_code.assign (test_state->bplate->bplate_string[set_sst_check]);
}
sst_set_call::~sst_set_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

void sst_set_call::fill_in_prep_code (void)
{
    // Single string of two lines declaring string data and its length:
    string var_name = asset_id.get_name() + "_data";  // the data holder var
    prep_code = test_state->bplate->bplate_string[declare_string];
    find_replace_1st ("$var", var_name, prep_code);
    find_replace_1st ("$init", set_data.get(), prep_code);
    var_name = asset_id.get_name() + "_data_size";  // the data length value
    prep_code.append (test_state->bplate->bplate_string[declare_int]);
    find_replace_1st ("$var", var_name, prep_code);
    find_replace_1st ("$init", to_string(set_data.get().length()), prep_code);
}

void sst_set_call::fill_in_command (void)
{
    // Fill in preceding comment:
    if (how_asset_found == asset_search::created_new) {
        find_replace_1st ("$op", "Creating", call_code);
    } else {
        find_replace_1st ("$op", "Resetting", call_code);
    }
    if (asset_id.name_specified) {
        find_replace_1st ("$description", "\"" + asset_id.get_name() + ",\"",
                          call_code);
    } else {
        find_replace_1st ("$description",
                          "UID = " + to_string((long) asset_id.id_n), call_code);
    }
    if (set_data.string_specified) {
        find_replace_1st ("$data_source",
                          "\"" + set_data.get().substr (0, 10) + "...\"",
                          call_code);
    } else if (set_data.file_specified) {
        find_replace_1st ("$data_source", "from file " + set_data.file_path,
                          call_code);
    } else {
        find_replace_1st (" $data_source", "", call_code);
    }
    // Fill in the PSA command itself:
    string var_name = asset_id.get_name() + "_data_size";
    string id_string = to_string((long) asset_id.id_n);
    find_replace_1st ("$uid", id_string, call_code);
    find_replace_1st ("$length", var_name, call_code);
    var_name = asset_id.get_name() + "_data";
    find_replace_1st ("$data", var_name, call_code);
    find_replace_1st ("$flags", flags_string, call_code);
    // Figure out what expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class sst_set_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_get_call follow:
**********************************************************************************/

sst_get_call::sst_get_call (tf_fuzz_info *test_state,    // (constructor)
                            long &call_ser_no,
                            asset_search how_asset_found)
                                 : sst_call(test_state, call_ser_no, how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_sst_call]);
    check_code.assign ("");
        // depends upon the particular usage;  will get it in fill_in_command()
}
sst_get_call::~sst_get_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

void sst_get_call::fill_in_prep_code (void)
{
    string var_name, temp_string;

    /* Create and use gibberish object to create for-sure-bad data for the
       psa_ps_get() call to overwrite. */
    gibberish *gib = new gibberish;
    char gib_buff[1024];  // TODO:  Sizing of buffers needs to be managed better
    string wrong_data;
        /* bogus data to prefill actual data with in order to distinguish
           whether actual data was provided. */

    gib->sentence (gib_buff, gib_buff + 100 + (rand() % 800));
        // TODO:  Sizes of random data needs to be strategized better
    wrong_data = gib_buff;
    // Expected data:
    if (expect.data_specified) {
        /* Template specified expected verbatim, literal data.  Put that into a
           variable: */
        var_name.assign (asset_id.get_name() + "_exp_data");
    } else if (expect.data_var_specified) {
        // Template specified a variable name for expected data;  use that:
        var_name.assign (expect.data_var);
    }
    // Expected data:
    if (!(print_data || hash_data)) {
        prep_code.assign(test_state->bplate->bplate_string[declare_string]);
        find_replace_1st("$var", var_name, prep_code);
        find_replace_1st("$init", expect.data, prep_code);
    }
    // Actual data:
    if (assign_data_var_specified) {
        var_name.assign (assign_data_var);
    } else {
        var_name.assign (asset_id.get_name() + "_act_data");
    }
    prep_code.append (test_state->bplate->bplate_string[declare_big_string]);
    find_replace_1st ("$var", var_name, prep_code);
    find_replace_1st ("$init", wrong_data, prep_code);  // fill with gibberish
    // (Don't need to specify data length for expected data.)
    // Actual-data length:
    var_name = asset_id.get_name() + "_act_length";
    temp_string.assign (test_state->bplate->bplate_string[declare_int]);
    find_replace_1st ("static int", "static size_t", temp_string);
    prep_code.append (temp_string);
    find_replace_1st ("$var", var_name, prep_code);
    find_replace_1st ("$init", "0", prep_code);
    // Offset (always 0 for now):
    find_replace_1st ("$offset", "0", prep_code);
    // If hashing the (actual) data, then create a variable for that:
    if (hash_data) {
        prep_code.append (test_state->bplate->bplate_string[declare_generic]);
        var_name.assign (asset_id.get_name() + "_hash");  // where to put the hash of the data
        find_replace_1st ("$type", "uint32_t", prep_code);
        find_replace_1st ("$var", var_name, prep_code);
        find_replace_1st ("$init", "0", prep_code);  // for now...
    }
    // Clean-up:
    delete gib;
}

void sst_get_call::fill_in_command (void)
{
    string exp_var_name, act_var_name, act_data_length, hash_var_name, id_string;

/* TODO:  Flesh-out/fix this (a good try, but not quite right):
    // Fill in preceding comment:
    if (how_asset_found == asset_search::created_new) {
        find_replace_1st ("$op", "Creating", call_code);
    } else {
        find_replace_1st ("$op", "Resetting", call_code);
    }
    if (asset_id.name_specified) {
        find_replace_1st ("$description", "\"" + asset_id.get_name() + ",\"",
                          call_code);
    } else {
        find_replace_1st ("$description",
                          "UID = " + to_string((long) asset_id.id_n), call_code);
    }
    if (set_data.string_specified) {
        find_replace_1st ("$data_source",
                          "\"" + data.substr (0, 10) + "...\"",
                          call_code);
    } else if (set_data.file_specified) {
        find_replace_1st ("$data_source", "from file " + set_data.file_path,
                          call_code);
    } else {
        find_replace_1st (" $data_source", "", call_code);
    }
*/    // Fill in the call itself:
    if (assign_data_var_specified || print_data || hash_data) {
        // Dump to variable;  no data-check code needed:
        check_code.assign (test_state->bplate->bplate_string[get_sst_check]);
    } else {
        // Check either against literal or variable, so need data-check code:
        check_code.assign (test_state->bplate->bplate_string[get_sst_check_all]);
    }
    /* Note:  Can fill in the check code identically between the dump-to-variable
              and check-data cases, because the boilerplate for the former is just an
              abbreviated version of the latter.  The find_replace_1st() calls for
              the check-data stuff will just simply not have any effect. */
    if (expect.data_specified) {
        exp_var_name.assign (asset_id.get_name() + "_exp_data");
    } else {
        // whether expect.data_var_specified is true or not:
        exp_var_name.assign (expect.data_var);
    }
    if (assign_data_var_specified) {
        act_var_name.assign (assign_data_var);
    } else {
        act_var_name.assign (asset_id.get_name() + "_act_data");
    }
    /* TODO:  In the case of reading into a variable, does it make more sense to
       assign into two variables:  Data and length? */
    act_data_length = asset_id.get_name() + "_act_length";

    id_string = to_string((long) asset_id.id_n);
    // Fill in the PSA command itself:
    find_replace_1st ("$uid", id_string, call_code);
    find_replace_all ("$length", to_string(set_data.get().length()), call_code);
    find_replace_1st ("$offset", "0", call_code);
    find_replace_1st ("$exp_data", exp_var_name, call_code);
    find_replace_all ("$act_data", act_var_name, call_code);
    find_replace_all ("$act_length", act_data_length, call_code);
    // Perform most of the same substitutions in the check_code:
// TODO:  Make data checks contingent upon the PSA call itself passing!
    find_replace_1st ("$offset", "0", check_code);
    find_replace_1st ("$exp_data", exp_var_name, check_code);
    find_replace_all ("$act_data", act_var_name, check_code);
    find_replace_all ("$length", to_string(expect.data.length()), check_code);
    if (print_data) {
        check_code.append (test_state->bplate->bplate_string[test_log]);
        find_replace_1st ("$message", act_var_name, check_code);
    }
    if (hash_data) {
        hash_var_name.assign (asset_id.get_name() + "_hash");  // where to put the hash of the data
        check_code.append (test_state->bplate->bplate_string[get_sst_hash]);
        find_replace_all ("$act_data_var", act_var_name, check_code);
        find_replace_all ("$hash_var", hash_var_name, check_code);
    }
    // Figure out what expected results:
    calc_result_code();  // this only fills $expect check_code
    // Fill in expected data, actual data, and length:
}

/**********************************************************************************
   End of methods of class sst_get_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_remove_call follow:
**********************************************************************************/

sst_remove_call::sst_remove_call (tf_fuzz_info *test_state,    // (constructor)
                                  long &call_ser_no,
                                  asset_search how_asset_found)
                                 : sst_call(test_state, call_ser_no, how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[remove_sst]);
    check_code.assign (test_state->bplate->bplate_string[remove_sst_check]);
}
sst_remove_call::~sst_remove_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

void sst_remove_call::fill_in_prep_code (void)
{
    // No prep-code.
    return;  // just to have something to pin a breakpoint onto
}

void sst_remove_call::fill_in_command (void)
{
    // Fill in the call:
    string id_string = to_string((long) asset_id.id_n);
    find_replace_1st ("$uid", id_string, call_code);
    // Fill in expected results:
    calc_result_code();  // this only fills $expect check_code
}

/**********************************************************************************
   End of methods of class sst_remove_call.
**********************************************************************************/

