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


/**********************************************************************************
   Methods of class psa_asset follow:
**********************************************************************************/

void psa_asset::set_name (string set_val)
{
    asset_id.name_specified = true;
    asset_name.assign (set_val);
}

string psa_asset::get_name (void)
{
    return asset_name;
}

psa_asset::psa_asset (void)  // (default constructor)
{
    asset_ser_no = unique_id_counter++;  // just something to pin a breakpoint onto
}


psa_asset::~psa_asset (void)
{
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class psa_asset.
**********************************************************************************/
