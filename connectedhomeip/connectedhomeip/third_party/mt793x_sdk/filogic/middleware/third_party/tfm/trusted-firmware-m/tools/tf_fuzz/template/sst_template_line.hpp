/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SST_TEMPLATE_LINE_HPP
#define SST_TEMPLATE_LINE_HPP

#include <cstdlib>  // for rand()

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "psa_asset.hpp"
#include "template_line.hpp"
#include "psa_call.hpp"
*/


using namespace std;

class set_sst_template_line : public sst_template_line
{
public:
    // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                    bool fill_in_template, bool create_call, template_line *temLin,
                    tf_fuzz_info *rsrc) {
            define_call<sst_set_call> (set_info, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        set_sst_template_line (tf_fuzz_info *resources);  // (constructor)
        ~set_sst_template_line (void);

protected:
    // Data members:
    // Methods:
        string rand_creation_flags (void);  // choose a random set of creation flags

private:
    // Data members:
    // Methods:
};

class remove_sst_template_line : public sst_template_line
{
public:
    // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                            bool fill_in_template, bool create_call,
                            template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<sst_remove_call> (set_info, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        remove_sst_template_line (tf_fuzz_info *resources);  // (constructor)
        ~remove_sst_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class read_sst_template_line : public sst_template_line
{
public:
    // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                            bool fill_in_template, bool create_call,
                            template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<sst_get_call> (set_info, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        read_sst_template_line (tf_fuzz_info *resources);  // (constructor)
        ~read_sst_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif  // #ifndef SST_TEMPLATE_LINE_HPP
