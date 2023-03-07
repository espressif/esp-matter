/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CRYPTO_TEMPLATE_LINE_HPP
#define CRYPTO_TEMPLATE_LINE_HPP

#include <cstdint>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "psa_asset.hpp"
//class psa_asset;  // just need a forward reference
#include "template_line.hpp"
#include "psa_call.hpp"
*/
using namespace std;


class set_policy_template_line : public policy_template_line
{
public:
    // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                            bool fill_in_template, bool create_call,
                            template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<policy_set_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        set_policy_template_line (tf_fuzz_info *resources);  // (constructor)
        ~set_policy_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class read_policy_template_line : public policy_template_line
{
public:
    // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                            bool fill_in_template, bool create_call,
                            template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<policy_get_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        read_policy_template_line (tf_fuzz_info *resources);  // (constructor)
        ~read_policy_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class set_key_template_line : public key_template_line
{
public:
   // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<set_key_call> (set_info, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        set_key_template_line (tf_fuzz_info *resources);  // (constructor)
        ~set_key_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class remove_key_template_line : public key_template_line
{
public:
    // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<destroy_key_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        remove_key_template_line (tf_fuzz_info *resources);  // (constructor)
        ~remove_key_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class read_key_template_line : public key_template_line
{
public:
    // Data members:
    // Methods:
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<get_key_info_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        read_key_template_line (tf_fuzz_info *resources);  // (constructor)
        ~read_key_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif  // #ifndef CRYPTO_TEMPLATE_LINE_HPP
