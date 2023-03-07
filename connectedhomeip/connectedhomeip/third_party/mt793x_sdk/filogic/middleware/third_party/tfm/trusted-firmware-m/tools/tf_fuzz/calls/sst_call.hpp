/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SST_CALL_HPP
#define SST_CALL_HPP

#include <string>
#include <vector>
#include <cstdint>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "sst_asset.hpp"
#include "psa_call.hpp"
*/


using namespace std;

class sst_set_call : public sst_call
{
public:
    // Data members:
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        sst_set_call (tf_fuzz_info *test_state, long &asset_ser_no,
                      asset_search how_asset_found);  // (constructor)
        ~sst_set_call (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class sst_get_call : public sst_call
{
public:
    // Data members:
        uint32_t offset;
        uint32_t data_length;
        string data_var_name;
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        sst_get_call (tf_fuzz_info *test_state, long &asset_ser_no,
                      asset_search how_asset_found);  // (constructor)
        ~sst_get_call (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class sst_remove_call : public sst_call
{
public:
    // Data members:
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        sst_remove_call (tf_fuzz_info *test_state, long &asset_ser_no,
                      asset_search how_asset_found);  // (constructor)
        ~sst_remove_call (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif  // SST_CALL_HPP

