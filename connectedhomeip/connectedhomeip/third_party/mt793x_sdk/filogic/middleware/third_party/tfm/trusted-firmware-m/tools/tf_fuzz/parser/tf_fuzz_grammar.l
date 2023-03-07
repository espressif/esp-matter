/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


%{
#include "class_forwards.hpp"
#include "boilerplate.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "string_ops.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "psa_call.hpp"
#include "tf_fuzz_grammar.tab.hpp"

int yycolumn = 1;

//char *yytext;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng; \
    yylval.str = strdup(yytext);

void yyerror (tf_fuzz_info *, const char *str)
    /* not sure why it sends the yyparse() argument to yyerror(), but OK... */
{
    fprintf (stderr, "tf_fuzz template on line %d, at text = \"%s\":  %s\n",
            yylineno, yytext, str);
    exit (1);
}

#ifndef yyterminate
#define yyterminate() return YY_NULL
#endif

%}

%x BLOCK_COMMENT

%option yylineno
%option nodefault
%option noyywrap
%array

%%

\r                        ;   /* ignore all \r */
\/\/.*\n                  ;   /* ignore C++-style line comments */
purpose[ \t]+[^;]*;       return PURPOSE;
"/*"                      {BEGIN(BLOCK_COMMENT);}
<BLOCK_COMMENT>"*/"       {BEGIN(INITIAL);}
<BLOCK_COMMENT>\n         ;
<BLOCK_COMMENT>.          ;
  /* Root commands: */
set                       return SET;
read                      return READ;
remove                    return REMOVE;
secure                    return SECURE;
done                      return DONE;
  /* PSA-asset types: */
sst                       return SST;
key                       return KEY;
policy                    return POLICY;
  /* Other root-command operands: */
expect                    return EXPECT;
pass                      return PASS;
nothing                   return NOTHING;
error                     return ERROR;
\{                        return OPEN_BRACE;
\}                        return CLOSE_BRACE;
;                         return SEMICOLON;
name                      return NAME;
uid                       return UID;
data                      return DATA;
\*                        return STAR;
active                    return ACTIVE;
deleted                   return DELETED;
check                     return CHECK;
assign                    return ASSIGN;
print                     return PRINT;
hash                      return HASH;
neq                       return NEQ;
dfname                    return DFNAME;
shuffle                   return SHUFFLE;
to                        return TO;
of                        return OF;
  /* Structure operands: */
[a-zA-z][a-zA-Z_0-9]*     {yylval.str = yytext; return IDENTIFIER_TOK;}
[+-]?[0-9]*               {yylval.valueN = atol(yytext); return NUMBER_TOK;}
\'[a-zA-Z_0-9\/\.]+\'     {yylval.str = yytext; return FILE_PATH_TOK;}
\"[^\"]*\"                {yylval.str = yytext; return LITERAL_TOK;}
                              /* inside quotes:  anything but a quote, or nothing */
[ \t\n\r]                 ;   /* ignore white space */
.                         yyerror ((tf_fuzz_info *) NULL, "Unexpected character");

%%

