/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

 /* Objects of these types are new()ed as a test-template line is parsed, and are
    resolved to be the type of template command stated -- "create a key," "remove
    an SST asset," or similar.  Parameters of the template line are parsed into
    the object's members, where specified, but those members are randomized upon
    construction.  Thus, any parameters not "nailed down," get randomized.
 */

#include <vector>
#include <algorithm>  // for STL find()

#include "class_forwards.hpp"

#include "data_blocks.hpp"
#include "boilerplate.hpp"
#include "randomization.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "crypto_call.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"



/**********************************************************************************
   Methods of class template_line follow:
**********************************************************************************/

// Constructor, passing in the tf_fuzz object for reference to all objects:
template_line::template_line (tf_fuzz_info *resources) : test_state(resources)
{
    set_data.file_path.assign ("");
    assign_data_var_specified = false;
    flags_string.assign ("");
    random_asset = psa_asset_usage::all;
        // if not deleting a random asset of a certain type, then search all as usual
    set_data.file_specified = false;
    print_data = hash_data = false;
    is_remove = false;  // will correct this in the remove case
}

/**********************************************************************************
   Class template_line methods regarding setting and getting asset-names follow:
**********************************************************************************/

// Default destructor:
template_line::~template_line (void)
{
    // Destruct the vectors of asset names/IDs:
    asset_id.asset_name_vector.erase (asset_id.asset_name_vector.begin(), asset_id.asset_name_vector.end());
    asset_id.asset_id_n_vector.erase (asset_id.asset_id_n_vector.begin(), asset_id.asset_id_n_vector.end());
}

// (Default constructor not used)

/**********************************************************************************
   End of methods of class template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_template_line follow:
**********************************************************************************/

sst_template_line::sst_template_line (tf_fuzz_info *resources) : template_line (resources)
{
    asset_type = psa_asset_type::sst;
    return;  // just to have something to pin a breakpoint onto
}

// Default destructor:
sst_template_line::~sst_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class sst_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class key_template_line follow:
**********************************************************************************/

key_template_line::key_template_line (tf_fuzz_info *resources) : template_line (resources)
{
    // Note:  Similar random initialization for asset and template
    // Randomize handle:
    // TODO:  Key handles appear to be a lot more complex a question than the below
    asset_type = psa_asset_type::key;
    string wrong_data;  // holder for random data to be overwritten
    gibberish *gib = new gibberish;
    handle_str = gib->word (false, const_cast<char*>(wrong_data.c_str()),
                            const_cast<char*>(  wrong_data.c_str())
                                              + set_data.get().length()-1);
    // Randomize key type:
    key_type = rand_key_type();
    // Randomize lifetime:
    lifetime_str = ((rand() % 2) == 1)?
                       "PSA_KEY_LIFETIME_VOLATILE" : "PSA_KEY_LIFETIME_PERSISTENT";
    // Choose a random expected key size in bits:
    expected_n_bits = to_string(rand()%10000);
}

// Create ID-based name:
string key_template_line::make_id_based_name (uint64_t id_n, string &name)
{
    string result = "Key_ID_";
    result.append(to_string(id_n));
    return result;
}

// Default destructor:
key_template_line::~key_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class key_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class policy_template_line follow:
**********************************************************************************/

policy_template_line::policy_template_line (tf_fuzz_info *resources)
            : template_line (resources)
{
    asset_type = psa_asset_type::policy;
    // Randomize key-policy usage and algorithm:
    policy_usage = rand_key_usage();
    policy_algorithm = rand_key_algorithm();
}

// Create ID-based name:
string policy_template_line::make_id_based_name (uint64_t id_n, string &name)
{
    string result = "Policy_ID_";
    result.append(to_string(id_n));
    //
    return result;
}

// Default destructor:
policy_template_line::~policy_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class policy_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class security_template_line follow:
**********************************************************************************/

security_template_line::security_template_line (tf_fuzz_info *resources)
            : template_line (resources)
{
}

// Default destructor:
security_template_line::~security_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class security_template_line.
**********************************************************************************/

