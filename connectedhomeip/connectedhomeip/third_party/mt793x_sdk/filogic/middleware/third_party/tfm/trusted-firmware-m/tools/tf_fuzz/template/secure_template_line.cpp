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
#include "security_call.hpp"
#include "secure_template_line.hpp"
#include "sst_call.hpp"
#include "sst_template_line.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"



/**********************************************************************************
   Methods of class security_hash_template_line follow:
**********************************************************************************/

//**************** security_hash_template_line methods ****************

bool security_hash_template_line::copy_template_to_asset (void)
{
    // The assets are not directly involved in this call.
    return true;
}

bool security_hash_template_line::copy_template_to_call (void)
{
    // Find the call by serial number (must search;  may have moved).
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            // Copy asset info to call object for creation code -- the entire vector:
            for (auto as_name : asset_id.asset_name_vector) {
                /* Also copy into template line object's local vector: */
                call->asset_id.asset_name_vector.push_back (as_name);
            }
            call->asset_id.id_n = asset_id.id_n;
                // this call is currently limited to name-based
            call->asset_id.name_specified = true;
            call->set_data.string_specified = false;  // shouldn't matter, but...
            call->set_data.file_specified = false;
            call->set_data.file_path.assign("");
            call->asset_ser_no = asset_ser_no;  // TODO:  Does this make sense?
            call->flags_string.assign ("");  call->set_data.set("");
            call->how_asset_found = asset_search::found_active;
            call->expect.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

security_hash_template_line::security_hash_template_line (tf_fuzz_info *resources)
    : security_template_line (resources)
{
}
// Default destructor:
security_hash_template_line::~security_hash_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class security_hash_template_line.
**********************************************************************************/

