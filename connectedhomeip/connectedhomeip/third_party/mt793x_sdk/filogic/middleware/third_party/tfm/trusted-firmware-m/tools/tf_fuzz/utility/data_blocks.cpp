/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* These classes "cut down the clutter" by grouping together related data and
   associated methods (most importantly their constructors) used in template_
   line, psa_call, psa_asset (etc.). */

#include <string>
#include <vector>
#include <cstdint>

#include "class_forwards.hpp"

#include "boilerplate.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "string_ops.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "crypto_asset.hpp"
#include "find_or_create_asset.hpp"
#include "psa_call.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"



/**********************************************************************************
   Methods of class expect_info follow:
**********************************************************************************/

expect_info::expect_info (void)  // (default constructor)
{
    pf_nothing = false;  // by default, TF-Fuzz provides expected results
    pf_pass = pf_specified = false;
    pf_result_string.assign ("");  data.assign ("");
    data_var_specified = false;
    data_var.assign ("");  // name of expected-data variable
    data_specified = false;
    data.assign ("");
    pf_info_incomplete = true;
}
expect_info::~expect_info (void)  // (destructor)
{}

void expect_info::set_pf_pass (void)
{
    pf_pass = true;
    pf_nothing = pf_specified = false;
    pf_result_string = "";
}

void expect_info::set_pf_nothing (void)
{
    pf_nothing = true;
    pf_pass = pf_specified = false;
    pf_result_string = "";
}

void expect_info::set_pf_error (string error)
{
    pf_specified = true;
    pf_result_string.assign (error);  // just default "guess," to be filled in
    pf_pass = pf_nothing = false;
}

/* What the call expects is not available from the parser until the call has already
   been created.  The flag, pf_info_incomplete, that indicates whether or not the
   "expects" information has been filled in  If not, fill it in from the template,
   once that info has been parsed. */
void expect_info::copy_expect_to_call (psa_call *the_call)
{
    the_call->expect.pf_nothing = pf_nothing;
    the_call->expect.pf_pass = pf_pass;
    the_call->expect.pf_specified = pf_specified;
    the_call->expect.pf_result_string = pf_result_string;
    the_call->expect.pf_info_incomplete = false;
}

/**********************************************************************************
   End of methods of class expect_info.
**********************************************************************************/


/**********************************************************************************
   Class set_data_info methods regarding setting and getting asset-data values:
**********************************************************************************/

set_data_info::set_data_info (void)  // (default constructor)
{
    literal_data_not_file = true;  // currently, not using files as data sources
    string_specified = false;
    data.assign ("");
    random_data = false;
    file_specified = false;
    file_path.assign ("");
}
set_data_info::~set_data_info (void)  // (destructor)
{}

/* set() establishes:
   *  An asset's data value from a template line (e.g., set sst snort data "data
      value"), and
   *  *That* such a value was directly specified, as opposed to no data value having
      been specified, or a random data value being requested.
   Arguably, this method "has side effects," in that it not only sets a value, but
   also "takes notes" about where that value came from.
*/
void set_data_info::set (string set_val)
{
    literal_data_not_file = true;  // currently, not using files as data sources
    string_specified = true;
    data.assign (set_val);
}

/* set_calculated() establishes:
   *  An asset's data value as *not* taken from a template line, and
   *  *That* such a value was not directly specified in any template line, such as
      if a random data value being requested.
   Arguably, this method "has side effects," in that it not only sets a value, but
   also "takes notes" about where that value came from.
*/
void set_data_info::set_calculated (string set_val)
{
    literal_data_not_file = true;  // currently, not using files as data sources
    string_specified = false;
    data.assign (set_val);
}

/* Getter for protected member, data.  Protected so that it can only be set by
   set() or set_calculated(), above, to establish not only its value but
   how it came about. */
string set_data_info::get (void)
{
    return data;
}

/* Currently, files as data sources aren't used, so this whole method is not "of
   use," but that might change at some point. */
bool set_data_info::set_file (string file_name)
{
    literal_data_not_file = true;
    string_specified = false;
    data.assign ("");
    file_specified = true;
    // Remove the ' ' quotes around the file name:
    file_name.erase (0, 1);
    file_name.erase (file_name.length()-1, 1);
    file_path = file_name;
    return true;
}

/**********************************************************************************
   End of methods of class set_data_info.
**********************************************************************************/


/**********************************************************************************
   Class asset_name_id_info methods regarding setting and getting asset-data values:
**********************************************************************************/

asset_name_id_info::asset_name_id_info (void)  // (default constructor)
{
    id_n_not_name = false;  // (arbitrary)
    id_n = 100 + (rand() % 10000);  // default to random ID# (e.g., SST UID)
    asset_name.assign ("");
    id_n_specified = name_specified = false;  // no ID info yet
    asset_name_vector.clear();
    asset_id_n_vector.clear();
}
asset_name_id_info::~asset_name_id_info (void)
{
    asset_name_vector.clear();
    asset_id_n_vector.clear();
}

/* set_name() establishes:
   *  An asset's "human" name from a template line, and
   *  *That* that name was directly specified, as opposed to the asset being defined
      by ID only, or a random name being requested.
   Arguably, this method "has side effects," in that it not only sets a name, but
   also "takes notes" about where that name came from.
*/
void asset_name_id_info::set_name (string set_val)
{
    /* Use this to set the name as specified in the template file.  Call this only
       if the template file does indeed define a name. */
    name_specified = true;
    asset_name = set_val;
}

/* set_calc_name() establishes:
   *  An asset's "human" name *not* from a template line, and
   *  *That* that name was *not* directly specified in any template line.
   Arguably, this method "has side effects," in that it not only sets a name, but
   also "takes notes" about where that name came from.
*/
void asset_name_id_info::set_calc_name (string set_val)
{
    name_specified = false;
    asset_name = set_val;
}

// set_just_name() sets an asset's "human" name, without noting how that name came up.
void asset_name_id_info::set_just_name (string set_val)
{
    asset_name = set_val;
}

/* Getter for protected member, asset_name.  Protected so that it can only be set by
   set_name() or set_calc_name(), above, to establish not only its value but
   how it came about. */
string asset_name_id_info::get_name (void)
{
    return asset_name;
}

// Asset IDs can be set directly from a uint64_t or converted from a string:
void asset_name_id_info::set_id_n (string set_val)
{
    id_n = stol (set_val, 0, 0);
}
void asset_name_id_info::set_id_n (uint64_t set_val)
{
    id_n = set_val;
}

// Create ID-based name:
string asset_name_id_info::make_id_n_based_name (uint64_t id_n, string &name)
{
    string result = "SST_ID_";
    result.append(to_string(id_n));
    return result;
}

/**********************************************************************************
   End of methods of class asset_name_id_info.
**********************************************************************************/
