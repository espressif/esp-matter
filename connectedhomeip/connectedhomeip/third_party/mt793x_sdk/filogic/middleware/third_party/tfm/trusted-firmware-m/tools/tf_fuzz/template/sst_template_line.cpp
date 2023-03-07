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
#include "sst_call.hpp"
#include "sst_template_line.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"



/**********************************************************************************
   Methods of class set_sst_template_line follow:
**********************************************************************************/

//**************** set_sst_template_line methods ****************

string set_sst_template_line::rand_creation_flags (void)
{
    return ((rand() % 2) == 1)?
        "PSA_STORAGE_FLAG_WRITE_ONCE" : "PSA_STORAGE_FLAG_NONE";
    /* TODO:  There's seems to be some uncertainty as to how many creation-flag
       values are actually used, so for now only using PSA_STORAGE_FLAG_WRITE_ONCE
       and PSA_STORAGE_FLAG_NONE.  If there are more in real-world use, then
       resurrect the commented-out code below to assign them:
    string result = "";
    const int most_flags = 3,
    int n_flags = (rand() % most_flags);

    for (int i = 0;  i < ;  i < n_flags;  ++i) {
        switch (rand() % 5) {
            case 0:
                result += "PSA_STORAGE_FLAG_WRITE_ONCE";
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
        }
        if (i < n_flags-1)
            result += " | ";
    }
    if (result == "") result = "PSA_STORAGE_FLAG_NONE";
*/
}

set_sst_template_line::set_sst_template_line (tf_fuzz_info *resources)
    : sst_template_line (resources)  // constructor)
{
    // Creation flags:
    flags_string = rand_creation_flags();
}



bool set_sst_template_line::copy_template_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;
    sst_asset* this_asset;
    // Find the call by serial number (must search;  may have moved).
    asset_search how_found = test_state->find_or_create_sst_asset (
                        psa_asset_search::serial, psa_asset_usage::all, "", 0,
                        asset_ser_no, yes_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* an sst_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(108);
    }
    // Copy over everything relevant:
    if (how_found != asset_search::not_found) {  // will be found for make calls, but not necessarily others
        this_asset = reinterpret_cast<sst_asset*>(*found_asset);
        this_asset->set_data.string_specified = set_data.string_specified;
        this_asset->set_data.set (set_data.get());
        this_asset->set_data.file_specified = set_data.file_specified;
        this_asset->set_data.file_path.assign (set_data.file_path);
        this_asset->flags_string = flags_string;
        this_asset->set_uid (asset_id.id_n);
        this_asset->asset_id.name_specified = asset_id.name_specified;
        this_asset->asset_id.set_name (asset_id.get_name());
    }
    return true;
}

bool set_sst_template_line::copy_template_to_call (void)
{
    // Find the call by serial number (must search;  may have moved).
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            // Copy asset info to call object for creation code:
            call->asset_id.set_just_name (asset_id.get_name());
                // TODO:  Question:  Just call->asset_id = asset_id?
            call->asset_id.id_n = asset_id.id_n;
            call->asset_id.name_specified = asset_id.name_specified;
            call->set_data.string_specified =   set_data.string_specified
                                             || set_data.random_data;
            call->set_data.file_specified = set_data.file_specified;
            call->set_data.file_path = set_data.file_path;
            call->asset_ser_no = asset_ser_no;
            call->flags_string = flags_string;  call->set_data.set (set_data.get());
            call->how_asset_found = how_asset_found;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
set_sst_template_line::~set_sst_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class set_sst_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class remove_sst_template_line follow:
**********************************************************************************/

remove_sst_template_line::remove_sst_template_line (tf_fuzz_info *resources)
    : sst_template_line (resources)  // (constructor)
{
    is_remove = true;  // template_line's constructor defaults this to false
}

bool remove_sst_template_line::copy_template_to_asset (void)
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
           // Note: The vector is of base-class type, but the assets *are* sst_asset.
        set_data.string_specified = this_asset->set_data.string_specified;
        set_data.file_specified = this_asset->set_data.file_specified;
        flags_string = this_asset->flags_string;
        asset_id.id_n = this_asset->asset_id.id_n;
        asset_id.name_specified = this_asset->asset_id.name_specified;
    }
    return true;
}

bool remove_sst_template_line::copy_template_to_call (void)
{
    // Find the call by serial number (must search;  may have moved).
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            call->asset_id.set_just_name (asset_id.get_name());
            call->asset_id.id_n = asset_id.id_n;
            call->set_data.string_specified = false;
            call->set_data.set ("");  call->id_string = asset_name;
            call->set_data.file_specified = false;
            call->set_data.file_path = "";  call->asset_ser_no = asset_ser_no;
            call->how_asset_found = how_asset_found;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
remove_sst_template_line::~remove_sst_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class remove_sst_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class read_sst_template_line follow:
**********************************************************************************/

read_sst_template_line::read_sst_template_line (tf_fuzz_info *resources)
    : sst_template_line (resources)  // (constructor)
{
    char gibberish_buffer[4096];  string databuff;
    int data_length;
    set_data.string_specified = (rand()%2) == 1?  true : false;

    // Go ahead and create a literal-data string even if not needed:
    data_length = test_state->gibbergen.pick_sentence_len();
    test_state->gibbergen.sentence (gibberish_buffer, gibberish_buffer + data_length);
    databuff = gibberish_buffer;  set_data.set (databuff);

    set_data.file_specified = (!set_data.string_specified && (rand()%2) == 1)?  true : false;
    set_data.file_path = "";  // can't really devise a random path
}

bool read_sst_template_line::copy_template_to_asset (void)
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
    // TODO:  Shouldn't data be copied over?
}

bool read_sst_template_line::copy_template_to_call (void)
{
    // Find the call by serial number (must search;  may have moved).
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            call->asset_id.set_just_name (asset_id.get_name());
            call->asset_id.id_n = asset_id.id_n;
            call->set_data.string_specified
                    = set_data.string_specified || set_data.random_data;
            call->set_data.set(set_data.get());
            call->assign_data_var = assign_data_var;
            call->assign_data_var_specified = assign_data_var_specified;
            // TODO:  Just copy entire expect object?  call->expect = expect;
            call->expect.data_var = expect.data_var;
            call->expect.data_var_specified = expect.data_var_specified;
            call->expect.data_specified = expect.data_specified;
            call->expect.data.assign(expect.data);
            call->expect.pf_info_incomplete = true;
            call->id_string = asset_name;  // data = expected
            call->set_data.file_specified = set_data.file_specified;
            call->set_data.file_path = set_data.file_path;  call->asset_ser_no = asset_ser_no;
            call->flags_string = flags_string;
            call->how_asset_found = how_asset_found;
            call->print_data = print_data;
            call->hash_data = hash_data;
            return true;
        }
    }
    return false;  // somehow didn't find it the call
}

// Default destructor:
read_sst_template_line::~read_sst_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class read_sst_template_line.
**********************************************************************************/

