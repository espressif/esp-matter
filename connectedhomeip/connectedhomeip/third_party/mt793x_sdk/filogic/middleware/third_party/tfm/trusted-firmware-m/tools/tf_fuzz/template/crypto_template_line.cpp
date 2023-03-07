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
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "crypto_call.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "crypto_template_line.hpp"



/**********************************************************************************
   Methods of class set_policy_template_line follow:
**********************************************************************************/

set_policy_template_line::set_policy_template_line (tf_fuzz_info *resources)
    : policy_template_line (resources)
{
    // No further setup to be performed.
    return;  // just to have something to pin a breakpoint onto
}

bool set_policy_template_line::copy_template_to_asset (void)
{
    // TODO:  Should this ever be invoked?
    return true;
}

bool set_policy_template_line::copy_template_to_call (void)
{
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no && call->expect.pf_info_incomplete) {
            call->asset_id.set_just_name (asset_id.get_name());
            call->asset_id.id_n = asset_id.id_n;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // failed to find the call
}

// Default destructor:
set_policy_template_line::~set_policy_template_line (void)
{
    // TODO:  Add an error for this being invoked.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class set_policy_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class read_policy_template_line follow:
**********************************************************************************/

read_policy_template_line::read_policy_template_line (tf_fuzz_info *resources)
    : policy_template_line (resources)
{
}

bool read_policy_template_line::copy_template_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;
    sst_asset* this_asset;
    // Find the call by serial number (must search;  may have moved).
    asset_search how_found = test_state->find_or_create_sst_asset (
                        psa_asset_search::serial, psa_asset_usage::all, "", 0,
                        asset_ser_no, dont_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* an sst_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(108);
    }
    // Copy *the other way* on asset reads!
    if (how_found != asset_search::not_found) {
        this_asset = reinterpret_cast<sst_asset*>(*found_asset);
        set_data.string_specified = this_asset->set_data.string_specified;
        set_data.file_specified = this_asset->set_data.file_specified;
        flags_string = this_asset->flags_string;
        asset_id.id_n = this_asset->asset_id.id_n;
        asset_id.name_specified = this_asset->asset_id.name_specified;
    }
    return true;
}

bool read_policy_template_line::copy_template_to_call (void)
{
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no && call->expect.pf_info_incomplete) {
            call->asset_id.set_just_name (asset_id.get_name());
            call->asset_id.id_n = asset_id.id_n;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // failed to find the call
}

// Default destructor:
read_policy_template_line::~read_policy_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class read_policy_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_key_template_line follow:
**********************************************************************************/

set_key_template_line::set_key_template_line (tf_fuzz_info *resources)
    : key_template_line (resources)
{
    // Nothing further to initialize.
}

bool set_key_template_line::copy_template_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;
    key_asset* this_asset;

    // Find the asset by serial number (must search;  may have moved).
    asset_search how_found = test_state->find_or_create_key_asset (
                        psa_asset_search::serial, psa_asset_usage::all, "", 0,
                        asset_ser_no, yes_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* a key_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(109);
    }
    // Copy over everything relevant:
    if (how_found != asset_search::not_found) {  // will be found for make calls, but not necessarily others
        this_asset = reinterpret_cast<key_asset*>(*found_asset);
        this_asset->set_data.string_specified = set_data.string_specified;
        this_asset->set_data.set (set_data.get());
        this_asset->set_data.file_specified = set_data.file_specified;
        this_asset->set_data.file_path = set_data.file_path;
        this_asset->flags_string = flags_string;
    }
    return true;
}

bool set_key_template_line::copy_template_to_call (void)
{
    // Find the call by serial number (must search;  may have moved).
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            // Copy asset info to call object for creation code:
            call->asset_id.set_just_name (asset_id.get_name());
            call->asset_id.id_n = asset_id.id_n;
            call->set_data.string_specified = set_data.string_specified;
            call->set_data.set (set_data.get());  call->asset_ser_no = asset_ser_no;
            call->set_data.file_specified = set_data.file_specified;
            call->set_data.file_path = set_data.file_path;
            call->flags_string = flags_string;
            call->how_asset_found = how_asset_found;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
set_key_template_line::~set_key_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class set_key_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class remove_key_template_line follow:
**********************************************************************************/

remove_key_template_line::remove_key_template_line (tf_fuzz_info *resources)
    : key_template_line (resources)
{
    is_remove = true;  // template_line's constructor defaults this to false
}

bool remove_key_template_line::copy_template_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;
    sst_asset* this_asset;
    // Find the call by serial number (must search;  may have moved).
    asset_search how_found = test_state->find_or_create_sst_asset (
                        psa_asset_search::serial, psa_asset_usage::all, "", 0,
                        asset_ser_no, dont_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* an sst_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(108);
    }
    // Copy *the other way* on asset reads!
    if (how_found != asset_search::not_found) {  // will be found for make calls, but not necessarily others
        this_asset = reinterpret_cast<sst_asset*>(*found_asset);
        set_data.string_specified = this_asset->set_data.string_specified;
        set_data.file_specified = this_asset->set_data.file_specified;
        flags_string = this_asset->flags_string;
        asset_id.id_n = this_asset->asset_id.id_n;
        asset_id.name_specified = this_asset->asset_id.name_specified;
    }
    return true;
}

bool remove_key_template_line::copy_template_to_call (void)
{
    // Find the call by serial number (must search;  may have moved).
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            call->asset_id.set_just_name (asset_id.get_name());
            call->asset_id.id_n = asset_id.id_n;
            call->expect.pf_nothing = expect.pf_nothing;
            call->expect.pf_pass = expect.pf_pass;  call->asset_ser_no = asset_ser_no;
            call->expect.pf_specified = expect.pf_specified;
            call->expect.pf_result_string = expect.pf_result_string;
            call->how_asset_found = how_asset_found;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
remove_key_template_line::~remove_key_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class remove_key_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class read_key_template_line follow:
**********************************************************************************/

read_key_template_line::read_key_template_line (tf_fuzz_info *resources)
    : key_template_line (resources)
{
    char gibberish_buffer[4096];  string databuff;
    int data_length;
    set_data.string_specified = (rand()%2) == 1?  true : false;

    // Go ahead and create a literal-data string even if not ultimately used:
    data_length = test_state->gibbergen.pick_sentence_len();
    test_state->gibbergen.sentence (gibberish_buffer, gibberish_buffer + data_length);
    databuff = gibberish_buffer;  set_data.set (databuff);

    set_data.file_specified = (!set_data.string_specified && (rand()%2) == 1)?  true : false;
    set_data.file_path = "";  // can't really devise a random path
}

bool read_key_template_line::copy_template_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;
    sst_asset* this_asset;
    // Find the call by serial number (must search;  may have moved).
    asset_search how_found = test_state->find_or_create_sst_asset (
                        psa_asset_search::serial, psa_asset_usage::all, "", 0,
                        asset_ser_no, dont_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* an sst_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(108);
    }
    // Copy *the other way* on asset reads!
    if (how_found != asset_search::not_found) {
        this_asset = reinterpret_cast<sst_asset*>(*found_asset);
        set_data.string_specified = this_asset->set_data.string_specified;
        set_data.file_specified = this_asset->set_data.file_specified;
        flags_string = this_asset->flags_string;
        asset_id.id_n = this_asset->asset_id.id_n;
        asset_id.name_specified = this_asset->asset_id.name_specified;
    }
    return true;
}

bool read_key_template_line::copy_template_to_call (void)
{
    // Find the call by serial number (must search;  may have moved).
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            // Copy expected results to the call object, to check:
            call->asset_id.set_just_name (asset_id.get_name());
            call->asset_id.id_n = asset_id.id_n;
            call->set_data.string_specified = set_data.string_specified;
            call->set_data.set (set_data.get());  call->asset_ser_no = asset_ser_no;
            call->set_data.file_specified = set_data.file_specified;
            call->set_data.file_path = set_data.file_path;
            call->flags_string = flags_string;
            call->set_data.string_specified = set_data.string_specified;
            call->how_asset_found = how_asset_found;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
read_key_template_line::~read_key_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)

/**********************************************************************************
   End of methods of class read_key_template_line.
**********************************************************************************/
