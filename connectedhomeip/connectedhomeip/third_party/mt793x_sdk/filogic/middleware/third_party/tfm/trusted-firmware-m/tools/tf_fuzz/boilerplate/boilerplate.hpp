/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* boilerplate.*pp contains class(es) to read and store customizable code
   fragments used in realizing the pieces of a test.  It's critical that
   these be customizable in external library files, so that TF-Fuzz can
   exercise TF-A as well as TF-M.  Each TF-x has its own library of text
   pieces, which are read into these data structures upon program initial-
   ization. */

#ifndef BOILERPLATE_HPP
#define BOILERPLATE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;

// Constant mnemonics for entries in array of boilerplate texts:
const int
    // Stuff around the test itself:
        preamble_A = 0,  // setup stuff before PSA calls begin
            /* Strings to be substituted from template:
            $purpose:  The purpose of the test */
        hashing_code = 1,  // code to perform a simple hash of asset data
            /* Strings to be substituted from template:  (none) */
        preamble_B = 2,  // setup stuff before PSA calls begin
            /* Strings to be substituted from template:
            $purpose:  The purpose of the test */
        declare_int = 3,  // declaration for an int
            /* Strings to be substituted from template:
            $var:  The integer thing to declare
            $init:  Its initialization */
        declare_string = 4,  // declaration for a string
            /* Strings to be substituted from template:
            $var:  The string variable to declare
            $init:  Its initialization */
        declare_big_string = 5,  // declaration for a string, sized large
            /* Strings to be substituted from template:
            $var:  The string variable to declare
            $init:  Its initialization */
        declare_generic = 6,  // declaration for a some other type
            /* Strings to be substituted from template:
            $type:  The type to declare the variable to
            $var:  The string variable to declare
            $init:  Its initialization */
        test_log = 7,  // print a message to the test log
            /* Strings to be substituted from template:
            $message:  What to print. */
        teardown_sst = 8,  // call to delete SST resources after test completes
            /* Strings to be substituted from template:
              $uid:  (Exactly that) */
        teardown_sst_check = 9,  // boilerplate code to check success of previous;
            /* No strings to be substituted from template */
        teardown_key = 10,  // call to delete crypto keys after test completes
            /* Strings to be substituted from template:
              $handle:  The handle to the key */
        teardown_key_check = 11,  // boilerplate code to check success of previous;
            /* No strings to be substituted from template */
        closeout = 12,  // final code to close out the test.
            // No strings to substitute.
        sst_pass_string = 13,  // passing SST expected result
        sst_fail_removed = 14,  // SST expected result from having deleted the asset
    // SST calls:
        set_sst_call = 15,  // boilerplate for setting up an SST asset;
            /* Strings to be substituted from template:
              $op:  (for comment) either "Creating" or "Resetting"
              $description:  its "human name" if given, or "UID=..." if not
              $data_source:  either first ~10 chars of data or file path
              $uid:  (Exactly that)
              $length:  Data length
              $data:  Pointer to data (C string)
              $flags:  SST-asset creation flags */
        set_sst_check = 16,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status string */
        get_sst_call = 17,  // boilerplate for retrieving data from an SST asset;
            /* Strings to be substituted from template:
              $uid:  (Exactly that)
              $offset:  Start positon in the "file"  TO DO:  ADD OFFSET
              $length:  Data length
              $act_data:  Pointer to actual data (C string) */
        get_sst_check = 18,  // boilerplate code to check call result only;
            /* Strings to be substituted from template:
              $expect:  Expected return status string */
        get_sst_check_all = 19,  // boilerplate code to check call result and data;
            /* Strings to be substituted from template:
              $expect:  Expected return status
              $exp_data:  Expected read data
              $act_data:  Actual read data
              $length:  Data length */
        get_sst_hash = 20,  // boilerplate code to invoke hasher;
            /* Strings to be substituted from template:
              $act_data_var:  Actual read data, to be hashed
              $hash_var:  Hash-result variable */
        remove_sst = 21,  // boilerplate for removing an SST asset;
            /* Strings to be substituted from template:
              $uid:  (Exactly that) */
        remove_sst_check = 22,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
    // Crypto-key-policy calls:
        set_policy = 23,  // boilerplate for setting up a key policy;
            /* Strings to be substituted from template:
              $policy:  The key-policy to define
              $usage:  The key-policy usage
              $alg:  The key-policy algorithm */
        set_policy_check = 24,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
        get_policy = 25,  // boilerplate for retrieving data from a key policy;
            /* Strings to be substituted from template:
              $policy:  The key-policy to view */
        get_policy_check = 26,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
    // Crypto-key-related calls:
        set_key = 27,  // boilerplate for setting up a key;
            /* Strings to be substituted from template:
              $lifetime:  The lifetime of the key
              $handle:  The handle to the key */
        set_key_check = 28,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
        get_key = 29,  // boilerplate for retrieving data from a key;
            /* Strings to be substituted from template:
              $handle:  The handle to the key
              $type:  The returned key type
              $bits:  The returned size in bits */
        get_key_check = 30,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
        get_key_check_n_bits = 31,
            /* Strings to be substituted from template:
              $n_bits:  returned #bits (actually just left side of if (a!=b)
              $m_bits:  expected #bits (actually just right side of if (a!=b) */
        remove_key = 32,  // boilerplate for creating a key;
            /* Strings to be substituted from template:
              $handle:  The handle to the key */
        remove_key_check = 33,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
    // Just an anchor at the end, for sizing the string array (if needed):
        n_boilerplate_texts = 34;


class boilerplate
{
public:  // (More pain than it's worth to create getters and setters)
    // Data members:
    vector<string> bplate_string;
    string cpp_string;  // used to stuff bplate_string
    // Methods:
    void stuff_boilerplate_strings (void);
    boilerplate (void);
};


#endif  // #ifndef BOILERPLATE_HPP
