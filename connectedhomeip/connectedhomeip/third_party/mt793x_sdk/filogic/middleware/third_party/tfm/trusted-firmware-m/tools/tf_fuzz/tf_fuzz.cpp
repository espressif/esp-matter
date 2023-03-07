/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <ctime>  // to seed random, if seed not passed in
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>  // for srand() and rand()
#include <cstdio>  // for template lex&yacc input file
#include "class_forwards.hpp"
#include "boilerplate.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "string_ops.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "tf_fuzz_grammar.tab.hpp"


extern FILE* yyin;  // telling lex&yacc which file to parse

using namespace std;

long psa_asset::unique_id_counter = 10;  // counts unique IDs for assets
long psa_call::unique_id_counter = 10;  // counts unique IDs for assets
    /* FYI:  Must initialize these class variables outside the class.  If
             initialized inside the class, g++ requires they be const. */

/**********************************************************************************
   Methods of class tf_fuzz_info follow:
**********************************************************************************/

asset_search tf_fuzz_info::find_or_create_sst_asset (
    psa_asset_search criterion,  // what to search on
    psa_asset_usage where,  // where to search
    string target_name,  // ignored if not searching on name
    uint64_t target_id,  // also ignored if not searching on ID (e.g., SST UID)
    long &serial_no,  // search by asset's unique serial number
    bool create_asset,  // true to create the asset if it doesn't exist
    vector<psa_asset*>::iterator &asset  // returns a pointer to requested asset
) {
    return generic_find_or_create_asset<sst_asset>(
               active_sst_asset, deleted_sst_asset,
               invalid_sst_asset, criterion, where, target_name, target_id,
               serial_no, create_asset, asset
           );
}

asset_search tf_fuzz_info::find_or_create_key_asset (
    psa_asset_search criterion,  // what to search on
    psa_asset_usage where,  // where to search
    string target_name,  // ignored if not searching on name
    uint64_t target_id,  // also ignored if not searching on ID (e.g., SST UID)
    long &serial_no,  // search by asset's unique serial number
    bool create_asset,  // true to create the asset if it doesn't exist
    vector<psa_asset*>:: iterator &asset  // returns iterator to requested asset
) {
    return generic_find_or_create_asset<key_asset>(
               active_key_asset, deleted_key_asset,
               invalid_key_asset, criterion, where, target_name, target_id,
               serial_no, create_asset, asset
           );
}

asset_search tf_fuzz_info::find_or_create_policy_asset (
    psa_asset_search criterion,  // what to search on
    psa_asset_usage where,  // where to search
    string target_name,  // ignored unless searching on name
    uint64_t target_id,  // also ignored unless searching on ID (e.g., SST UID)
    long &serial_no,  // search by asset's unique serial number
    bool create_asset,  // true to create the asset if it doesn't exist
    vector<psa_asset*>::iterator &asset  // returns iterator to requested asset
) {
    return generic_find_or_create_asset<policy_asset>(
               active_policy_asset, deleted_policy_asset,
               invalid_policy_asset, criterion, where, target_name, target_id,
               serial_no, create_asset, asset
           );
}

asset_search tf_fuzz_info::find_or_create_psa_asset (
    psa_asset_type asset_type,  // what type of asset to find
    psa_asset_search criterion,  // what to search on
    psa_asset_usage where,  // where to search
    string target_name,  // ignored if not searching on name
    uint64_t target_id,  // also ignored if not searching on ID (e.g., SST UID)
    long &serial_no,  // search by asset's unique serial number
    bool create_asset,  // true to create the asset if it doesn't exist
    vector<psa_asset*>::iterator &asset  // returns iterator to asset
) {
    switch (asset_type) {
        case psa_asset_type::sst:
            return find_or_create_sst_asset (
                criterion, where, target_name, target_id,
                serial_no, create_asset, asset);
            break;
        case psa_asset_type::key:
            return find_or_create_key_asset (
                criterion, where, target_name, target_id,
                serial_no, create_asset, asset);
            break;
        case psa_asset_type::policy:
            return find_or_create_policy_asset (
                criterion, where, target_name, target_id,
                serial_no, create_asset, asset);
            break;
        default:
            cerr << "\nError:  Internal:  Please report error "
                 << "#1503 to TF-Fuzz developers." << endl;
            exit (1500);
    }
}

// Remove any PSA resources used in the test.  Returns success==true, fail==false.
void tf_fuzz_info::teardown_test (void)
{
    string call;
    // Traverse through the SST-assets list, writing out remove commands:
    for (auto &asset : active_sst_asset) {
        call = bplate->bplate_string[teardown_sst];
        find_replace_1st ("$uid", to_string(asset->asset_id.id_n), call);
        call.append (bplate->bplate_string[teardown_sst_check]);
        output_C_file << call;
    }
    // Same, but with key assets:
    for (auto &asset : active_key_asset) {
        call = bplate->bplate_string[teardown_key];
        find_replace_1st ("$handle", asset->handle_str, call);
        call.append (bplate->bplate_string[teardown_key_check]);
        output_C_file << call;
    }
}

// Write out the test itself.
void tf_fuzz_info::write_test (void)
{
    string call;
    string work = bplate->bplate_string[preamble_A];  // a temporary workspace string

    // The test file should be open before calling this method.
    // Spit out the obligatory preamble:
    find_replace_all ("$purpose", test_purpose, work);
    output_C_file << work;

    // If using hashing, then spit out the hashing functions:
    if (include_hashing_code) {
        work = bplate->bplate_string[hashing_code];
        output_C_file << work;
    }

    // Print out the second half of the preamble code:
    work = bplate->bplate_string[preamble_B];
    find_replace_all ("$purpose", test_purpose, work);
    output_C_file << work;

    output_C_file << "\n\n    /* Variables (etc.) to initialize and check PSA "
                  << "assets: */" << endl;
    for (auto call : calls) {
        // Reminder:  calls is a vector of *pointers to* psa_call subclass objects.
        call->fill_in_prep_code();
        call->write_out_prep_code (output_C_file);
    }

    output_C_file << "\n\n    /* PSA calls to test: */" << endl;
    for (auto call : calls) {
        call->fill_in_command();  // (fills in check code too)
        output_C_file << endl;
        call->write_out_command (output_C_file);
        call->write_out_check_code (output_C_file);
    }

    output_C_file << "\n\n    /* Removing assets left over from testing: */"
                  << endl;
    teardown_test();

    // Seal the deal:
    output_C_file << bplate->bplate_string[closeout];

    // Close the template and test files:
    output_C_file.close();
    fclose (template_file);
}


/* Parse command-line parameters.  exit() if error(s) found.  Place results
   into resource object. */
void tf_fuzz_info::parse_cmd_line_params (int argc, char* argv[])
{
    int exit_val = 0;  // the linux return value, default 0, meaning all-good
    vector<string> cmd_line_parameter, cmd_line_switch;
        // (STL) vectors of hard cmd_line_parameter and cmd_line_switches
    int n_parameters = 0, n_switches = 0;
        // counting off cmd_line_parameter and cmd_line_switches while parsing
    char testc;

    // Parse arguments into lists of strings:
    for (int i = 1;  i < argc;  ++i) {
        if (argv[i][0] == '-') {  // cmd_line_switch
            if (argv[i][1] == '-') {  // double-dash
                cmd_line_switch.push_back (string(argv[i]+2));
            } else {  // single-dash cmd_line_switch;  fine either way
                cmd_line_switch.push_back (string(argv[i]+1));
            }
            ++n_switches;
        } else {  // hard cmd_line_parameter
            cmd_line_parameter.push_back(argv[i]);
            ++n_parameters;
        }
    }
    // If too-few or too many cmd_line_parameter supplied
    for (int i = 0;  i < n_switches;  ++i) {
        // If usage string requested...
        if (cmd_line_switch[i] == "h") {
            exit_val = 10;
        }
        // If verbose requested, make note:
        if (cmd_line_switch[i] == "v") {
            verbose_mode = true;
        }
    }
    if (exit_val == 10) {  // -h switch
        cout << "\nHow to run TF-Fuzz:" << endl;
    } else if (n_parameters < 2) {
        cerr << "\nToo few command-line parameters." << endl;
        exit_val = 11;
    } else if (n_parameters > 3) {
        cerr << "\nToo many command-line parameters." << endl;
        exit_val = 12;
    } else {
        template_file_name = cmd_line_parameter[0];
        template_file = fopen (template_file_name.c_str(), "r");
        test_output_file_name = cmd_line_parameter[1];
        output_C_file.open (test_output_file_name, ios::out);
        if (n_parameters == 3) {
            /* TODO:  The try-catch below doesn't always seem to work.  For now,
               manually "catch" the most basic problem: */
            testc = cmd_line_parameter[2][0];
            if (testc < '0' || testc > '9') {
                cerr << "\nError:  Random-seed value (third parameter) could "
                     << "not be interpreted as a number." << endl;
                rand_seed = 0;
            } else {
                try {
                    rand_seed = stol (cmd_line_parameter[2], 0, 0);
                } catch (int excep) {
                    excep = 0;  // (keep compiler from complaining about not using excep)
                    cerr << "\nWarning:  Random-seed value (third parameter) could "
                         << "not be interpreted as a number." << endl;
                    rand_seed = 0;
                }
            }
        }
        if (rand_seed == 0 || n_parameters < 3) {
            if (n_parameters < 3) {
                cout << "Info:  random seed was not specified." << endl;
            } else {
                cout << "Warning:  random seed, " << cmd_line_parameter[2]
                     << ", was not usable!" << endl;
            }
            srand((unsigned int) time(0));  // TODO:  ideally, XOR or add in PID#
            rand_seed = rand();
                /* doesn't really matter, but it just "feels better" when the
                   default seed value is itself more random. */
        }
        cout << endl << "Using seed value of " << dec << rand_seed << " " << hex
             << "(0x" << rand_seed << ")." << endl;
        srand(rand_seed);
        if (template_file == NULL) {
            cerr << "\nError:  Template file " << template_file_name
                 << " could not be opened." << endl;
            exit_val = 13;
        } else if (!output_C_file.is_open()) {
            // If test-output file couldn't be opened
            cerr << "\nError:  Output C test file " << test_output_file_name
                 << " could not be opened." << endl;
            exit_val = 14;
        }
        // Default (not entirely worthless) purpose of the test:
        test_purpose.assign (  "template = " + template_file_name + ", seed = "
                             + to_string(rand_seed));
    }
    // Bad command line, or request for usage blurb, so tell them how to run us:
    if (exit_val != 0) {
        cout << endl << argv[0] << " usage:" << endl;
        cout << "    Basic cmd_line_parameter (positional, in order, "
             << "left-to-right):" << endl;
        cout << "        Test-template file" << endl;
        cout << "        Test-output .c file" << endl;
        cout << "        (optional) random seed value" << endl;
        cout << "    Optional switches:" << endl;
        cout << "        -h or --h:  This help (command-line usage) summary."
             << endl;
        cout << "        -v or --v:  Verbose mode." << endl;
        cout << "Examples:" << endl;
        cout << "    " << argv[0] << " -h" << endl;
        cout << "    " << argv[0] << " template.txt output_test.c 0x5EED" << endl;
        exit (exit_val);
    }
}

tf_fuzz_info::tf_fuzz_info (void)  // (constructor)
{
    this->bplate = new boilerplate();
    test_purpose = template_file_name = test_output_file_name = "";
    rand_seed = 0;
    verbose_mode = false;
    include_hashing_code = false;  // default
}

tf_fuzz_info::~tf_fuzz_info (void)
{
    delete bplate;
}

/**********************************************************************************
   End of methods of class tf_fuzz_info.
**********************************************************************************/


int main(int argc, char* argv[])
{
    cout << "Trusted Firmware Fuzzer (TF-Fuzz) starting..." << endl << endl;

    // Allocate "the world":
    tf_fuzz_info *rsrc = new tf_fuzz_info;

    // Parse parameters and open files:
    rsrc->parse_cmd_line_params (argc, argv);

    // Parse the test-template file:
    yyin = rsrc->template_file;
    int parse_result = yyparse (rsrc);

    if (parse_result == 1) {
        cerr << "\nError:  Template file has errors." << endl;
    } else if (parse_result == 2) {
        cerr << "\nError:  Sorry, TF-Fuzz ran out of memory." << endl;
    }

    cout << "Writing test file, " << rsrc->test_output_file_name << "." << endl;
    rsrc->write_test();
    rsrc->output_C_file.close();

    cout << endl << "TF-Fuzz test generation complete." << endl;
    return 0;
}
