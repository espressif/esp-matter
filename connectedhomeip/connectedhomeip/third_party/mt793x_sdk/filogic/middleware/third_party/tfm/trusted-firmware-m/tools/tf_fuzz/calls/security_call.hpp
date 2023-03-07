/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SECURITY_CALL_HPP
#define SECURITY_CALL_HPP

#include <string>
#include <vector>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs. */


using namespace std;

class hash_call : public security_call
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
    // Methods:
        void fill_in_prep_code (void);
        void fill_in_command (void);
        /* Hash checks are different from the rest in that there's a single "call" --
           not a PSA call though -- for all of the assets cited in the template line.
           In other cases, create a single call for each asset cited by the template
           line, but in this case it's a single call for all of them. */
        hash_call (tf_fuzz_info *test_state, long &asset_ser_no,
                    asset_search how_asset_found);  // (constructor)
        ~hash_call (void);

protected:
    // Data members:
    // Methods:
//        void calc_result_code (void);  for *now* keep this in security_call::

private:
    // Data members:
    // Methods:
};

#endif // #ifndef SECURITY_CALL_HPP
