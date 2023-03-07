/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

%{
#include <iostream>
#include <vector>
#include <set>

#include "class_forwards.hpp"
#include "data_blocks.hpp"
#include "boilerplate.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "string_ops.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "crypto_call.hpp"
#include "sst_call.hpp"
#include "security_call.hpp"
#include "secure_template_line.hpp"
#include "sst_template_line.hpp"
#include "crypto_template_line.hpp"

/* These items are defined in tf_fuzz_grammar.l.  Note, however that, because
   of "name mangling," defining them as extern "C" may or may not be ideal,
   depending upon which compiler -- gcc vs. g++, compiles the output from lex.
   So far, it seems best without the extern "C", including also compiling
   under Visual Studio. */
/* extern "C"
{ */
  extern int yylineno;
  int yywrap() {return 1;}
  extern char yytext[];
  extern int yyleng;
/* } */

int yylex (void);
void yyerror (tf_fuzz_info *, const char *);
    /* not sure why it sends the yyparse() argument to yyerror(), but OK... */

/* A few consts just to make code more comprehensible: */
const bool yes_fill_in_template = true;
const bool dont_fill_in_template = false;
const bool yes_create_call = true;
const bool dont_create_call = false;

tf_fuzz_info *rsrc;

/* These are object pointers used to parse the template and create the test.  Ac-
   tually, probably only templateLin will be used. */
template_line                   *templateLin = nullptr;
  sst_template_line             *sstTemplateLin = nullptr;
    set_sst_template_line       *setSstTemplateLin = nullptr;
    read_sst_template_line      *reaSstTemplateLin = nullptr;
    remove_sst_template_line    *remSstTemplateLin = nullptr;
  policy_template_line          *polTemplateLin = nullptr;
    set_policy_template_line    *setPolTemplateLin = nullptr;
    read_policy_template_line   *reaPolTemplateLin = nullptr;
  key_template_line             *keyTemplateLin = nullptr;
    set_key_template_line       *setKeyTemplateLin = nullptr;
    read_key_template_line      *reaKeyTemplateLin = nullptr;
    remove_key_template_line    *remKeyTemplateLin = nullptr;
  security_template_line        *secTemplateLin = nullptr;
    security_hash_template_line *secHasTemplateLin = nullptr;
/* Call and asset objects are presumably not immediately needed, because the objects of
   these types are within the resource object, *rsrc, but even if just to show class
   hierarchy: */
psa_call                        *psaCal = nullptr;
  sst_call                      *sstCal = nullptr;
    sst_set_call                *sstSetCal = nullptr;
    sst_get_call                *sstGetCal = nullptr;
    sst_remove_call             *sstRemCal = nullptr;
  crypto_call                   *cryCal = nullptr;
    policy_call                 *polCal = nullptr;
      policy_set_call           *polSetCal = nullptr;
      policy_get_call           *polGetCal = nullptr;
    key_call                    *keyCal = nullptr;
      get_key_info_call         *getKeyInfCal = nullptr;
      set_key_call              *makKeyCal = nullptr;
      destroy_key_call          *desKeyCal = nullptr;
psa_asset                       *psaAst = nullptr;
  sst_asset                     *sstAst = nullptr;
  crypto_asset                  *cryAst = nullptr;
    policy_asset                *polAst = nullptr;
    key_asset                   *keyAst = nullptr;

/* For generating random, but readable/memorable, data: */
gibberish gib;
char gib_buff[4096];  // spew gibberish into here
int rand_data_length = 0;

/* General-utility variables: */
string purp_str;  /* test purpose */
psa_asset_usage random_asset = psa_asset_usage::all;  /* pick what type of asset at random */
bool random_name;  /* template didn't specify name, so it's generated randomly */
string literal_data;  /* literal data for an asset value */

/* Holders for state in read commands: */
expect_info expect;  /* everything about expected results and data */
set_data_info set_data;  /* everything about setting the value of PSA-asset data */
asset_name_id_info asset_id;  /* everything about identifying assets */
bool assign_data_var_specified;
string assign_data_var;
bool print_data;  /* true to just print asset data to the test log */
bool hash_data;  /* true to just print asset data to the test log */

/* The following are more tied to the template syntax than to the resulting PSA calls */
string literal;  /* temporary holder for all string literals */
string identifier;  /* temporary holder for strings representing identifiers */
string var_name;  /* a variable name */
string asset_name;  /* as parsed, not yet put into asset_id */
string aid;  /* string-typed holder for an asset ID in a list thereof */
int nid;  /* same idea as aid, but for asset ID# lists */
size_t strFind1, strFind2;  /* for searching through strings */

/* Because of the parsing order, psa_calls of the specific type have to be
   push_back()ed onto rsrc->calls before their expected results are known.  Therefore,
   inject those results after parsing the expected results.  add_expect is a vector
   index of where to start "back-filling" the expect information. */
unsigned int add_expect = 0;

/* Temporaries: */
vector<psa_asset*>::iterator t_sst_asset;
vector<psa_asset*>::iterator t_key_asset;
vector<psa_asset*>::iterator t_policy_asset;
sst_call *t_sst_call = nullptr;
key_call *t_key_call = nullptr;
policy_call *t_policy_call = nullptr;
long number;  /* temporary holder for a number, e.g., sting form of UID */
int i, j, k;

/* Relating to template-statement blocks: */
vector<template_line*> template_block_vector;  /* (must be *pointers to* templates) */
vector<int> block_order;  /* "statisticalized" order of template lines in a block */
int nesting_level = 0;
    /* how many levels deep in { } nesting currently.  Initially only 0 or 1. */
bool shuffle_not_pick;
    /* true to shuffle statements in a block, rather than pick so-and-so
       number of them at random. */
int low_nmbr_lines = 1;  /* if picking so-and-so number of template lines from a ... */
int high_nmbr_lines = 1; /*    ... block at random, these are fewest and most lines. */
int exact_nmbr_lines = 1;

/* Shortcuts, to reduce code clutter, and reduce risk of coding errors. */
#define IVM(content) if(rsrc->verbose_mode){content}  /* IVM = "If Verbose Mode" */

using namespace std;

/* randomize_template_lines() chooses a template-line order in cases where they are to
   be randomized -- shuffled or random picked. */
void randomize_template_lines (
    bool shuffle_not_pick,  /* true to perform a shuffle operation rather than pick */
    int &low_nmbr_lines, /* if picking so-and-so number of template lines from a ... */
    int &high_nmbr_lines, /*    ... block at random, these are fewest and most lines. */
    int &exact_nmbr_lines,
    vector<template_line*> &template_block_vector,
    vector<int> &block_order
) {
    set<int> template_used;  /* used for shuffle */
    low_nmbr_lines = (low_nmbr_lines < 0)?  0 : low_nmbr_lines;
    high_nmbr_lines = (high_nmbr_lines < 0)?  0 : high_nmbr_lines;
    if (low_nmbr_lines > high_nmbr_lines) {
        int swap = low_nmbr_lines;
        low_nmbr_lines = high_nmbr_lines;
        high_nmbr_lines = swap;
    }
    template_used.clear();
    if (shuffle_not_pick) {
        /* Choose a random order in which to generate all of the
           template lines in the block: */
        while (template_used.size() < template_block_vector.size()) {
            i = rand() % template_block_vector.size();
            if (template_used.find (i) == template_used.end()) {
                /* This template not already shuffled in. */
                block_order.push_back (i);
                template_used.insert (i);
            }
        }
        /* Done shuffling;  empty out the set: */
    } else {
        if (high_nmbr_lines == low_nmbr_lines) {
            exact_nmbr_lines = low_nmbr_lines;
                /* just in case the template says "3 to 3 of"... */
        } else {
            exact_nmbr_lines =   low_nmbr_lines
                               + (rand() % (  high_nmbr_lines
                                            - low_nmbr_lines + 1 )  );
        }
        for (int j = 0;  j < exact_nmbr_lines;  ++j) {
            /* Repeatedly choose a random template line from the block: */
            i = rand() % template_block_vector.size();
            block_order.push_back (i);
        }
    }
}

/* interpret_template_line() fills in random data, locates PSA assets, (etc.) and
   conditionally creates PSA calls for a given template line.  Note that there needs
   to be a single place where all of this is done, so that statement blocks can be
   randomized and then dispatched from a single point. */
void interpret_template_line (
    template_line *templateLin,  /* the template line to process */
    tf_fuzz_info *rsrc,  /* program resources in general */
    set_data_info set_data, psa_asset_usage random_asset,
    bool assign_data_var_specified, expect_info expect, bool print_data, bool hash_data,
    string asset_name, string assign_data_var,
    asset_name_id_info &asset_id,  /* everything about the asset(s) involved */
    bool create_call_bool,  /* true to create the PSA call at this time */
    bool create_asset_bool,  /* true to create the PSA asset at this time */
    bool fill_in_template,  /* true to back-fill info into template */
    int instance
        /* if further differentiation to the names or IDs is needed, make this >0 */
) {
    const bool yes_fill_in_template = true;  // just to clarify a call
    vector<psa_asset*> *active_asset, *deleted_asset;
    vector<psa_asset*>::iterator t_psa_asset;

    if (fill_in_template) {
        /* Set basic parameters from the template line: */
        templateLin->asset_id.id_n_not_name = asset_id.id_n_not_name;
        templateLin->set_data.random_data = set_data.random_data;
        templateLin->asset_id.set_name (asset_name);
        /* Fill in state parsed from the template below: */
        templateLin->assign_data_var_specified = assign_data_var_specified;
        templateLin->assign_data_var.assign (assign_data_var);
        templateLin->expect.data_var_specified
                = expect.data_var_specified;
        templateLin->expect.data_var.assign (expect.data_var);
        templateLin->expect.data_specified = expect.data_specified;
        templateLin->expect.data = expect.data;
        templateLin->print_data = print_data;
        templateLin->hash_data = hash_data;
        templateLin->random_asset = random_asset;
        templateLin->set_data.string_specified = set_data.literal_data_not_file;
            /* TODO:  is this right for multiple assets? */
        if (set_data.literal_data_not_file && !set_data.random_data) {
            templateLin->set_data.set (literal_data);
        }
        /* Save names or IDs to the template-line tracker: */
        for (auto id_no : asset_id.asset_id_n_vector) {
             templateLin->asset_id.asset_id_n_vector.push_back (id_no);
        }
        asset_id.asset_id_n_vector.clear();
        for (auto as_name : asset_id.asset_name_vector) {
             templateLin->asset_id.asset_name_vector.push_back (as_name);
        }
        asset_id.asset_name_vector.clear();
    }

    switch (templateLin->asset_type) {
        case psa_asset_type::sst:
            active_asset = &(rsrc->active_sst_asset);
            deleted_asset = &(rsrc->deleted_sst_asset);
            /* Currently "invalid" assets aren't used */
            break;
        case psa_asset_type::key:
            active_asset = &(rsrc->active_key_asset);
            deleted_asset = &(rsrc->deleted_key_asset);
            break;
        case psa_asset_type::policy:
            active_asset = &(rsrc->active_policy_asset);
            deleted_asset = &(rsrc->deleted_policy_asset);
            break;
        default:
            cerr << "\nError:  Internal:  Please report error "
                 << "#1502 to TF-Fuzz developers." << endl;
            exit (1500);
    }

    if (templateLin->random_asset != psa_asset_usage::all) {
        /* != psa_asset_usage::all means to choose some known asset at random: */
        if (templateLin->random_asset == psa_asset_usage::active) {
            if (active_asset->size() > 0) {
                i = rand() % active_asset->size();
                t_psa_asset = active_asset->begin() + i;
                templateLin->how_asset_found = asset_search::found_active;
                templateLin->asset_id.id_n = (*t_psa_asset)->asset_id.id_n;
                templateLin->asset_ser_no = (*t_psa_asset)->asset_ser_no;
                if (templateLin->is_remove) {
                    templateLin->set_data.string_specified
                        = (*t_psa_asset)->set_data.string_specified;
                    templateLin->asset_id.name_specified
                        = (*t_psa_asset)->asset_id.name_specified;
                    templateLin->asset_id.set_calc_name ((*t_psa_asset)->asset_id.get_name());
                    templateLin->set_data.set_calculated ((*t_psa_asset)->set_data.get());
                }
                templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                         yes_fill_in_template, create_call_bool,
                                         templateLin, rsrc   );
                if (templateLin->is_remove) {
                    /* Move asset from active vector to deleted vector: */
                    if (templateLin->how_asset_found == asset_search::found_active) {
                        deleted_asset->push_back(*t_psa_asset);
                        active_asset->erase(t_psa_asset);
                    }  /* if not active, deem the call expected to fail. */
                }
            } else {
                cerr << "\nWarning:  Request for random active asset, "
                     << "when no active assets of that type exist."
                     << "          Request disregarded." << endl;
            }
        } else if (templateLin->random_asset == psa_asset_usage::deleted) {
            if (deleted_asset->size() > 0) {
                i = rand() % deleted_asset->size();
                t_psa_asset = deleted_asset->begin() + i;
                templateLin->how_asset_found = asset_search::found_deleted;
                templateLin->asset_id.id_n = (*t_psa_asset)->asset_id.id_n;
                templateLin->asset_ser_no = (*t_psa_asset)->asset_ser_no;
                if (templateLin->is_remove) {
                    templateLin->set_data.string_specified
                        = (*t_psa_asset)->set_data.string_specified;
                    templateLin->asset_id.name_specified
                        = (*t_psa_asset)->asset_id.name_specified;
                    templateLin->asset_id.set_calc_name (
                                              (*t_psa_asset)->asset_id.get_name()    );
                    templateLin->set_data.set_calculated ((*t_psa_asset)->set_data.get());
                }
                templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                         yes_fill_in_template, create_call_bool,
                                         templateLin, rsrc   );
                if (templateLin->is_remove && create_call_bool) {
                    /* Move asset from active vector to deleted vector: */
                    if (templateLin->how_asset_found == asset_search::found_active) {
                        deleted_asset->push_back(*t_psa_asset);
                        active_asset->erase(t_psa_asset);
                    }  /* if not active, deem the call expected to fail. */
                }
            } else {
                cerr << "\nWarning:  Request for random deleted asset, "
                     << "when no deleted assets of that type exist."
                     << "          Request disregarded." << endl;
            }
        } /* "invalid" assets are not used, so if not psa_asset_usage::active or
             psa_asset_usage::deleted, then just ignore the template request */
    } else {
        if (asset_id.id_n_not_name) {  /* Not random asset, asset by ID */
            for (auto id_no :  templateLin->asset_id.asset_id_n_vector) {
                templateLin->asset_id.set_id_n(id_no);
                asset_name = templateLin->asset_id.make_id_n_based_name (
                               id_no + (uint64_t) instance * 10000, asset_name );
                templateLin->asset_id.set_calc_name (asset_name);
                templateLin->how_asset_found = rsrc->find_or_create_psa_asset (
                               templateLin->asset_type, psa_asset_search::id,
                               psa_asset_usage::all, "",
                               (id_no + (uint64_t) instance * 10000),
                               templateLin->asset_ser_no, create_asset_bool,
                               t_psa_asset );
                if (!templateLin->is_remove) {
                    /* Set asset's ID to what's being searched for (whether it's
                       already that because it's been found, or was just created): */
                    (*t_psa_asset)->asset_id.id_n = templateLin->asset_id.id_n;
                    templateLin->expect.data_var = var_name;
                    if (!set_data.literal_data_not_file) {
                        templateLin->set_data.set_file (set_data.file_path);
                    }
                    if (templateLin->how_asset_found != asset_search::not_found) {
                        templateLin->asset_id.id_n = (*t_psa_asset)->asset_id.id_n;
                        templateLin->asset_ser_no = (*t_psa_asset)->asset_ser_no;
                    }
                    templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                             fill_in_template, create_call_bool,
                                             templateLin, rsrc   );
                }
            }
        } else {  /* Not random asset, asset by name */
            for (auto as_name :  templateLin->asset_id.asset_name_vector) {
                /* Also copy into template line object's local vector: */
                string t_string, t_string2;
                t_string.assign(as_name);
                if (instance > 0) {
                    t_string += "_" + to_string (instance);
                }
                templateLin->asset_id.set_name (t_string);
                templateLin->how_asset_found = rsrc->find_or_create_psa_asset (
                                templateLin->asset_type, psa_asset_search::name,
                                psa_asset_usage::all, t_string,
                                (uint64_t) 0, templateLin->asset_ser_no,
                                create_asset_bool, t_psa_asset   );
                if (!templateLin->is_remove) {
                    /* Give each occurrence a different UID: */
                    templateLin->asset_id.set_id_n (100 + (rand() % 10000));
                        /* TODO:  unlikely, but this could alias! */
                    if (templateLin->how_asset_found != asset_search::not_found) {
                        templateLin->asset_id.id_n = (*t_psa_asset)->asset_id.id_n;
                        templateLin->asset_ser_no = (*t_psa_asset)->asset_ser_no;
                    }
                    templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                             yes_fill_in_template, create_call_bool,
                                             templateLin, rsrc   );
                }
            }
        }
        if (templateLin->is_remove) {
            if (templateLin->how_asset_found != asset_search::not_found) {
                templateLin->asset_ser_no = (*t_psa_asset)->asset_ser_no;
            }
            if (templateLin->how_asset_found == asset_search::unsuccessful) {
                cerr << "\nError:  Tool-internal:  Please report error #109 "
                     << "to the TF-Fuzz developers." << endl;
                exit(109);
            }
            templateLin->setup_call (set_data, templateLin->set_data.random_data,
                                     dont_fill_in_template, create_call_bool,
                                     templateLin, rsrc);
            templateLin->copy_template_to_asset();
            /* TODO:  See comment in setup_call() */
            if (create_call_bool && !templateLin->copy_template_to_call()) {
                cerr << "\nError:  Tool-internal:  Please report error "
                     << "#602 to the TF-Fuzz developers." << endl;
                exit(602);
            }

            /* TODO:  This ideally would be done in an organized "simulate" stage. */
            /* Move asset from active vector to deleted vector: */
            if (   create_call_bool  /* don't do this if just parsing */
                && templateLin->how_asset_found == asset_search::found_active) {
                rsrc->deleted_sst_asset.push_back(*t_psa_asset);
                rsrc->active_sst_asset.erase(t_psa_asset);
            }  /* if not active, deem the call expected to fail. */
        }
    }
}

%}

%start lines

%union {int valueN; int tokenN; char *str;}
%token <tokenN> PURPOSE RAW_TEXT
%token <tokenN> SET READ REMOVE SECURE DONE  /* root commands */
%token <tokenN> SST KEY POLICY NAME UID STAR ACTIVE DELETED EQUAL DATA DFNAME
%token <tokenN> CHECK ASSIGN HASH NEQ PRINT EXPECT PASS NOTHING ERROR  /* expected results */
%token <str> IDENTIFIER_TOK LITERAL_TOK FILE_PATH_TOK  /* variables and content */
%token <valueN> NUMBER_TOK  /* variables and content */
%token <tokenN> SEMICOLON SHUFFLE TO OF OPEN_BRACE CLOSE_BRACE  /* block structure */

%define parse.error verbose
%locations
%parse-param {tf_fuzz_info *rsrc}

%%

  /* Top-level syntax: */
lines:      /* nothing */
      | line lines {
            IVM(cout << "Lines:  Line number " << yylineno << "." << endl;)
        }
      ;

line:
        PURPOSE {
            IVM(cout << "Purpose line:  " << flush;)
            purp_str = yytext;
            strFind1 = purp_str.find (" ");
            purp_str = purp_str.substr (strFind1, purp_str.length());
            purp_str.erase (0, 1);  // (extra space)
            strFind1 = purp_str.rfind (";");
            purp_str = purp_str.substr (0, strFind1);
            rsrc->test_purpose = purp_str;
            IVM(cout << rsrc->test_purpose << endl;)
            /* Just a precaution to make sure that these vectors start out empty.
               Should be, and purpose is typically specified first: */
            asset_id.asset_id_n_vector.clear();
            asset_id.asset_name_vector.clear();
        }
      | block {
            /* TODO:  This code may not won't work with "secure hash neq ..." */
            IVM(cout << "Block of lines." << endl;)
            /* "Statisticalize" :-) the vector of template lines, then crank
               the selected lines in order here. */
            randomize_template_lines (shuffle_not_pick,
                low_nmbr_lines, high_nmbr_lines, exact_nmbr_lines,
                template_block_vector, block_order
            );
            IVM(cout << "Order of lines in block:  " << flush;
                 for (auto i : block_order) {
                     cout << i << "  ";
                 }
                 cout << endl;
            )
            /* Vector block_order contains the sequence of template lines to be
               realized, in order.  Pop the indicated template line off the
               vector and generate code from it: */
            k = 0;  /* ID adder to at least help ensure uniqueness */
            for (int i : block_order) {
                templateLin = template_block_vector[i];
                /* Note that temLin will have its fields filled in already. */
                interpret_template_line (
                    templateLin, rsrc, set_data, random_asset,
                    assign_data_var_specified, expect, print_data, hash_data,
                    asset_name, assign_data_var, asset_id,
                    yes_create_call,  /* did not create call nor asset earlier */
                    yes_create_asset,
                    dont_fill_in_template,  /* but did fill it all in before */
                    k
                );
                k++;
                for (;  add_expect < rsrc->calls.size();  ++add_expect) {
                    templateLin->expect.copy_expect_to_call (rsrc->calls[add_expect]);
                }
            }
             templateLin->asset_id.asset_id_n_vector.clear();
             templateLin->asset_id.asset_name_vector.clear();
            /* Done.  Empty out the "statisticalization" vector: */
            block_order.clear();
            /* Empty out the vector of template lines; no longer needed. */
            template_block_vector.clear();
            --nesting_level;
            IVM(cout << "Finished coding block of lines." << endl;)
        }
      | command SEMICOLON {
            IVM(cout << "Command with no expect:  \"" << flush;)
            if (nesting_level == 0) {  /* if laying down the code now... */
                for (;  add_expect < rsrc->calls.size();  ++add_expect) {
                    templateLin->expect.copy_expect_to_call (rsrc->calls[add_expect]);
                }
                delete templateLin;  /* done with this template line */
            } else {
                /* The template line is now fully decoded, so stuff it onto
                   vector of lines to be "statisticalized": */
                template_block_vector.push_back (templateLin);
            }
            IVM(cout << yytext << "\"" << endl;)
        }
      | command expect SEMICOLON {
            IVM(cout << "Command with expect:  \"" << flush;)
            if (nesting_level == 0) {
                for (;  add_expect < rsrc->calls.size();  ++add_expect) {
                    templateLin->expect.copy_expect_to_call (rsrc->calls[add_expect]);
                }
                delete templateLin;  /* done with this template line */
            } else {
                template_block_vector.push_back (templateLin);
            }
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

command:
        set_command {
            IVM(cout << "Set command:  \"" << yytext << "\"" << endl;)
        }
      | remove_command {
            IVM(cout << "Remove command:  \"" << yytext << "\"" << endl;)
        }
      | read_command {
            IVM(cout << "Read command:  \"" << yytext << "\"" << endl;)
        }
      | secure_command {
            IVM(cout << "Security command:  \"" << yytext << "\"" << endl;)
        }
      | done_command {
            IVM(cout << "Done command:  \"" << yytext << "\"" << endl;)
        }
      ;

expect:
        EXPECT PASS  {
            IVM(cout << "Expect pass clause:  \"" << flush;)
            templateLin->expect.set_pf_pass();
            IVM(cout << yytext << "\"" << endl;)
        }
      | EXPECT NOTHING {
            IVM(cout << "Expect nothing clause:  \"" << flush;)
            templateLin->expect.set_pf_nothing();
            IVM(cout << yytext << "\"" << endl;)
        }
      | EXPECT IDENTIFIER {
            IVM(cout << "Expect error clause:  \"" << flush;)
            templateLin->expect.set_pf_error (identifier);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

  /* Root commands: */
set_command:
        SET sst_set_args {
            IVM(cout << "Set SST command:  \"" << flush;)
            templateLin = new set_sst_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                nesting_level == 0 /* similarly, create asset unless inside {} */,
                yes_fill_in_template, 0
            );
            IVM(cout << yytext << "\"" << endl;)
        }
      | SET key_set_args {
            IVM(cout << "Set key command:  \"" << flush;)
            templateLin = new set_key_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                nesting_level == 0 /* similarly, create asset unless inside {} */,
                yes_fill_in_template, 0
            );
            IVM(cout << yytext << "\"" << endl;)
        }
      | SET policy_set_args {
            IVM(cout << "Set policy command:  \"" << flush;)
            templateLin = new set_policy_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                nesting_level == 0 /* similarly, create asset unless inside {} */,
                yes_fill_in_template, 0
            );
            rsrc->calls.push_back (t_policy_call);
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

remove_command:
        REMOVE sst_remove_args {
            IVM(cout << "Remove SST command:  \"" << flush;)
            templateLin = new remove_sst_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* don't create an asset being deleted */,
                yes_fill_in_template, 0
            );
            IVM(cout << yytext << "\"" << endl;)
        }
      | REMOVE key_remove_args {
            IVM(cout << "Remove key command:  \"" << flush;)
            templateLin = new remove_key_template_line (rsrc);
            templateLin->asset_id.set_name (asset_name);  // set in key_asset_name, below
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* don't create an asset being deleted */,
                yes_fill_in_template, 0
            );
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

read_command:
        READ SST sst_read_args {
            IVM(cout << "Read SST command:  \"" << flush;)
            templateLin = new read_sst_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* if no such asset exists, fail the call */,
                yes_fill_in_template, 0
            );
            IVM(cout << yytext << "\"" << endl;)
        }
      | READ KEY key_read_args {
            IVM(cout << "Read key command:  \"" << flush;)
            templateLin = new read_key_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* if no such asset exists, fail the call */,
                yes_fill_in_template, 0
            );
            IVM(cout << yytext << "\"" << endl;)
        }
      | READ POLICY policy_read_args {
            IVM(cout << "Read policy command:  \"" << flush;)
            templateLin = new read_policy_template_line (rsrc);
            interpret_template_line (
                templateLin, rsrc, set_data, random_asset,
                assign_data_var_specified, expect, print_data, hash_data,
                asset_name, assign_data_var, asset_id,
                nesting_level == 0 /* create call unless inside {} */,
                dont_create_asset /* if no such asset exists, fail the call */,
                yes_fill_in_template, 0
            );
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

secure_command: SECURE HASH NEQ ASSET_IDENTIFIER_LIST {
  /* TODO:  This needs to allow not only SST assets, but mix and match with others
             (keys especially) as well. */
            templateLin = new security_hash_template_line (rsrc);
            templateLin->asset_id.set_name (asset_name);  // set in sst_asset_name, below
            /* Fill in state parsed out below: */
            templateLin->assign_data_var_specified = assign_data_var_specified;
            templateLin->assign_data_var.assign (assign_data_var);
/* TODO:  Replace the below with templateLin->expect = expect? */
            templateLin->expect.data_var_specified = expect.data_var_specified;
            templateLin->expect.data_var.assign (expect.data_var);
            templateLin->expect.data_specified = expect.data_specified;
            templateLin->expect.data = literal;
            templateLin->print_data = print_data;
            templateLin->hash_data = hash_data;
            templateLin->random_asset = random_asset;
            /* Hash checks are different from the rest in that there's a single
               "call" -- not a PSA call though -- for all of the assets cited in the
               template line.  In other cases, create a single call for each
               asset cited by the template line, but not in this case. */
            for (auto as_name : asset_id.asset_name_vector) {
                /* Also copy into template line object's local vector: */
                 templateLin->asset_id.asset_name_vector.push_back (as_name);
            }
            /* Don't need to locate the assets, so no searches required. */
            templateLin->expect.data_var = var_name;
            templateLin->setup_call (set_data, set_data.random_data, yes_fill_in_template,
                                     nesting_level == 0, templateLin, rsrc   );
            asset_id.asset_name_vector.clear();
            IVM(cout << yytext << "\"" << endl;)


        }
      ;

done_command: DONE {
            if (nesting_level != 0) {
                cerr << "\n\"done\" only available at outer-most { } nesting level."
                     << endl;
                exit (702);
            } else {
                YYACCEPT;
            }
        }
      ;

  /* Root-command parameters: */
sst_set_args:
        SST sst_asset_name DATA LITERAL {
            IVM(cout << "SST-create from literal data:  \"" << flush;)
            set_data.random_data = false;
            set_data.literal_data_not_file = true;
            literal.erase(0,1);    // zap the ""s
            literal.erase(literal.length()-1,1);
            literal_data.assign (literal);
            IVM(cout << yytext << "\"" << endl;)
        }
      | SST sst_asset_name DATA STAR {  /* TF-Fuzz supplies random data */
            IVM(cout << "SST-create from random data" << endl;)
            set_data.random_data = true;
            set_data.literal_data_not_file = true;
            rand_data_length = 40 + (rand() % 256);  /* Note:  Multiple assets do get different data */
            gib.sentence (gib_buff, gib_buff + rand_data_length - 1);
            set_data.set (gib_buff);
            literal.assign (gib_buff);  /* just in case something uses literal */
        }
      | SST sst_asset_name DFNAME sst_asset_set_file_path {
            set_data.literal_data_not_file = set_data.random_data = false;
            IVM(cout << "SST-create from file:  " << yytext << "\"" << endl;)
            /* TODO:  Need to decide whether the concept of using files to set SST
                       asset values has meaning, and then write code to write code to
                       set data appropriately from the file. */
        }
      ;

sst_read_args:
        sst_asset_name ASSIGN IDENTIFIER {  /* dump to variable */
            IVM(cout << "SST-read dump to variable:  \"" << flush;)
            /* TODO:  set_data content probably doesn't need to be set here;
                       constructor probably sets it fine. */
            set_data.random_data = false;
            set_data.literal_data_not_file = true;
            assign_data_var.assign (identifier);
            assign_data_var_specified = true;
            expect.data_specified = false;
            expect.data_var_specified = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | sst_asset_name CHECK sst_read_args_var_name {  /* check against variable */
            IVM(cout << "SST-read check against variable:  \""
                     << yytext << "\"" << endl;)
            /* TODO:  set_data content probably doesn't need to be set here;
                       constructor probably sets it fine. */
            set_data.random_data = false;
            set_data.literal_data_not_file = true;  /* most importantly not file */
            set_data.set (literal);
            assign_data_var_specified = false;
            expect.data_specified = false;
            expect.data_var_specified = true;
            expect.data_var = identifier;
        }
      | sst_asset_name CHECK LITERAL {  /* check against literal */
            IVM(cout << "SST-read check against literal:  " << flush;)
            /* TODO:  set_data content probably doesn't need to be set here;
                       constructor probably sets it fine. */
            set_data.random_data = false;
            set_data.literal_data_not_file = true;
            expect.data.assign (literal);
            expect.data.erase(0,1);    // zap the ""s
            expect.data.erase(expect.data.length()-1,1);
            assign_data_var_specified = false;
            expect.data_specified = true;
            expect.data_var_specified = false;
            IVM(cout << yytext << endl;)
        }
      | sst_asset_name PRINT {  /* print out content in test log */
            IVM(cout << "SST-read log to test log:  \"" << flush;)
            /* TODO:  set_data content probably doesn't need to be set here;
                       constructor probably sets it fine. */
            set_data.random_data = false;
            set_data.literal_data_not_file = true;
            assign_data_var_specified = false;
            expect.data_specified = false;
            expect.data_var_specified = false;
            print_data = true;
            IVM(cout << yytext << "\"" << endl;)
        }
      | sst_asset_name HASH {  /* hash the data and save for later comparison */
            IVM(cout << "SST-read hash for future data-leak detection:  \"" << flush;)
            /* TODO:  set_data content probably doesn't need to be set here;
                       constructor probably sets it fine. */
            set_data.random_data = false;
            set_data.literal_data_not_file = true;
            assign_data_var_specified = false;
            expect.data_specified = false;
            expect.data_var_specified = false;
            hash_data = true;
            rsrc->include_hashing_code = true;
            IVM(cout << yytext << "\"" << endl;)
        }
      | sst_asset_name DFNAME sst_asset_dump_file_path {  /* dump to file */
            IVM(cout << "SST-read dump to file:  \""
                     << yytext << "\"" << endl;)
            set_data.literal_data_not_file = set_data.random_data = false;
        }
      ;

sst_remove_args:
      SST sst_asset_name {
            IVM(cout << "SST-remove arguments:  \""
                     << yytext << "\"" << endl;)
      }
      ;

sst_asset_name:
        NAME ASSET_IDENTIFIER_LIST {
            IVM(cout << "SST-asset identifier list:  \"" << flush;)
            random_name = false;
            asset_name.assign (identifier);  /* TODO:  Not sure this ultimately has any effect... */
            random_asset = psa_asset_usage::all;  /* don't use random asset */
            asset_id.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | NAME STAR {
            IVM(cout << "SST-asset random identifier:  \"" << flush;)
            random_name = true;
            rand_data_length = 2 + (rand() % 10);
            gib.word (false, gib_buff, gib_buff + rand_data_length - 1);
            aid.assign (gib_buff);
            asset_id.asset_name_vector.push_back (aid);
            random_asset = psa_asset_usage::all;  /* don't use random asset */
            asset_id.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | UID ASSET_NUMBER_LIST {
            IVM(cout << "SST-asset UID list:  \"" << flush;)
            random_name = false;
            random_asset = psa_asset_usage::all;  /* don't use random asset */
            asset_id.id_n_not_name = true;
            asset_id.id_n_specified = true;
            IVM(cout << yytext << "\"" << endl;)
        }
      | UID STAR {
            IVM(cout << "SST-asset random UID:  \"" << flush;)
            asset_id.id_n_not_name = true;
            random_name = false;
            nid = 100 + (rand() % 10000);
            asset_id.asset_id_n_vector.push_back (nid);
            random_asset = psa_asset_usage::all;  /* don't use random asset */
            IVM(cout << yytext << "\"" << endl;)
        }
      | STAR ACTIVE {
            IVM(cout << "SST-asset random active:  \"" << flush;)
            random_asset = psa_asset_usage::active;
            asset_id.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      | STAR DELETED {
            IVM(cout << "SST-asset random deleted:  \"" << flush;)
            random_asset = psa_asset_usage::deleted;
            asset_id.id_n_not_name = false;
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

sst_asset_set_file_path:
      FILE_PATH {
            IVM(cout << "SST-asset-create file path:  \"" << flush;)
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

sst_read_args_var_name:
      IDENTIFIER {
            IVM(cout << "SST-read-arguments variable name:  \"" << flush;)
            var_name = yytext;
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

sst_asset_dump_file_path:
      FILE_PATH {
            IVM(cout << "SST-asset dump-file path:  \"" << flush;)
            set_data.file_path = yytext;
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

key_set_args:
      KEY key_id POLICY policy_asset_name {
            IVM(cout << "Key-create arguments:  \""
                     << yytext << "\"" << endl;)
      }
      ;

key_remove_args:
      KEY key_id {
            IVM(cout << "Key-remove arguments:  \""
                     << yytext << "\"" << endl;)
      }
      ;

key_read_args:
      KEY key_id key_read_var_name {
            IVM(cout << "Key dump to variable:  \""
                     << yytext << "\"" << endl;)
      }
      ;

key_read_var_name:
      IDENTIFIER {
            IVM(cout << "Key-read variable name:  \""
                     << yytext << "\"" << endl;)
      }
      ;

key_id:
      IDENTIFIER {
            IVM(cout << "Key ID:  \""
                     << yytext << "\"" << endl;)
      }
      ;

policy_set_args:
      POLICY policy_asset_name {
            IVM(cout << "Policy-create arguments:  \""
                     << yytext << "\"" << endl;)
      }
      ;

policy_read_args:
      POLICY policy_asset_name policy_read_var_name {
            IVM(cout << "Policy dump to variable:  \""
                     << yytext << "\"" << endl;)
      }
      ;

policy_asset_name:
      IDENTIFIER {
            IVM(cout << "Policy Asset ID:  \"" << flush;)
            asset_name = identifier;
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

policy_read_var_name:
      IDENTIFIER {
            IVM(cout << "Policy read variable name:  \""
                     << yytext << "\"" << endl;)
      }
      ;

/* Code structuring: */
block:
        SHUFFLE block_content {
            IVM(cout << "Shuffled block:  \"" << flush;)
            if (nesting_level > 1) {
                cerr << "\nError:  Sorry, currently only one level of { } "
                     << "nesting is allowed." << endl;
                exit (500);
            }
            shuffle_not_pick = true;
            low_nmbr_lines = high_nmbr_lines = 0;  /* not used, but... */
            IVM(cout << yytext << "\"" << endl;)
        }
      | exact_sel_count OF block_content {
            IVM(cout << "Fixed number of lines from block:  \"" << flush;)
            shuffle_not_pick = false;
            /* low_nmbr_lines and high_nmbr_lines are set below. */
            IVM(cout << yytext << "\"" << endl;)
        }
      | low_sel_count TO high_sel_count OF block_content {
            IVM(cout << "Range number of lines from block:  \"" << flush;)
            if (nesting_level > 1) {
                cerr << "\nError:  Sorry, currently only one level of { } "
                     << "nesting is allowed." << endl;
                exit (502);
            }
            shuffle_not_pick = false;
            /* low_nmbr_lines and high_nmbr_lines are set below. */
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

block_content:
        open_brace lines close_brace {
            IVM(cout << "Block content:  \"" << yytext << "\"" << endl;)
        }
      | line {
            IVM(cout << "Single-line would-be-block content:  \"" << flush;)
            IVM(cout << yytext << "\"" << endl;)
        }
      ;

open_brace:   OPEN_BRACE {
            IVM(cout << "Open brace:  \"" << flush;)
            template_block_vector.clear();  // clean slate of template lines
            nesting_level = 1;
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

close_brace:  CLOSE_BRACE {
            IVM(cout << "Close brace:  " << flush;)
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

  /* Low-level structures: */

  /* Please see comment before ASSET_IDENTIFIER_LIST, below. */
ASSET_NUMBER_LIST:    ASSET_NUMBER ASSET_NUMBERS;  /* at least one number */

ASSET_NUMBERS:      /* nothing, or */
      | ASSET_NUMBER ASSET_NUMBERS;

ASSET_NUMBER:     NUMBER_TOK {
            IVM(cout << "ASSET_NUMBER:  \"" << flush;)
            nid = atol(yytext);
            asset_id.asset_id_n_vector.push_back (nid);
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

  /* ASSET_IDENTIFIER* are used specifically for lists of assets in a template line.
     That, as opposed to list of identifers in general.  The difference is the need
     to queue ASSET_IDENTIFIERS up into asset_id.asset_name_vector, and have to do so
     here before they "vanish." */
ASSET_IDENTIFIER_LIST:  ASSET_IDENTIFIER ASSET_IDENTIFIERS;  /* (at least one) */

ASSET_IDENTIFIERS:
        /* nothing, or */
      | ASSET_IDENTIFIER ASSET_IDENTIFIERS;

ASSET_IDENTIFIER: IDENTIFIER_TOK {
            IVM(cout << "ASSET_IDENTIFIER:  \"" << flush;)
            aid = identifier = yytext;
            asset_id.asset_name_vector.push_back (aid);
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

IDENTIFIER: IDENTIFIER_TOK {
            IVM(cout << "IDENTIFIER:  \"" << flush;)
            identifier = yytext;
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

FILE_PATH: FILE_PATH_TOK {
            IVM(cout << "FILE_PATH:  \"" << flush;)
            set_data.file_path = yytext;
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

  /* These are related to randomized blocks of template lines: */

exact_sel_count:  NUMBER {
            IVM(cout << "Exact number of random template lines:  \"" << flush;)
            low_nmbr_lines = high_nmbr_lines = exact_nmbr_lines = number;
            ++nesting_level;
            IVM(cout << number << "\"" << endl;)
      }
      ;

low_sel_count:  NUMBER {
            IVM(cout << "Least number of random template lines:  \"" << flush;)
            low_nmbr_lines = number;
            IVM(cout << number << "\"" << endl;)
      }
      ;

high_sel_count:  NUMBER {
            IVM(cout << "Most number of random template lines:  \"" << flush;)
            high_nmbr_lines = number;
            ++nesting_level;
            IVM(cout << number << "\"" << endl;)
      }
      ;


  /* These are general-case numbers, literals and such: */

NUMBER:   NUMBER_TOK {
            IVM(cout << "NUMBER:  \"" << flush;)
            number = atol(yytext);
            IVM(cout << yytext << "\"" << endl;)
      }
      ;

LITERAL:  LITERAL_TOK {
            IVM(cout << "LITERAL:  " << flush;)
            literal = yytext;
            IVM(cout << yytext << endl;)
      }
      ;


%%

