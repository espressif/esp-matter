/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "class_forwards.hpp"

#include "boilerplate.hpp"
#include "randomization.hpp"
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
   Methods of class crypto_asset follow:
**********************************************************************************/

crypto_asset::crypto_asset (void)  // (default constructor)
{
    return;  // just to have something to pin a breakpoint onto
}


crypto_asset::~crypto_asset (void)  // (destructor)
{
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class crypto_asset.
**********************************************************************************/


/**********************************************************************************
   Methods of class policy_asset follow:
**********************************************************************************/

policy_asset::policy_asset (void)  // (default constructor)
{
    // Randomize key-policy usage and algorithm:
    policy_usage = rand_key_usage();
    policy_algorithm = rand_key_algorithm();
    // keys:  Should automatically come up as empby.
}


policy_asset::~policy_asset (void)  // (destructor)
{
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class policy_asset.
**********************************************************************************/


/**********************************************************************************
   Methods of class key_asset follow:
**********************************************************************************/

bool key_asset::set_key_id (int id_n)
{
    key_id = id_n;
    return true;
}


key_asset::key_asset (void)
{
    // Note:  Similar random initialization for asset and template
    // Randomize handle:
    // TODO:  Key handles appear to be a lot more complex a question than the below
    string wrong_data;  // temporary for initial random to be overwritten
    gibberish *gib = new gibberish;
    handle_str = gib->word (false, const_cast<char*>(wrong_data.c_str()),
                            const_cast<char*>(wrong_data.c_str()) + data.length()-1);
    // Randomize key type:
    key_type = rand_key_type();
    // Randomize lifetime:
    lifetime_str = ((rand() % 2) == 1)?
                       "PSA_KEY_LIFETIME_VOLATILE" : "PSA_KEY_LIFETIME_PERSISTENT";
}


key_asset::~key_asset (void)
{
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class key_asset.
**********************************************************************************/
