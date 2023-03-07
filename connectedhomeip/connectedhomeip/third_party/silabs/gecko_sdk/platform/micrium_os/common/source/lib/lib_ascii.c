/***************************************************************************//**
 * @file
 * @brief Common - Ascii Character Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_ascii.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               ASCII_IsAlpha()
 *
 * @brief    Determines whether a character is an alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is alphabetic.
 *           DEF_NO, the character is NOT alphabetic.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.2.(2) states that "isalpha() returns true only for the
 *               characters for which isupper() or islower() is true".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsAlpha(CPU_CHAR c)
{
  CPU_BOOLEAN alpha;

  alpha = ASCII_IS_ALPHA(c);

  return (alpha);
}

/****************************************************************************************************//**
 *                                           ASCII_IsAlphaNum()
 *
 * @brief    Determines whether a character is an alphanumeric character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is alphanumeric.
 *           DEF_NO, the character is NOT alphanumeric.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.1.(2) states that "isalnum() ... tests for any character
 *               for which isalpha() or isdigit() is true".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsAlphaNum(CPU_CHAR c)
{
  CPU_BOOLEAN alpha_num;

  alpha_num = ASCII_IS_ALPHA_NUM(c);

  return (alpha_num);
}

/****************************************************************************************************//**
 *                                               ASCII_IsLower()
 *
 * @brief    Determines whether a character is a lowercase alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is lowercase alphabetic.
 *           DEF_NO, the character is lowercase alphabetic.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.7.(2)  states that "islower() returns true only for
 *               the lowercase letters".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsLower(CPU_CHAR c)
{
  CPU_BOOLEAN lower;

  lower = ASCII_IS_LOWER(c);

  return (lower);
}

/****************************************************************************************************//**
 *                                               ASCII_IsUpper()
 *
 * @brief    Determines whether a character is an uppercase alphabetic character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is uppercase alphabetic.
 *           DEF_NO, the character is uppercase alphabetic.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.11.(2) states that "isupper() returns true only for
 *               the uppercase letters".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsUpper(CPU_CHAR c)
{
  CPU_BOOLEAN upper;

  upper = ASCII_IS_UPPER(c);

  return (upper);
}

/****************************************************************************************************//**
 *                                               ASCII_IsDig()
 *
 * @brief    Determines whether a character is a decimal-digit character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is a decimal-digit character.
 *           DEF_NO, the character is NOT a decimal-digit character.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.5.(2)  states that "isdigit()  ... tests for any
 *               decimal-digit character".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsDig(CPU_CHAR c)
{
  CPU_BOOLEAN dig;

  dig = ASCII_IS_DIG(c);

  return (dig);
}

/****************************************************************************************************//**
 *                                               ASCII_IsDigOct()
 *
 * @brief    Determines whether a character is an octal-digit character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is an octal-digit.
 *           DEF_NO, the character is NOT an octal-digit.
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsDigOct(CPU_CHAR c)
{
  CPU_BOOLEAN dig_oct;

  dig_oct = ASCII_IS_DIG_OCT(c);

  return (dig_oct);
}

/****************************************************************************************************//**
 *                                               ASCII_IsDigHex()
 *
 * @brief    Determine whether a character is a hexadecimal-digit character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is a hexadecimal-digit character.
 *           DEF_NO, the character is NOT a hexadecimal-digit character.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.12.(2) states that "isxdigit() ... tests for any
 *               hexadecimal-digit character".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsDigHex(CPU_CHAR c)
{
  CPU_BOOLEAN dig_hex;

  dig_hex = ASCII_IS_DIG_HEX(c);

  return (dig_hex);
}

/****************************************************************************************************//**
 *                                               ASCII_IsBlank()
 *
 * @brief    Determines whether a character is a standard blank character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is a standard blank character.
 *           DEF_NO, the character is NOT a standard blank character.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.3.(2) states that "isblank() returns true only for
 *               the standard blank characters".
 *
 * @note     (2) ISO/IEC 9899:TC2, Section 7.4.1.3.(2) defines "the standard blank characters" as
 *               the "space (' '), and horizontal tab ('\t')".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsBlank(CPU_CHAR c)
{
  CPU_BOOLEAN blank;

  blank = ASCII_IS_BLANK(c);

  return (blank);
}

/****************************************************************************************************//**
 *                                               ASCII_IsSpace()
 *
 * @brief    Determines whether a character is a white-space character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is a white-space character.
 *           DEF_NO, the character is NOT a white-space character.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.10.(2) states that "isspace() returns true only
 *               for the standard white-space characters".
 *
 * @note     (2) ISO/IEC 9899:TC2, Section 7.4.1.10.(2) defines "the standard white-space characters"
 *               as the "space (' '), form feed ('\f'), new-line ('\n'), carriage return ('\r'),
 *               horizontal tab ('\t'), and vertical tab ('\v')".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsSpace(CPU_CHAR c)
{
  CPU_BOOLEAN space;

  space = ASCII_IS_SPACE(c);

  return (space);
}

/****************************************************************************************************//**
 *                                               ASCII_IsPrint()
 *
 * @brief    Determines whether a character is a printing character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is a printing character.
 *           DEF_NO, the character is NOT a printing character.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.8.(2) states that "isprint() ... tests for any
 *               printing character including space (' ')".
 *
 * @note     (2) ISO/IEC 9899:TC2, Section 7.4.(3), Note 169, states that in "the seven-bit US
 *               ASCII character set, the printing characters are those whose values lie from
 *               0x20 (space) through 0x7E (tilde)".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsPrint(CPU_CHAR c)
{
  CPU_BOOLEAN print;

  print = ASCII_IS_PRINT(c);

  return (print);
}

/****************************************************************************************************//**
 *                                               ASCII_IsGraph()
 *
 * @brief    Determines whether a character is any printing character except a space character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is a graphic.
 *           DEF_NO, the character is NOT a graphic.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.6.(2) states that "isgraph() ... tests for any
 *               printing character except space (' ')".
 *
 * @note     (2) ISO/IEC 9899:TC2, Section 7.4.(3), Note 169, states that in "the seven-bit US
 *               ASCII character set, the printing characters are those whose values lie from
 *               0x20 (space) through 0x7E (tilde)".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsGraph(CPU_CHAR c)
{
  CPU_BOOLEAN graph;

  graph = ASCII_IS_GRAPH(c);

  return (graph);
}

/****************************************************************************************************//**
 *                                               ASCII_IsPunct()
 *
 * @brief    Determines whether a character is a punctuation character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is punctuation.
 *           DEF_NO, the character is NOT punctuation.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.9.(2) states that "ispunct() returns true for every
 *               printing character for which neither isspace() nor isalnum() is true".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsPunct(CPU_CHAR c)
{
  CPU_BOOLEAN punct;

  punct = ASCII_IS_PUNCT(c);

  return (punct);
}

/****************************************************************************************************//**
 *                                               ASCII_IsCtrl()
 *
 * @brief    Determines whether a character is a control character.
 *
 * @param    c   Character to examine.
 *
 * @return   DEF_YES, the character is a control character.
 *           DEF_NO, the character is NOT a control character.
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.1.4.(2) states that "iscntrl() ... tests for any
 *               control character".
 *
 * @note     (2) ISO/IEC 9899:TC2, Section 7.4.(3), Note 169, states that in "the seven-bit US
 *               ASCII character set, ... the control characters are those whose values lie from
 *               0 (NUL) through 0x1F (US), and the character 0x7F (DEL)".
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_IsCtrl(CPU_CHAR c)
{
  CPU_BOOLEAN ctrl;

  ctrl = ASCII_IS_CTRL(c);

  return (ctrl);
}

/****************************************************************************************************//**
 *                                               ASCII_ToLower()
 *
 * @brief    Converts an uppercase alphabetic character to its corresponding lowercase alphabetic
 *           character.
 *
 * @param    c   Character to convert.
 *
 * @return   Lowercase equivalent of 'c', if the character 'c' is an uppercase character (see Note
 *           #2a).
 *           Character 'c', otherwise (see Note #2c).
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.2.1.(2) states that "tolower() ... converts an
 *               uppercase letter to a corresponding lowercase letter".
 *
 * @note     (2) ISO/IEC 9899:TC2, Section 7.4.2.1.(3) states that :
 *           - (a) "if the argument is a character for which isupper() is true and there are
 *                   one or more corresponding characters ... for which islower() is true," ...
 *           - (b) "tolower() ... returns one of the corresponding characters;" ...
 *           - (c) "otherwise, the argument is returned unchanged."
 *******************************************************************************************************/
CPU_CHAR ASCII_ToLower(CPU_CHAR c)
{
  CPU_CHAR lower;

  lower = ASCII_TO_LOWER(c);

  return (lower);
}

/****************************************************************************************************//**
 *                                               ASCII_ToUpper()
 *
 * @brief    Converts a lowercase alphabetic character to its corresponding uppercase alphabetic
 *           character.
 *
 * @param    c   Character to convert.
 *
 * @return   Uppercase equivalent of 'c', if the character 'c' is a lowercase character (see Note
 *           #2a).
 *           Character 'c', otherwise (see Note #2c).
 *
 * @note     (1) ISO/IEC 9899:TC2, Section 7.4.2.2.(2) states that "toupper() ... converts a
 *               lowercase letter to a corresponding uppercase letter".
 *
 * @note     (2) ISO/IEC 9899:TC2, Section 7.4.2.2.(3) states that :
 *           - (a) "if the argument is a character for which islower() is true and there are
 *                   one or more corresponding characters ... for which isupper() is true," ...
 *           - (b) "toupper() ... returns one of the corresponding characters;" ...
 *           - (c) "otherwise, the argument is returned unchanged."
 *******************************************************************************************************/
CPU_CHAR ASCII_ToUpper(CPU_CHAR c)
{
  CPU_CHAR upper;

  upper = ASCII_TO_UPPER(c);

  return (upper);
}

/****************************************************************************************************//**
 *                                               ASCII_Cmp()
 *
 * @brief    Determines if two characters are identical (case-insensitive).
 *
 * @param    c1  First character.
 *
 * @param    c2  Second character.
 *
 * @return   DEF_YES, the characters are identical.
 *           DEF_NO, the characters are different.
 *******************************************************************************************************/
CPU_BOOLEAN ASCII_Cmp(CPU_CHAR c1,
                      CPU_CHAR c2)
{
  CPU_CHAR    c1_upper;
  CPU_CHAR    c2_upper;
  CPU_BOOLEAN cmp;

  c1_upper = ASCII_ToUpper(c1);
  c2_upper = ASCII_ToUpper(c2);
  cmp = (c1_upper == c2_upper) ? (DEF_YES) : (DEF_NO);

  return (cmp);
}
