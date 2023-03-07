/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef FIND_OR_CREATE_ASSET_HPP
#define FIND_OR_CREATE_ASSET_HPP

/* This enum defines possible results when asked to find an existing, or create a
   new PSA asset. */
enum class asset_search
{   found_active,  // found as an actively-used asset
    found_deleted,  // found as a previously-used, but now-unusable asset
    found_invalid,  // found as a previously-used, but now-unusable asset
    not_found,  // if not found and if not allowed to create it
    created_new,  // no such existing asset was found so created new one
    unsuccessful,  // ran out of memory or whatever
    something_wrong  // something wrong with the code;  shouldn't happen
};
// Search areas:
enum class psa_asset_usage  // one particular area or all
{   active,  // found as an actively-used asset
    deleted,  // found as a previously-used, but now-unusable asset
    invalid,  // found as a previously-used, but now-unusable asset
    all,  // if not found and if not allowed to create it
    none  // just create it
};
// In searching for an asset, this enum specifies what to search for:
enum class psa_asset_search
{   name,  // its human name
    id,  // its id, such as SST UID
    serial  // its serial number assigned upon creation
};
// Types of assets:
enum class psa_asset_type
{   sst,  // (pretty obvious what each of these mean)
    key,
    policy
};


// A few consts just to make code more readable:
const bool yes_create_asset = true;
const bool dont_create_asset = false;


/* There are several variants, by asset type, of this method.  So, C++ templating
   is best.  Note that, while the vectors are pointers to the base, psa_asset type,
   the individual entries are all of the same ASSET_TYPE type. */
template <typename ASSET_TYPE>
asset_search generic_find_or_create_asset (
    vector<psa_asset*> &active_asset_vector,  // the three vectors of known assets
    vector<psa_asset*> &deleted_asset_vector,
    vector<psa_asset*> &invalid_asset_vector,
    psa_asset_search criterion,  // what to search on
    psa_asset_usage where,  // where to search
    string target_name,  // ignored if not searching on name
    uint64_t target_id,  // also ignored if not searching on ID (e.g., SST UID)
    long &serial_no,  // search on this if requested, but return serial regardless
    bool create_asset,  // true to create the asset if it doesn't exist
    typename vector<psa_asset*>::iterator &asset
        // returns iterator to the requested asset
) {
    ASSET_TYPE *new_asset;
    bool match = false;  // match found
    // Look first in active assets:
    if (where == psa_asset_usage::active || where == psa_asset_usage::all) {
        for (auto as = active_asset_vector.begin();  as < active_asset_vector.end();
             ++as) {
            psa_asset *pass = *as;
            switch (criterion) {
                case psa_asset_search::name:  // human-meaningful name
                    match = (pass->asset_id.get_name() == target_name);
                    break;
                case psa_asset_search::id:  // ID#
                    match = (pass->asset_id.id_n == target_id);
                    break;
                default:  // psa_asset_search::serial
                    match = (pass->asset_ser_no == serial_no);
                    break;
            }
            if (match) {
                asset = as;
                return asset_search::found_active;
            }
        }
    }
    // Look then in deleted assets:
    if (where == psa_asset_usage::deleted || where == psa_asset_usage::all) {
        for (auto as = deleted_asset_vector.begin();  as < deleted_asset_vector.end();
             ++as) {
            psa_asset *pass = *as;
            switch (criterion) {
                case psa_asset_search::name:  // human-meaningful name
                    match = (pass->asset_id.get_name() == target_name);
                    break;
                case psa_asset_search::id:  // ID#
                    match = (pass->asset_id.id_n == target_id);
                    break;
                default:  // psa_asset_search::serial
                    match = (pass->asset_ser_no == serial_no);
                    break;
            }
            if (match) {
                asset = as;
                return asset_search::found_deleted;
            }
        }
    }
    // Look then in invalid assets:
    if (where == psa_asset_usage::invalid || where == psa_asset_usage::all) {
        for (auto as = invalid_asset_vector.begin();  as < invalid_asset_vector.end();
             ++as) {
            psa_asset *pass = *as;
            switch (criterion) {
                case psa_asset_search::name:  // human-meaningful name
                    match = (pass->asset_id.get_name() == target_name);
                    break;
                case psa_asset_search::id:  // ID#
                    match = (pass->asset_id.id_n == target_id);
                    break;
                default:  // psa_asset_search::serial
                    match = (pass->asset_ser_no == serial_no);
                    break;
            }
            if (match) {
                asset = as;
                return asset_search::found_invalid;
            }
        }
    }
    // Couldn't find it in any of the existing lists, so create it in active assets:
    if (create_asset) {
        try {
            new_asset = new ASSET_TYPE;
            if (criterion == psa_asset_search::id) {
                new_asset->asset_id.id_n = target_id;
            }  // TO DO:  probably should do the same for its name in a name search!
            active_asset_vector.push_back(new_asset);
            asset = prev(active_asset_vector.end());
            return asset_search::created_new;
        }
        catch (std::bad_alloc& bad) {
            std::cerr << "Error:  Failed to allocate an ASSET_TYPE:  " << bad.what()
                      << endl;
            return asset_search::unsuccessful;
        }
    } else {
        return asset_search::not_found;
    }
    return asset_search::something_wrong;  // should never get here
}

#endif  // ifndef FIND_OR_CREATE_ASSET_HPP
