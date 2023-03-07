/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Objects typed to subclasses of the these classes are constructed and filled in
   by the parser as it parses the template.  Although these objects do fairly
   closely correspond to template lines, there's no real correlation to lines in
   the generated code. */

#ifndef SECURE_TEMPLATE_LINE_HPP
#define SECURE_TEMPLATE_LINE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <new>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
*/


using namespace std;

/* Note:  The following are sub-classed from security (above). */

class security_hash_template_line : public security_template_line
{
public:
    // Data members:
    // Methods:
        void setup_call (set_data_info set_data, bool random_data,
                 bool fill_in_template, bool create_call, template_line *temLin,
                 tf_fuzz_info *rsrc)
        {
            define_call<hash_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc);
        }
        bool copy_template_to_asset (void);
        bool copy_template_to_call (void);
        security_hash_template_line (tf_fuzz_info *resources);  // (constructor)
        ~security_hash_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif // #ifndef SECURE_TEMPLATE_LINE_HPP

