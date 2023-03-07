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

/**********************************************************************************
   Methods of class boilerplate follow:
**********************************************************************************/

void env_var_usage (void)
{
    cerr << "Example, for typical TF-M usage, using bash syntax:" << endl
         << "   export TF_FUZZ_LIB_DIR=<path to TF-M installation>/tools/tf_fuzz/lib"
         << endl
         << "   export TF_FUZZ_BPLATE=tfm_boilerplate.txt" << endl;
}

void boilerplate::stuff_boilerplate_strings (void)
{
    ifstream boilerplate_lib;
    string file_name;  // boilerplate file name
    string holder;
        // temp string, e.g., holder for strings read from the boilerplate file

    /* User must point the environment variable $TF_FUZZ_BPLATE to the boilerplate
       file s/he wants to use, within a directory named by $TF_FUZZ_LIB_DIR. */
    string bpStringL, bpStringF;
    char *bpLibDir = getenv ("TF_FUZZ_LIB_DIR");
    if (bpLibDir == NULL) {
        cerr << "Error:  Please point environment variable $TF_FUZZ_LIB_DIR to "
             << "TF-Fuzz's library directory." << endl;
        env_var_usage();
        exit (701);
    }
    bpStringL = bpLibDir;
    char *bpFName = getenv ("TF_FUZZ_BPLATE");
    if (bpFName == NULL) {
        cerr << "Error:  Please set environment variable $TF_FUZZ_BPLATE to name the "
             << "\"boilerplate\" text-library file." << endl;
        env_var_usage();
        exit (702);
    }
    bpStringF = bpFName;
    file_name.assign (bpStringL + "/" + bpStringF);
    boilerplate_lib.open (file_name);
    if (!boilerplate_lib.is_open()) {
        cerr << "\nError:  Unable to open boilerplate text-library file, at path "
             << file_name << "." << endl;
        env_var_usage();
        exit(200);
    }

    // Read the strings into the boilerplate vector:
    getline (boilerplate_lib, holder, '`');
        /* This first one is "not real."  It's a README, in essence, explaining the
           library-text file, so skip past it. */
    for (int i = preamble_A;  i < n_boilerplate_texts;  i++) {
        if (!getline (boilerplate_lib, holder, '`')) {
            cerr << "\nError:  Unable to read from boilerplate text-library file, at path "
                 << file_name << "." << endl;
            cerr << "        Please make sure the file is not empty." << endl;
            env_var_usage();
            exit(851);
        }
        if (holder.length() < 5) {
            cerr << "\nError:  Read from boilerplate text-library file, at path "
                 << file_name << ", was too short." << endl;
            cerr << "        Please make sure the file has not been damaged "
                 << "from the TF-Fuzz distribution." << endl;
            env_var_usage();
            exit(852);
        }
        // Shave off the three-character tag + \n from the front of the string:
        holder.erase(0, 4);
        bplate_string.push_back(holder);
    }
    boilerplate_lib.close();
}

boilerplate::boilerplate (void) {
    stuff_boilerplate_strings();
}

/**********************************************************************************
   End of methods of class boilerplate.
**********************************************************************************/
