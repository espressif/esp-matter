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
#include "sst_asset.hpp"



/**********************************************************************************
   Methods of class sst_asset follow:
**********************************************************************************/

bool sst_asset::set_uid (uint64_t uid)
{
    /* TODO:  What are the limits upon UIDs?  I don't necessarily not want to be
              able to set an illegal value, but if it is illegal, I might want to
              set some flag appropriately to generate expected results. */
    asset_id.set_id_n (uid);
    return true;
}

sst_asset::sst_asset (void)  // (default constructor)
{
    return;  // just to have something to pin a breakpoint onto
}


sst_asset::~sst_asset (void)  // (destructor)
{
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class sst_asset.
**********************************************************************************/
