/***************************************************************************//**
 * @file
 * @brief Common - Ascii String Management
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

/****************************************************************************************************//**
 * @note     (1) The Institute of Electrical and Electronics Engineers and The Open Group, have given us
 *               permission to reprint portions of their documentation. Portions of this text are
 *               reprinted and reproduced in electronic form from the IEEE Std 1003.1, 2004 Edition,
 *               Standard for Information Technology -- Portable Operating System Interface (POSIX), The
 *               Open Group Base Specifications Issue 6, Copyright (C) 2001-2004 by the Institute of
 *               Electrical and Electronics Engineers, Inc and The Open Group. In the event of any
 *               discrepancy between these versions and the original IEEE and The Open Group Standard, the
 *               original IEEE and The Open Group Standard is the referee document. The original Standard
 *               can be obtained online at http://www.opengroup.org/unix/online.html.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 *
 * Note(s) : (1) The file 'stdarg.h' is included ONLY for the use of variable arguments definitions such
 *               as va_start, va_list and va_end. It is included in a .c file in order to minimize chances
 *               of clashes with part of the software. Nothing else from this file should be used.
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>
#include  <common/source/lib/lib_str_priv.h>
//                                                                 See Note #1.
#include  <stdarg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct str_sprintf_cb_arg {
  CPU_CHAR   *BufPtr;
  CPU_SIZE_T CurIx;
} STR_SPRINTF_CB_ARG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const CPU_INT32U Str_MultOvfThTbl_Int32U[] = {
  (CPU_INT32U) DEF_INT_32U_MAX_VAL,                             // Invalid base  0.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  1u),                      // Invalid base  1.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  2u),                      // 32-bit mult ovf th for base  2.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  3u),                      // 32-bit mult ovf th for base  3.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  4u),                      // 32-bit mult ovf th for base  4.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  5u),                      // 32-bit mult ovf th for base  5.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  6u),                      // 32-bit mult ovf th for base  6.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  7u),                      // 32-bit mult ovf th for base  7.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  8u),                      // 32-bit mult ovf th for base  8.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL /  9u),                      // 32-bit mult ovf th for base  9.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 10u),                      // 32-bit mult ovf th for base 10.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 11u),                      // 32-bit mult ovf th for base 11.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 12u),                      // 32-bit mult ovf th for base 12.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 13u),                      // 32-bit mult ovf th for base 13.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 14u),                      // 32-bit mult ovf th for base 14.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 15u),                      // 32-bit mult ovf th for base 15.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 16u),                      // 32-bit mult ovf th for base 16.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 17u),                      // 32-bit mult ovf th for base 17.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 18u),                      // 32-bit mult ovf th for base 18.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 19u),                      // 32-bit mult ovf th for base 19.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 20u),                      // 32-bit mult ovf th for base 20.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 21u),                      // 32-bit mult ovf th for base 21.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 22u),                      // 32-bit mult ovf th for base 22.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 23u),                      // 32-bit mult ovf th for base 23.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 24u),                      // 32-bit mult ovf th for base 24.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 25u),                      // 32-bit mult ovf th for base 25.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 26u),                      // 32-bit mult ovf th for base 26.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 27u),                      // 32-bit mult ovf th for base 27.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 28u),                      // 32-bit mult ovf th for base 28.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 29u),                      // 32-bit mult ovf th for base 29.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 30u),                      // 32-bit mult ovf th for base 30.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 31u),                      // 32-bit mult ovf th for base 31.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 32u),                      // 32-bit mult ovf th for base 32.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 33u),                      // 32-bit mult ovf th for base 33.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 34u),                      // 32-bit mult ovf th for base 34.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 35u),                      // 32-bit mult ovf th for base 35.
  (CPU_INT32U)(DEF_INT_32U_MAX_VAL / 36u)                       // 32-bit mult ovf th for base 36.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_CHAR *Str_FmtNbr_Int32(CPU_INT32U  nbr,
                                  CPU_INT08U  nbr_dig,
                                  CPU_INT08U  nbr_base,
                                  CPU_BOOLEAN nbr_neg,
                                  CPU_CHAR    lead_char,
                                  CPU_BOOLEAN lower_case,
                                  CPU_BOOLEAN nul,
                                  CPU_CHAR    *p_str);

static CPU_INT32U Str_ParseNbr_Int32(const CPU_CHAR *p_str,
                                     CPU_CHAR       **p_str_next,
                                     CPU_INT08U     nbr_base,
                                     CPU_BOOLEAN    nbr_signed,
                                     CPU_BOOLEAN    *p_nbr_neg);

static CPU_SIZE_T Str_FmtOutput(CPU_CHAR          *p_str,
                                CPU_SIZE_T        min_char_cnt,
                                CPU_SIZE_T        max_char_cnt,
                                CPU_BOOLEAN       left_justify,
                                STR_PRINTF_OUT_CB out_cb,
                                void              *p_out_cb_arg,
                                CPU_INT32S        rem_size);

static CPU_SIZE_T Str_UnsgnIntFmtOutput(CPU_INT64U        nbr,
                                        CPU_INT08U        base,
                                        CPU_SIZE_T        min_digit_cnt,
                                        CPU_BOOLEAN       left_justify,
                                        CPU_CHAR          pad_char,
                                        STR_PRINTF_OUT_CB out_cb,
                                        void              *p_out_cb_arg,
                                        CPU_INT32S        rem_size);

static CPU_SIZE_T Str_SngIntFmtOutput(CPU_INT64S        nbr,
                                      CPU_SIZE_T        min_digit_cnt,
                                      CPU_BOOLEAN       left_justify,
                                      CPU_CHAR          pad_char,
                                      STR_PRINTF_OUT_CB out_cb,
                                      void              *p_out_cb_arg,
                                      CPU_INT32S        rem_size);

static CPU_INT16U Str_PrintfImpl(STR_PRINTF_OUT_CB out_cb,
                                 void              *p_out_cb_arg,
                                 CPU_INT16U        max_char_cnt,
                                 const CPU_CHAR    *format,
                                 va_list           argp);

static int Str_SprintfCb(int  c,
                         void *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                   Str_Len()
 *
 * @brief    Calculates the length of a string.
 *
 * @param    p_str   Pointer to the string. This string is not modified.
 *
 * @return   Length of a string; number of characters in a string before the terminating NULL
 *           character.
 *
 * @note     (1) See notes for Str_Len_N().
 *******************************************************************************************************/
CPU_SIZE_T Str_Len(const CPU_CHAR *p_str)
{
  CPU_SIZE_T len;

  len = Str_Len_N(p_str,
                  DEF_INT_CPU_U_MAX_VAL);

  return (len);
}

/****************************************************************************************************//**
 *                                               Str_Len_N()
 *
 * @brief    Calculates the length of a string, up to a maximum number of characters.
 *
 * @param    p_str       Pointer to the string. This string is not modified.
 *
 * @param    len_max     Maximum number of characters to search. Does NOT include the terminating NULL
 *                       character.
 *
 * @return   Length of string; number of characters in string before terminating NULL character, if
 *           terminating NULL character found.
 *           Requested maximum number of characters to search, if terminating NULL character NOT
 *           found.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strlen() : DESCRIPTION' states that :
 *               "The strlen() function shall compute the number of bytes in the string to which 's'
 *               ('p_str') points, [...] not including the terminating null byte."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strlen() : RETURN VALUE' states that :
 *               "The strlen() function shall return the length of 's' ('p_str'); [and that] no return
 *               value shall be reserved to indicate an error."
 *
 * @note     (3) String length calculation terminates when :
 *               - (a) String pointer points to NULL.
 *               - (b) Terminating NULL character found.
 *******************************************************************************************************/
CPU_SIZE_T Str_Len_N(const CPU_CHAR *p_str,
                     CPU_SIZE_T     len_max)
{
  const CPU_CHAR *p_str_len;
  CPU_SIZE_T     len;

  p_str_len = p_str;
  len = 0u;
  while ((p_str_len != DEF_NULL)                                // Calc str len until NULL ptr (see Note #3a) ...
         && (*p_str_len != (CPU_CHAR)'\0')                      // ... or NULL char found      (see Note #3b) ...
         && (len < (CPU_SIZE_T)len_max)) {                      // ... or max nbr chars srch'd.
    p_str_len++;
    len++;
  }

  return (len);
}

/****************************************************************************************************//**
 *                                               Str_Copy()
 *
 * @brief    Copies the source string to destination string buffer.
 *
 * @param    p_str_dest  Pointer to the destination string buffer to receive source string copy.
 *                       Destination buffer size is NOT validated; buffer overruns MUST be prevented
 *                       by caller. Destination buffer size MUST be large enough to accommodate the
 *                       entire source string size including the terminating NULL character.
 *
 * @param    p_str_src   Pointer to the source string to copy into destination string buffer. This
 *                       string is not modified.
 *
 * @return   Pointer to the destination string, if NO error(s).
 *           Pointer to NULL, otherwise (see Note #2).
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strcpy() : DESCRIPTION' states that :
 *               "The strcpy() function shall copy the string pointed to by 's2' ('p_str_src') [...]
 *               into the array pointed to by 's1' ('p_str_dest') [...] (including the terminating
 *               null byte)."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strcpy() : RETURN VALUE' states that :
 *               "The strcpy() function shall return 's1' ('p_str_dest'); [and that] no return value
 *               is reserved to indicate an error."
 *               This requirement is intentionally NOT implemented in order to return NULL for any
 *               error(s).
 *
 * @note     (3) IEEE Std 1003.1, 2004 Edition, Section 'strcpy() : DESCRIPTION' states that:
 *               "if copying takes place between objects that overlap, the behavior is undefined".
 *
 * @note     (4) String copy terminates when :
 *               - (a) Destination/Source string pointer(s) are passed as NULL pointers. No string copy
 *                     performed; NULL pointer is returned.
 *               - (b) Destination/Source string pointer(s) point to NULL. String buffer(s) overlap with
 *                     NULL address; NULL pointer is returned.
 *               - (c) Source string's terminating NULL character found. Entire source string copied
 *                     into destination string buffer.
 *******************************************************************************************************/
CPU_CHAR *Str_Copy(CPU_CHAR       *p_str_dest,
                   const CPU_CHAR *p_str_src)
{
  CPU_CHAR *p_str_rtn;

  p_str_rtn = Str_Copy_N(p_str_dest,
                         p_str_src,
                         DEF_INT_CPU_U_MAX_VAL);

  return (p_str_rtn);
}

/****************************************************************************************************//**
 *                                               Str_Copy_N()
 *
 * @brief    Copies the source string to the destination string buffer, up to a maximum number of
 *           characters.
 *
 * @param    p_str_dest  Pointer to the destination string buffer to receive source string copy.
 *                       Destination buffer size is NOT validated; buffer overruns MUST be prevented
 *                       by caller. Destination buffer size MUST be large enough to accommodate the
 *                       entire source string size including the terminating NULL character.
 *
 * @param    p_str_src   Pointer to the source string to copy into destination string buffer. This
 *                       string is not modified.
 *
 * @param    len_max     Maximum number of characters to copy. 'len_max' number of characters MAY
 *                       include the terminating NULL character. Zero-length copies allowed.
 *
 * @return   Pointer to destination string, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strncpy() : DESCRIPTION' states that :
 *               "The strncpy() function shall copy [...] the array pointed to by 's2' ('p_str_src')
 *               to the array pointed to by 's1' ('p_str_dest') [...] but "not more than 'n'
 *               ('len_max') bytes [...] & (bytes that follow a null byte are not copied)".
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strncpy() : DESCRIPTION' states that :
 *               "if the array pointed to by 's2' ('p_str_src') is a string that is shorter than 'n'
 *               ('len_max') bytes, null bytes shall be appended to the copy in the array pointed to
 *               by 's1' ('p_str_dest'), until 'n' ('len_max') bytes in all are written."
 *               Since Str_Copy() limits the maximum number of characters to copy via Str_Copy_N() by
 *               the CPU's maximum number of addressable characters, this requirement is intentionally
 *               NOT implemented to avoid appending a potentially large number of unnecessary
 *               terminating NULL characters.
 *
 * @note     (3) IEEE Std 1003.1, 2004 Edition, Section 'strncpy() : APPLICATION USAGE' states that :
 *               "if there is no null byte in the first 'n' ('len_max') bytes of the array pointed to
 *               by 's2' ('p_str_src'), the result is not null-terminated".
 *
 * @note     (4) IEEE Std 1003.1, 2004 Edition, Section 'strncpy() : RETURN VALUE' states that :
 *               "The strncpy() function shall return 's1' ('p_str_dest'); [...] no return value is
 *               reserved to indicate an error."
 *               This requirement is intentionally ignored in order to return NULL for any error(s).
 *
 * @note     (5) IEEE Std 1003.1, 2004 Edition, Section 'strncpy() : DESCRIPTION' states that :
 *               "if copying takes place between objects that overlap, the behavior is undefined".
 *
 * @note     (6) String copy terminates when :
 *               - (a) Destination/Source string pointer(s) are passed as NULL pointers. No string copy
 *                     performed; NULL pointer is returned.
 *               - (b) Destination/Source string pointer(s) point to NULL. String buffer(s) overlap with
 *                     NULL address; NULL pointer is returned.
 *               - (c) Source string's terminating NULL character found. Entire source string copied
 *                     into destination string buffer.
 *               - (d) 'len_max' number of characters copied.
 *******************************************************************************************************/
CPU_CHAR *Str_Copy_N(CPU_CHAR       *p_str_dest,
                     const CPU_CHAR *p_str_src,
                     CPU_SIZE_T     len_max)
{
  CPU_CHAR       *p_str_copy_dest;
  const CPU_CHAR *p_str_copy_src;
  CPU_SIZE_T     len_copy;

  //                                                               Rtn NULL if str ptr(s) NULL (see Note #6a).
  if (p_str_dest == DEF_NULL) {
    return (DEF_NULL);
  }
  if (p_str_src == DEF_NULL) {
    return (DEF_NULL);
  }

  p_str_copy_dest = p_str_dest;
  p_str_copy_src = p_str_src;
  len_copy = 0u;

  while ((p_str_copy_dest != DEF_NULL)                          // Copy str until NULL ptr(s) (see Note #6b) ...
         && (p_str_copy_src != DEF_NULL)
         && (*p_str_copy_src != (CPU_CHAR)'\0')                 // ... or NULL char found (see Note #6c); ...
         && (len_copy < len_max)) {                             // ... or max nbr chars copied (see Note #6d).
    *p_str_copy_dest = *p_str_copy_src;
    p_str_copy_dest++;
    p_str_copy_src++;
    len_copy++;
  }

  //                                                               Rtn NULL if NULL ptr(s) found (see Note #6b).
  if ((p_str_copy_dest == DEF_NULL)
      || (p_str_copy_src == DEF_NULL)) {
    return (DEF_NULL);
  }

  if (len_copy < len_max) {                                     // If copy str len < max buf len (see Note #2), ...
    *p_str_copy_dest = (CPU_CHAR)'\0';                          // ... copy NULL char (see Note #6c).
  }

  return (p_str_dest);
}

/****************************************************************************************************//**
 *                                                   Str_Cat()
 *
 * @brief    Appends the concatenation string to the destination string.
 *
 * @param    p_str_dest  Pointer to the destination string to append concatenation string. Destination
 *                       buffer size is NOT validated; buffer overruns MUST be prevented by caller.
 *                       Destination buffer size MUST be large enough to accommodate the entire
 *                       concatenated string size including the terminating NULL character.
 *
 * @param    p_str_cat   Pointer to the concatenation string to append to destination string.
 *                       Concatenation string buffer NOT modified.
 *
 * @return   Pointer to the destination string, if NO error(s).
 *           Pointer to NULL, otherwise (see Note #2).
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strcat() : DESCRIPTION' states that :
 *               "The strcat() function shall append a copy of the string pointed to by 's2'
 *               ('p_str_cat') [...] to the end of the string pointed to by 's1' ('p_str_dest')."
 *               "The initial byte of 's2' ('p_str_cat') overwrites the null byte at the end of 's1'
 *               ('p_str_dest'). [...] A "terminating null byte" is appended at the end of the
 *               concatenated destination strings.
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strcat() : RETURN VALUE' states that :
 *               "The strcat() function shall return 's1' ('p_str_dest'); [and that] no return value
 *               shall be reserved to indicate an error."
 *               This requirement is intentionally NOT implemented in order to return NULL for any
 *               error(s).
 *
 * @note     (3) IEEE Std 1003.1, 2004 Edition, Section 'strcat() : DESCRIPTION' states that :
 *               "if copying takes place between objects that overlap, the behavior is undefined."
 *
 * @note     (4) String concatenation terminates when :
 *               - (a) Destination/Concatenation string pointer(s) are passed as NULL pointers. No
 *                     string concatenation performed; NULL pointer is returned.
 *               - (b) Destination/Concatenation string pointer(s) point to NULL. String buffer(s)
 *                     overlap with NULL address; NULL pointer is returned.
 *               - (c) Concatenation string's terminating NULL character found. Entire concatenation
 *                     string appended to destination string.
 *******************************************************************************************************/
CPU_CHAR *Str_Cat(CPU_CHAR       *p_str_dest,
                  const CPU_CHAR *p_str_cat)
{
  CPU_CHAR *p_str_rtn;

  p_str_rtn = Str_Cat_N(p_str_dest,
                        p_str_cat,
                        DEF_INT_CPU_U_MAX_VAL);

  return (p_str_rtn);
}

/****************************************************************************************************//**
 *                                               Str_Cat_N()
 *
 * @brief    Appends concatenation string to destination string, up to a maximum number of characters.
 *
 * @param    p_str_dest  Pointer to the destination string to append concatenation string. Destination
 *                       buffer size is NOT validated; buffer overruns MUST be prevented by caller.
 *                       Destination buffer size MUST be large enough to accommodate the entire
 *                       concatenated string size including the terminating NULL character.
 *
 * @param    p_str_cat   Pointer to the concatenation string to append to destination string.
 *                       Concatenation string buffer NOT modified.
 *
 * @param    len_max     Maximum number of characters to concatenate. 'len_max' number of characters
 *                       does NOT include the terminating NULL character. Zero-length concatenations
 *                       allowed.
 *
 * @return   Pointer to the destination string, if NO error(s).
 *           Pointer to NULL, otherwise (see Note #2).
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strncat() : DESCRIPTION' states that :
 *               "The strncat() function shall append [...] the array pointed to by 's2' ('p_str_cat')
 *               to the end of the string pointed to by 's1' ('p_str_dest') [...] but not more than
 *               'n' ('len_max') bytes".
 *               "The initial byte of 's2' ('p_str_cat') overwrites the null byte at the end of 's1'
 *               ('p_str_dest'). [...] (a null byte and bytes that follow it are not appended). [...]
 *               A terminating null byte is always appended to the result."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strncat() : RETURN VALUE' states that :
 *               "The strncat() function shall return 's1' ('p_str_dest'); [and that]  no return value
 *               shall be reserved to indicate an error."
 *               This requirement is intentionally NOT implemented in order to return NULL for any
 *               error(s).
 *
 * @note     (3) IEEE Std 1003.1, 2004 Edition, Section 'strncat() : DESCRIPTION' states that :
 *               "if copying takes place between objects that overlap, the behavior is undefined."
 *
 * @note     (4) String concatenation terminates when :
 *               - (a) Destination/Concatenation string pointer(s) are passed as NULL pointers. No
 *                     string concatenation performed; NULL pointer is returned.
 *               - (b) Destination/Concatenation string pointer(s) point to NULL. String buffer(s)
 *                     overlap with NULL address; NULL pointer is returned.
 *               - (c) Concatenation string's terminating NULL character found. Entire concatenation
 *                     string appended to destination string.
 *               - (d) 'len_max' number of characters concatenated.
 *******************************************************************************************************/
CPU_CHAR *Str_Cat_N(CPU_CHAR       *p_str_dest,
                    const CPU_CHAR *p_str_cat,
                    CPU_SIZE_T     len_max)
{
  CPU_CHAR       *p_str_cat_dest;
  const CPU_CHAR *p_str_cat_src;
  CPU_SIZE_T     len_cat;

  //                                                               Rtn NULL if str ptr(s) NULL (see Note #4a).
  if (p_str_dest == DEF_NULL) {
    return (DEF_NULL);
  }
  if (p_str_cat == DEF_NULL) {
    return (DEF_NULL);
  }

  if (len_max < 1) {                                            // Rtn dest str if cat len = 0.
    return ((CPU_CHAR *)p_str_dest);
  }

  p_str_cat_dest = p_str_dest;
  while ((p_str_cat_dest != DEF_NULL)                           // Adv to end of cur dest str until NULL ptr ...
         && (*p_str_cat_dest != (CPU_CHAR)'\0')) {              // ... or NULL char found..
    p_str_cat_dest++;
  }

  if (p_str_cat_dest == DEF_NULL) {                             // Rtn NULL if NULL ptr found (see Note #4b).
    return (DEF_NULL);
  }

  p_str_cat_src = p_str_cat;
  len_cat = 0u;

  while ((p_str_cat_dest != DEF_NULL)                           // Cat str until NULL ptr(s) (see Note #4b) ...
         && (p_str_cat_src != DEF_NULL)
         && (*p_str_cat_src != (CPU_CHAR)'\0')                  // ... or NULL char found (see Note #4c); ...
         && (len_cat < (CPU_SIZE_T)len_max)) {                  // ... or max nbr chars cat'd (see Note #4d).
    *p_str_cat_dest = *p_str_cat_src;
    p_str_cat_dest++;
    p_str_cat_src++;
    len_cat++;
  }

  //                                                               Rtn NULL if NULL ptr(s) found (see Note #4b).
  if ((p_str_cat_dest == DEF_NULL)
      || (p_str_cat_src == DEF_NULL)) {
    return (DEF_NULL);
  }

  *p_str_cat_dest = (CPU_CHAR)'\0';                             // Append NULL char (see Note #1).

  return (p_str_dest);
}

/****************************************************************************************************//**
 *                                                   Str_Cmp()
 *
 * @brief    Determines if two strings are identical.
 *
 * @param    p1_str  Pointer to the first string. String buffer NOT modified.
 *
 * @param    p2_str  Pointer to the second string. String buffer NOT modified.
 *
 * @return   0, if strings are identical.
 *           Negative value, if 'p1_str' is less than 'p2_str'.
 *           Positive value, if 'p1_str' is greater than 'p2_str'.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strcmp() : DESCRIPTION' states that :
 *               "the strcmp() function shall compare the string pointed to by 's1' ('p1_str') to the
 *               string pointed to by 's2' ('p2_str) [and] the sign of a non-zero return value shall
 *               be determined by the sign of the difference between the values of the first pair of
 *               bytes [...] that differ in the strings being compared".
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strcmp() : RETURN VALUE' states that :
 *               "upon successful completion, strcmp() shall return an integer greater than, equal to,
 *               or less than 0".
 *
 * @note     (3) String comparison terminates when :
 *               - (a) BOTH string pointer(s) are passed as NULL pointers. Return 0.
 *               - (b) 'p1_str' passed a NULL pointer. Return negative value of character pointed to by
 *                     'p2_str'.
 *               - (c) 'p2_str' passed a NULL pointer. Return positive value of character pointed to by
 *                     'p1_str'.
 *               - (d) BOTH strings at some point point to NULL. Strings overlap with NULL address.
 *                     Strings identical up to but NOT beyond or including the NULL address. Return 0.
 *               - (e) 'p1_str_cmp_next' points to NULL; 'p1_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return negative
 *                     value of character pointed to by 'p2_str_cmp_next'.
 *               - (f) 'p2_str_cmp_next' points to NULL; 'p2_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return positive
 *                     value of character pointed to by 'p1_str_cmp_next'.
 *               - (g) Terminating NULL character found in both strings. Strings identical. Return 0.
 *               - (h) Non-matching characters found. Return signed-integer difference of the character
 *                     pointed to by 'p2_str' from the character pointed to by 'p1_str'.
 *
 * @note     (4) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_CHAR' native data type size MUST be 8-bit.
 *******************************************************************************************************/
CPU_INT16S Str_Cmp(const CPU_CHAR *p1_str,
                   const CPU_CHAR *p2_str)
{
  CPU_INT16S cmp_val;

  cmp_val = Str_Cmp_N(p1_str,
                      p2_str,
                      DEF_INT_CPU_U_MAX_VAL);

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                               Str_Cmp_N()
 *
 * @brief    Determines if two strings are identical for up to a maximum number of characters.
 *
 * @param    p1_str      Pointer to the first string. String buffer NOT modified.
 *
 * @param    p2_str      Pointer to the second string. String buffer NOT modified.
 *
 * @param    len_max     Maximum number of characters to compare.
 *
 * @return   0, if strings are identical.
 *           Negative value, if 'p1_str' is less than 'p2_str'.
 *           Positive value, if 'p1_str' is greater than 'p2_str'.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strncmp() : DESCRIPTION' states that :
 *               "The strncmp() function shall compare [...] the array pointed to by 's1' ('p1_str')
 *               to the array pointed to by 's2' ('p2_str) [...] but "not more than 'n' ('len_max')
 *               bytes" of either array. [...] [T]he sign of a non-zero return value is determined by
 *               the sign of the difference between the values of the first pair of bytes [...] that
 *               differ in the strings being compared".
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strncmp() : RETURN VALUE' states that :
 *               "upon successful completion, strncmp() shall return an integer greater than, equal
 *               to, or less than 0".
 *               - (a) BOTH string pointer(s) are passed as NULL pointers. Return 0.
 *               - (b) 'p1_str' passed a NULL pointer. Return negative value of character pointed to by
 *                     'p2_str'.
 *               - (c) 'p2_str' passed a NULL pointer. Return positive value of character pointed to by
 *                     'p1_str'.
 *               - (d) BOTH strings at some point point to NULL. Strings overlap with NULL address.
 *                     Strings identical up to but NOT beyond or including the NULL address. Return 0.
 *               - (e) 'p1_str_cmp_next' points to NULL; 'p1_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return negative
 *                     value of character pointed to by 'p2_str_cmp_next'.
 *               - (f) 'p2_str_cmp_next' points to NULL; 'p2_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return positive
 *                     value of character pointed to by 'p1_str_cmp_next'.
 *               - (g) Terminating NULL character found in both strings. Strings identical. Return 0.
 *               - (h) Non-matching characters found. Return signed-integer difference of the character
 *                     pointed to by 'p2_str' from the character pointed to by 'p1_str'.
 *               - (i) 'len_max' passed a zero length. Zero-length strings identical; 0 returned.
 *               - (j) First 'len_max' number of characters identical. Strings identical; 0 returned.
 *
 * @note     (3) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_CHAR' native data type size MUST be 8-bit.
 *******************************************************************************************************/
CPU_INT16S Str_Cmp_N(const CPU_CHAR *p1_str,
                     const CPU_CHAR *p2_str,
                     CPU_SIZE_T     len_max)
{
  const CPU_CHAR *p1_str_cmp;
  const CPU_CHAR *p2_str_cmp;
  const CPU_CHAR *p1_str_cmp_next;
  const CPU_CHAR *p2_str_cmp_next;
  CPU_INT16S     cmp_val;
  CPU_SIZE_T     cmp_len;

  if (len_max < 1) {                                            // If cmp len = 0, rtn 0 (see Note #2i).
    return (0);
  }

  if (p1_str == DEF_NULL) {
    if (p2_str == DEF_NULL) {
      return (0);                                               // If BOTH str ptrs NULL, rtn 0 (see Note #2a).
    }
    cmp_val = (CPU_INT16S)((CPU_INT16S)0 - (CPU_INT16S)(*p2_str));
    return (cmp_val);                                           // If p1_str NULL, rtn neg p2_str val (see Note #2b).
  }
  if (p2_str == DEF_NULL) {
    cmp_val = (CPU_INT16S)(*p1_str);
    return (cmp_val);                                           // If p2_str NULL, rtn pos p1_str val (see Note #2c).
  }

  p1_str_cmp = p1_str;
  p2_str_cmp = p2_str;
  p1_str_cmp_next = p1_str_cmp;
  p2_str_cmp_next = p2_str_cmp;
  p1_str_cmp_next++;
  p2_str_cmp_next++;
  cmp_len = 0u;

  while ((*p1_str_cmp == *p2_str_cmp)                           // Cmp strs until non-matching chars (see Note #2h) ...
         && (*p1_str_cmp != (CPU_CHAR)'\0')                     // ... or NULL chars (see Note #2g) ...
         && (p1_str_cmp_next != DEF_NULL)                       // ... or NULL ptr(s) found (see Notes #2d, #2e, #2f).
         && (p2_str_cmp_next != DEF_NULL)
         && (cmp_len < (CPU_SIZE_T)len_max)) {                  // ... or max nbr chars cmp'd (see Note #2j).
    p1_str_cmp++;
    p2_str_cmp++;
    p1_str_cmp_next++;
    p2_str_cmp_next++;
    cmp_len++;
  }

  if (cmp_len == len_max) {                                     // If strs identical for max len nbr of chars, ...
    return (0);                                                 // ... rtn 0 (see Note #2j).
  }

  if (*p1_str_cmp != *p2_str_cmp) {                             // If strs NOT identical, ...
                                                                // ... calc & rtn char diff (see Note #2h).
    cmp_val = (CPU_INT16S)((CPU_INT16S)(*p1_str_cmp) - (CPU_INT16S)(*p2_str_cmp));
  } else if (*p1_str_cmp == (CPU_CHAR)'\0') {                   // If NULL char(s) found, ...
    cmp_val = (CPU_INT16S)0;                                    // ... strs identical; rtn 0 (see Note #2g).
  } else {
    if (p1_str_cmp_next == DEF_NULL) {
      if (p2_str_cmp_next == DEF_NULL) {                        // If BOTH next str ptrs NULL, ...
        cmp_val = (CPU_INT16S)0;                                // ... rtn 0 (see Note #2d).
      } else {                                                  // If p1_str_cmp_next NULL, ...
                                                                // ... rtn neg p2_str_cmp_next val (see Note #2e).
        cmp_val = (CPU_INT16S)((CPU_INT16S)0 - (CPU_INT16S)(*p2_str_cmp_next));
      }
    } else {                                                    // If p2_str_cmp_next NULL, ...
      cmp_val = (CPU_INT16S)(*p1_str_cmp_next);                 // ... rtn pos p1_str_cmp_next val (see Note #2f).
    }
  }

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                           Str_CmpIgnoreCase()
 *
 * @brief    Determines if two strings are identical, ignoring case.
 *
 * @param    p1_str  Pointer to the first string. String buffer NOT modified.
 *
 * @param    p2_str  Pointer to the second string. String buffer NOT modified.
 *
 * @return   0, if strings are identical.
 *           Negative value, if 'p1_str' is less than 'p2_str'.
 *           Positive value, if 'p1_str' is greater than 'p2_str'.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strcasecmp() : DESCRIPTION' states that :
 *               "The strcasecmp() function shall compare [...] the string pointed to by 's1'
 *               ('p1_str') to the string pointed to by 's2' ('p2_str') [...] ignoring differences in
 *               case [and that] strcasecmp() [...] shall behave as if the strings had been converted
 *               to lowercase and then a byte comparison performed. [...] [T]he sign of a non-zero
 *               return value shall be determined by the sign of the difference between the values of
 *               the first pair of bytes [...] that differ in the strings being compared"
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strcasecmp() : RETURN VALUE' states that :
 *               "upon successful completion, strcasecmp() shall return an integer greater than, equal
 *               to, or less than 0".
 *
 * @note     (3) String comparison terminates when :
 *               - (a) BOTH string pointer(s) are passed as NULL pointers. Return 0.
 *               - (b) 'p1_str' passed a NULL pointer. Return negative value of character pointed to by
 *                     'p2_str'.
 *               - (c) 'p2_str' passed a NULL pointer. Return positive value of character pointed to by
 *                     'p1_str'.
 *               - (d) BOTH strings at some point point to NULL. Strings overlap with NULL address.
 *                     Strings identical up to but NOT beyond or including the NULL address. Return 0.
 *               - (e) 'p1_str_cmp_next' points to NULL; 'p1_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return negative
 *                     value of character pointed to by 'p2_str_cmp_next'.
 *               - (f) 'p2_str_cmp_next' points to NULL; 'p2_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return positive
 *                     value of character pointed to by 'p1_str_cmp_next'.
 *               - (g) Terminating NULL character found in both strings. Strings identical. Return 0.
 *               - (h) Non-matching characters found. Return signed-integer difference of the character
 *                     pointed to by 'p2_str' from the character pointed to by 'p1_str'.
 *
 * @note     (4) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_CHAR' native data type size MUST be 8-bit.
 *******************************************************************************************************/
CPU_INT16S Str_CmpIgnoreCase(const CPU_CHAR *p1_str,
                             const CPU_CHAR *p2_str)
{
  CPU_INT16S cmp_val;

  cmp_val = Str_CmpIgnoreCase_N(p1_str,
                                p2_str,
                                DEF_INT_CPU_U_MAX_VAL);

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                           Str_CmpIgnoreCase_N()
 *
 * @brief    Determines if two strings are identical for up to a maximum number of characters,
 *           ignoring case.
 *
 * @param    p1_str      Pointer to the first string. String buffer NOT modified.
 *
 * @param    p2_str      Pointer to the second string. String buffer NOT modified.
 *
 * @param    len_max     Maximum number of characters to compare.
 *
 * @return   0, if strings are identical.
 *           Negative value, if 'p1_str' is less than 'p2_str'.
 *           Positive value, if 'p1_str' is greater than 'p2_str'.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strncasecmp() : DESCRIPTION' states that :
 *               "The strncasecmp() function shall compare [...] the string pointed to by 's1'
 *               ('p1_str') to the string pointed to by 's2' ('p2_str') [...] ignoring differences in
 *               case [...] but not more than 'n' ('len_max') bytes of either string. [...]
 *               strncasecmp() shall behave as if the strings had been converted to lowercase and then
 *               a byte comparison performed. [...] [T]he sign of a non-zero return value shall be
 *               determined by the sign of the difference between the values of the first pair of
 *               bytes [...] that differ in the strings being compared".
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strncasecmp() : RETURN VALUE' states that :
 *               "upon successful completion, strncasecmp() shall return an integer greater than,
 *               equal to, or less than 0".
 *
 * @note     (3) String comparison terminates when :
 *               - (a) BOTH string pointer(s) are passed as NULL pointers. Return 0.
 *               - (b) 'p1_str' passed a NULL pointer. Return negative value of character pointed to by
 *                     'p2_str'.
 *               - (c) 'p2_str' passed a NULL pointer. Return positive value of character pointed to by
 *                     'p1_str'.
 *               - (d) BOTH strings at some point point to NULL. Strings overlap with NULL address.
 *                     Strings identical up to but NOT beyond or including the NULL address. Return 0.
 *               - (e) 'p1_str_cmp_next' points to NULL; 'p1_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return negative
 *                     value of character pointed to by 'p2_str_cmp_next'.
 *               - (f) 'p2_str_cmp_next' points to NULL; 'p2_str' overlaps with NULL address. Strings
 *                     compared up to but NOT beyond or including the NULL address. Return positive
 *                     value of character pointed to by 'p1_str_cmp_next'.
 *               - (g) Terminating NULL character found in both strings. Strings identical. Return 0.
 *               - (h) Non-matching characters found. Return signed-integer difference of the character
 *                     pointed to by 'p2_str' from the character pointed to by 'p1_str'.
 *               - (i) 'len_max' passed a zero length. Zero-length strings identical; 0 returned.
 *               - (j) First 'len_max' number of characters identical. Strings identical; 0 returned.
 *
 * @note     (4) Since 16-bit signed arithmetic is performed to calculate a non-identical comparison
 *               return value, 'CPU_CHAR' native data type size MUST be 8-bit.
 *******************************************************************************************************/
CPU_INT16S Str_CmpIgnoreCase_N(const CPU_CHAR *p1_str,
                               const CPU_CHAR *p2_str,
                               CPU_SIZE_T     len_max)
{
  const CPU_CHAR *p1_str_cmp;
  const CPU_CHAR *p2_str_cmp;
  const CPU_CHAR *p1_str_cmp_next;
  const CPU_CHAR *p2_str_cmp_next;
  CPU_CHAR       char_1;
  CPU_CHAR       char_2;
  CPU_INT16S     cmp_val;
  CPU_SIZE_T     cmp_len;

  if (len_max < 1) {                                            // If cmp len = 0, rtn 0 (see Note #3i).
    return (0);
  }

  if (p1_str == DEF_NULL) {
    if (p2_str == DEF_NULL) {
      return (0);                                               // If BOTH str ptrs NULL, rtn 0 (see Note #3a).
    }
    char_2 = ASCII_ToLower(*p2_str);
    cmp_val = (CPU_INT16S)((CPU_INT16S)0 - (CPU_INT16S)char_2);
    return (cmp_val);                                           // If p1_str NULL, rtn neg p2_str val (see Note #3b).
  }
  if (p2_str == DEF_NULL) {
    char_1 = ASCII_ToLower(*p1_str);
    cmp_val = (CPU_INT16S)char_1;
    return (cmp_val);                                           // If p2_str NULL, rtn pos p1_str val (see Note #3c).
  }

  p1_str_cmp = p1_str;
  p2_str_cmp = p2_str;
  p1_str_cmp_next = p1_str_cmp;
  p2_str_cmp_next = p2_str_cmp;
  p1_str_cmp_next++;
  p2_str_cmp_next++;
  char_1 = ASCII_ToLower(*p1_str_cmp);
  char_2 = ASCII_ToLower(*p2_str_cmp);
  cmp_len = 0u;

  while ((char_1 == char_2)                                     // Cmp strs until non-matching chars (see Note #3h) ...
         && (*p1_str_cmp != (CPU_CHAR)'\0')                     // ... or NULL chars (see Note #3g) ...
         && (p1_str_cmp_next != DEF_NULL)                       // ... or NULL ptr(s) found (see Note #3d).
         && (p2_str_cmp_next != DEF_NULL)
         && (cmp_len < (CPU_SIZE_T)len_max)) {                  // ... or max nbr chars cmp'd (see Note #3j).
    p1_str_cmp++;
    p2_str_cmp++;
    p1_str_cmp_next++;
    p2_str_cmp_next++;
    cmp_len++;
    char_1 = ASCII_ToLower(*p1_str_cmp);
    char_2 = ASCII_ToLower(*p2_str_cmp);
  }

  if (cmp_len == len_max) {                                     // If strs identical for max len nbr of chars, ...
    return (0);                                                 // ... rtn 0 (see Note #3j).
  }

  if (char_1 != char_2) {                                       // If strs NOT identical, ...
                                                                // ... calc & rtn char diff (see Note #3h).
    cmp_val = (CPU_INT16S)((CPU_INT16S)char_1 - (CPU_INT16S)char_2);
  } else if (char_1 == (CPU_CHAR)'\0') {                        // If NULL char(s) found, ...
    cmp_val = (CPU_INT16S)0;                                    // ... strs identical; rtn 0 (see Note #3g).
  } else {
    if (p1_str_cmp_next == DEF_NULL) {
      if (p2_str_cmp_next == DEF_NULL) {                        // If BOTH next str ptrs NULL, ...
        cmp_val = (CPU_INT16S)0;                                // ... rtn 0 (see Note #3d).
      } else {                                                  // If p1_str_cmp_next NULL, ...
        char_2 = ASCII_ToLower(*p2_str_cmp_next);
        //                                                         ... rtn neg p2_str_cmp_next val (see Note #3e).
        cmp_val = (CPU_INT16S)((CPU_INT16S)0 - (CPU_INT16S)char_2);
      }
    } else {                                                    // If p2_str_cmp_next NULL, ...
      char_1 = ASCII_ToLower(*p1_str_cmp_next);
      cmp_val = (CPU_INT16S)char_1;                             // ... rtn pos p1_str_cmp_next val (see Note #3f).
    }
  }

  return (cmp_val);
}

/****************************************************************************************************//**
 *                                               Str_Char()
 *
 * @brief    Searches the string for first occurrence of specific character.
 *
 * @param    p_str       Pointer to the string. String buffer NOT modified.
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to the first occurrence of search character in string, if any occur.
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strchr() : DESCRIPTION' states that :
 *               "The strchr() function shall locate the first occurrence of 'c' ('srch_char') [...]
 *               in the string pointed to by 's' ('p_str'). [...] The terminating null byte is
 *               considered to be part of the string."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strchr() : RETURN VALUE' states that :
 *               "upon completion, strchr() shall return [...] a pointer to the byte, [...] or a null
 *               pointer if the byte was not found."
 *               Although NO strchr() specification states to return NULL for any other reason(s),
 *               NULL is also returned for any error(s).
 *
 * @note     (3) String search terminates when :
 *               - (a) String pointer passed a NULL pointer. No string search performed. NULL pointer is
 *                     returned.
 *               - (b) String pointer points to NULL. String overlaps with NULL address. NULL pointer is
 *                     returned.
 *               - (c) String's terminating NULL character found. Search character NOT found in search
 *                     string. NULL pointer is returned. This is applicable even if search character is
 *                     the terminating NULL character.
 *               - (d) Search character found. Return pointer to first occurrence of search character in
 *                     search string.
 *******************************************************************************************************/
CPU_CHAR *Str_Char(const CPU_CHAR *p_str,
                   CPU_CHAR       srch_char)
{
  CPU_CHAR *p_str_rtn;

  p_str_rtn = Str_Char_N(p_str,
                         DEF_INT_CPU_U_MAX_VAL,
                         srch_char);

  return (p_str_rtn);
}

/****************************************************************************************************//**
 *                                               Str_Char_N()
 *
 * @brief    Searches the string for first occurrence of specific character, up to a maximum number of
 *           characters.
 *
 * @param    p_str       Pointer to the string. String buffer NOT modified.
 *
 * @param    len_max     Maximum number of characters to search. 'len_max' number of characters MAY
 *                       include terminating NULL character
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to the first occurrence of search character in string, if any occur.
 *           Pointer to the NULL, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strchr() : DESCRIPTION' states that :
 *               "The strchr() function shall locate the first occurrence of 'c' ('srch_char') [...]
 *               in the string pointed to by 's' ('p_str'). [...] The terminating null byte is
 *               considered to be part of the string."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strchr() : RETURN VALUE' states that :
 *               "upon completion, strchr() shall return [...] a pointer to the byte, [...] or a null
 *               pointer if the byte was not found."
 *               Although NO strchr() specification states to return NULL for any other reason(s),
 *               NULL is also returned for any error(s).
 *
 * @note     (3) String search terminates when :
 *               - (a) String pointer passed a NULL pointer. No string search performed. NULL pointer is
 *                     returned.
 *               - (b) String pointer points to NULL. String overlaps with NULL address. NULL pointer is
 *                     returned.
 *               - (c) String's terminating NULL character found. Search character NOT found in search
 *                     string. NULL pointer is returned. This is applicable even if search character is
 *                     the terminating NULL character.
 *               - (d) Search character found. Return pointer to first occurrence of search character in
 *                     search string.
 *               - (e) 'len_max' number of characters searched. Search character NOT found in search
 *                      string within first 'len_max' number of characters. NULL pointer is returned.
 *
 * @note     (4) Ideally, the 'len_max' argument would be the last argument in this function's
 *               argument list for consistency with all other custom string library functions.
 *               However, the 'len_max' argument is sequentially ordered as the second argument to
 *               comply with most standard library's strnchr() argument list.
 *******************************************************************************************************/
CPU_CHAR *Str_Char_N(const CPU_CHAR *p_str,
                     CPU_SIZE_T     len_max,
                     CPU_CHAR       srch_char)
{
  const CPU_CHAR *p_str_char;
  CPU_SIZE_T     len_srch;

  if (p_str == DEF_NULL) {                                      // Rtn NULL if srch str ptr NULL (see Note #3a).
    return (DEF_NULL);
  }

  if (len_max < 1) {                                            // Rtn NULL if srch len = 0 (see Note #3e).
    return (DEF_NULL);
  }

  p_str_char = p_str;
  len_srch = 0u;

  while ((p_str_char != DEF_NULL)                               // Srch str until NULL ptr (see Note #3b) ...
         && (*p_str_char != (CPU_CHAR)'\0')                     // ... or NULL char (see Note #3c)  ...
         && (*p_str_char != (CPU_CHAR)srch_char)                // ... or srch char found (see Note #3d); ...
         && (len_srch < (CPU_SIZE_T)len_max)) {                 // ... or max nbr chars srch'd (see Note #3e).
    p_str_char++;
    len_srch++;
  }

#if 0
  if (p_str_char == DEF_NULL) {                                 // Rtn NULL if NULL ptr found (see Note #3b).
    return (DEF_NULL);
  }
#endif

  if (len_srch >= len_max) {                                    // Rtn NULL if srch char NOT found ...
    return (DEF_NULL);                                          // ... within max nbr of chars (see Note #3e).
  }

  if (*p_str_char != srch_char) {                               // Rtn NULL if srch char NOT found (see Note #3c).
    return (DEF_NULL);
  }

  return ((CPU_CHAR *)p_str_char);                              // Else rtn ptr to found srch char (see Note #3d).
}

/****************************************************************************************************//**
 *                                               Str_Char_Last()
 *
 * @brief    Searches the string for last occurrence of specific character.
 *
 * @param    p_str       Pointer to the string. String buffer NOT modified.
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to the last occurrence of search character in string, if any.
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strrchr() : DESCRIPTION' states that :
 *               "The strrchr() function shall locate the last occurrence of 'c' ('srch_char') [...]
 *               in the string pointed to by 's' ('p_str'). [...] The terminating null byte is
 *               considered to be part of the string."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strrchr() : RETURN VALUE' states that :
 *               "upon successful completion, strrchr() shall return [...] a pointer to the byte [...]
 *               or a null pointer if 'c' ('srch_char') does not occur in the string."
 *               Although NO strrchr() specification states to return NULL for any other reason(s),
 *               NULL is also returned for any error(s).
 *
 * @note     (3) String search terminates when :
 *               - (a) String pointer passed a NULL pointer. No string search performed. NULL pointer is
 *                     returned.
 *               - (b) String pointer points to NULL. String overlaps with NULL address. NULL pointer is
 *                     returned.
 *               - (c) String searched from end to beginning. Search character NOT found in search
 *                     string. NULL pointer is returned. Applicable even if search character is the
 *                     terminating NULL character.
 *               - (d) Search character found. Return pointer to last occurrence of search character in
 *                     search string.
 *******************************************************************************************************/
CPU_CHAR *Str_Char_Last(const CPU_CHAR *p_str,
                        CPU_CHAR       srch_char)
{
  CPU_CHAR *p_str_rtn;

  p_str_rtn = Str_Char_Last_N(p_str,
                              DEF_INT_CPU_U_MAX_VAL,
                              srch_char);

  return (p_str_rtn);
}

/****************************************************************************************************//**
 *                                               Str_Char_Last_N()
 *
 * @brief    Searches the string for last occurrence of specific character, up to a maximum number of
 *           characters.
 *
 * @param    p_str       Pointer to the string. String buffer NOT modified.
 *
 * @param    len_max     Maximum number of characters to search. 'len_max' number of characters MAY
 *                       include terminating NULL character.
 *
 * @param    srch_char   Search character.
 *
 * @return   Pointer to the last occurrence of search character in string, if any.
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strrchr() : DESCRIPTION' states that :
 *               "The strrchr() function shall locate the last occurrence of 'c' ('srch_char') [...]
 *               in the string pointed to by 's' ('p_str'). [...] The terminating null byte is
 *               considered to be part of the string."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strrchr() : RETURN VALUE' states that :
 *               "upon successful completion, strrchr() shall return [...] a pointer to the byte [...]
 *               or a null pointer if 'c' ('srch_char') does not occur in the string."
 *               AlthougNO strrchr() specification states to return NULL for any other reason(s),
 *               NULL is also returned for any error(s).
 *
 * @note     (3) String search terminates when :
 *               - (a) String pointer passed a NULL pointer. No string search performed. NULL pointer is
 *                     returned.
 *               - (b) String pointer points to NULL. String overlaps with NULL address. NULL pointer is
 *                     returned.
 *               - (c) String searched from end to beginning. Search character NOT found in search
 *                     string. NULL pointer is returned. Applicable even if search character is the
 *                     terminating NULL character.
 *               - (d) Search character found. Return pointer to last occurrence of search character in
 *                     search string.
 *               - (e) 'len_max' number of characters searched. Search character NOT found in search
 *                     string within last 'len_max' number of characters. NULL pointer is returned.
 *
 * @note     (4) Ideally, the 'len_max' argument would be the last argument in this function's
 *               argument list for consistency with all other custom string library functions.
 *               However, the 'len_max' argument is sequentially ordered as the second argument to
 *               comply with most standard library's strnrchr() argument list.
 *******************************************************************************************************/
CPU_CHAR *Str_Char_Last_N(const CPU_CHAR *p_str,
                          CPU_SIZE_T     len_max,
                          CPU_CHAR       srch_char)
{
  const CPU_CHAR *p_str_char;
  CPU_SIZE_T     str_len_max;
  CPU_SIZE_T     str_len;

  if (p_str == DEF_NULL) {                                      // Rtn NULL if srch str ptr NULL (see Note #3a).
    return (DEF_NULL);
  }

  if (len_max < 1) {                                            // Rtn NULL if srch len = 0 (see Note #3e).
    return (DEF_NULL);
  }

  p_str_char = p_str;
  str_len_max = len_max - sizeof("");                           // Str len adj'd for NULL char len.
  str_len = Str_Len_N(p_str_char, str_len_max);
  p_str_char += str_len;

#if 0
  if (p_str_char == DEF_NULL) {                                 // Rtn NULL if NULL ptr found (see Note #3b).
    return (DEF_NULL);
  }
#endif

  while ((p_str_char != p_str)                                  // Srch str from end until beginning (see Note #3c) ...
         && (*p_str_char != srch_char)) {                       // ... until srch char found (see Note #3d).
    p_str_char--;
  }

  if (*p_str_char != srch_char) {                               // Rtn NULL if srch char NOT found (see Note #3c).
    return (DEF_NULL);
  }

  return ((CPU_CHAR *)p_str_char);                              // Else rtn ptr to found srch char (see Note #3d).
}

/****************************************************************************************************//**
 *                                           Str_Char_Replace()
 *
 * @brief    Searches the string for specific character and replace it by another specific character.
 *
 * @param    p_str           Pointer to the string. This string will be modified.
 *
 * @param    char_srch       Search character.
 *
 * @param    char_replace    Replace character.
 *
 * @return   Pointer to the string, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) String search terminates when :
 *               - (a) String pointer passed a NULL pointer. No string search performed. NULL pointer is
 *                     returned.
 *               - (b) String pointer points to NULL. String overlaps with NULL address. NULL pointer is
 *                     returned.
 *               - (c) String's terminating NULL character found. Search character NOT found in search
 *                     string. NULL pointer is returned. Applicable even if search character is the
 *                     terminating NULL character.
 *               - (d) Search character found. Replace character found by the specified character.
 *******************************************************************************************************/
CPU_CHAR *Str_Char_Replace(CPU_CHAR *p_str,
                           CPU_CHAR char_srch,
                           CPU_CHAR char_replace)
{
  CPU_CHAR *p_str_rtn;

  p_str_rtn = Str_Char_Replace_N(p_str,
                                 char_srch,
                                 char_replace,
                                 DEF_INT_CPU_U_MAX_VAL);

  return (p_str_rtn);
}

/****************************************************************************************************//**
 *                                           Str_Char_Replace_N()
 *
 * @brief    Searches the string for specific character and replace it by another specific character,
 *           up to a maximum number of characters.
 *
 * @param    p_str           Pointer to the string. This string will be modified.
 *
 * @param    char_srch       Search character.
 *
 * @param    char_replace    Replace character.
 *
 * @param    len_max         Maximum number of characters to search. 'len_max' number of characters
 *                           MAY include terminating NULL character.
 *
 * @return      Pointer to the string, if NO error(s).
 *              Pointer to NULL, otherwise.
 *
 * @note     (1) String search terminates when :
 *               - (a) String pointer passed a NULL pointer. No string search performed. NULL pointer is
 *                     returned.
 *               - (b) String pointer points to NULL. String overlaps with NULL address. NULL pointer is
 *                     returned.
 *               - (c) String's terminating NULL character found. Search character NOT found in search
 *                     string. NULL pointer is returned. Applicable even if search character is the
 *                     terminating NULL character.
 *               - (d) Search character found. Replace character found by the specified character.
 *               - (e) 'len_max' number of characters searched. Search character NOT found in search
 *                     string within first 'len_max' number of characters. NULL pointer is returned.
 *******************************************************************************************************/
CPU_CHAR *Str_Char_Replace_N(CPU_CHAR   *p_str,
                             CPU_CHAR   char_srch,
                             CPU_CHAR   char_replace,
                             CPU_SIZE_T len_max)
{
  CPU_CHAR   *p_str_char;
  CPU_SIZE_T len;

  if (p_str == DEF_NULL) {                                      // Rtn NULL if srch str ptr NULL (see Note #2a).
    return (DEF_NULL);
  }

  if (len_max < 1) {                                            // Rtn NULL if srch len = 0 (see Note #2e).
    return (DEF_NULL);
  }

  p_str_char = p_str;
  len = len_max;

  while ((p_str_char != DEF_NULL)                               // Srch str until NULL ptr (see Note #2b) ...
         && (*p_str_char != ASCII_CHAR_NULL)                    // ... or NULL char (see Note #2c) ...
         && (len > 0)) {                                        // ... or max nbr chars srch'd (see Note #2e).
    if (*p_str_char == char_srch) {
      *p_str_char = char_replace;                               // Replace char if srch char is found.
    }

    p_str_char++;
    len--;
  }

  return (p_str);
}

/****************************************************************************************************//**
 *                                                   Str_Str()
 *
 * @brief    Searches a string for the first occurrence of a specific search string.
 *
 * @param    p_str       Pointer to a string. String buffer is NOT modified.
 *
 * @param    p_str_srch  Pointer to the search string. String buffer is NOT modified.
 *
 * @return   Pointer to the first occurrence of search string in the string, if any.
 *           Pointer to the string, if search string has a zero length.
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strstr() : DESCRIPTION' states that :
 *               "The strstr() function shall locate the first occurrence in the string pointed to by
 *               's1' ('p_str') of the sequence of bytes [...] in the string pointed to by 's2'
 *               ('p_str_srch') [...] (excluding the terminating null byte)."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strstr() : RETURN VALUE' states that :
 *               "Upon successful completion, strstr() shall return [...] a pointer to the located
 *               string [...] or a null pointer if the string is not found. [...] If 's2'
 *               ('p_str_srch') points to a string with zero length, the function shall return 's1'
 *               ('p_str')."
 *               Although NO strstr() specification states to return NULL for any other reason(s),
 *               NULL is also returned for any error(s).
 *
 * @note     (3) String search terminates when :
 *               - (a) String pointer(s) are passed as NULL pointers. No string search performed. NULL
 *                     pointer is returned.
 *               - (b) String pointer(s) point to NULL. String buffer(s) overlap with NULL address. NULL
 *                     pointer is returned.
 *               - (c) for the first occurrence length equal to zero. No string search performed. String
 *                     pointer returned.
 *               - (d) Search string length greater than string length. No string search performed. NULL
 *                     pointer returned.
 *               - (e) Entire string has been searched. Search string not found. NULL pointer is
 *                     returned.
 *               - (f) Search string found. Return pointer to first occurrence of search string in
 *                     string.
 *******************************************************************************************************/
CPU_CHAR *Str_Str(const CPU_CHAR *p_str,
                  const CPU_CHAR *p_str_srch)
{
  CPU_CHAR *p_str_rtn;

  p_str_rtn = Str_Str_N(p_str,
                        p_str_srch,
                        DEF_INT_CPU_U_MAX_VAL);

  return (p_str_rtn);
}

/****************************************************************************************************//**
 *                                               Str_Str_N()
 *
 * @brief    Searches the string for the first occurrence of a specific search string, up to a maximum
 *           number of characters.
 *
 * @param    p_str       Pointer to a string. String buffer is NOT modified.
 *
 * @param    p_str_srch  Pointer to the search string. String buffer is NOT modified.
 *
 * @param    len_max     Maximum number of characters to search. 'len_max' number of characters does
 *                       NOT include terminating NULL character.
 *
 * @return   Pointer to the first occurrence of search string in the string, if any.
 *           Pointer to the string, if search string has a zero length.
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strstr() : DESCRIPTION' states that :
 *               "The strstr() function shall locate the first occurrence  in the string pointed to by
 *               's1' ('p_str') of the sequence of bytes [...] in the string pointed to by 's2'
 *               ('p_str_srch') [...] (excluding the terminating null byte)."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strstr() : RETURN VALUE' states that :
 *               "Upon successful completion, strstr() shall return [...] a pointer to the located
 *               string [...] or a null pointer if the string is not found. [...] If 's2'
 *               ('p_str_srch') points to a string with zero length, the function shall return 's1'
 *               ('p_str')."
 *               Although NO strstr() specification states to return NULL for any other reason(s),
 *               NULL is also returned for any error(s).
 *
 * @note     (3) String search terminates when :
 *               - (a) String pointer(s) are passed as NULL pointers. No string search performed. NULL
 *                     pointer is returned.
 *               - (b) String pointer(s) point to NULL. String buffer(s) overlap with NULL address. NULL
 *                     pointer is returned.
 *               - (c) for the first occurrence length equal to zero. No string search performed. String
 *                     pointer returned.
 *               - (d) Search string length greater than string length. No string search performed. NULL
 *                     pointer returned.
 *               - (e) Entire string has been searched. Search string not found. NULL pointer is
 *                     returned.  The maximum size of the search is defined as the subtraction of the
 *                     search string length from the string length.
 *               - (f) Search string found. Return pointer to first occurrence of search string in
 *                     string. Search string found via Str_Cmp_N().
 *               - (g) 'len_max' number of characters searched.
 *******************************************************************************************************/
CPU_CHAR *Str_Str_N(const CPU_CHAR *p_str,
                    const CPU_CHAR *p_str_srch,
                    CPU_SIZE_T     len_max)
{
#if 0
  const CPU_CHAR *p_str_str;
#endif
  CPU_SIZE_T     str_len;
  CPU_SIZE_T     str_len_srch;
  CPU_SIZE_T     len_max_srch;
  CPU_SIZE_T     srch_len;
  CPU_SIZE_T     srch_ix;
  CPU_BOOLEAN    srch_done;
  CPU_INT16S     srch_cmp;
  const CPU_CHAR *p_str_srch_ix;

  //                                                               Rtn NULL if str ptr(s) NULL (see Note #3a).
  if (p_str == DEF_NULL) {
    return (DEF_NULL);
  }
  if (p_str_srch == DEF_NULL) {
    return (DEF_NULL);
  }

  if (len_max < 1) {                                            // Rtn NULL if srch len = 0 (see Note #3g).
    return (DEF_NULL);
  }

  //                                                               Lim max srch str len (to chk > str len).
  len_max_srch = (len_max < DEF_INT_CPU_U_MAX_VAL)
                 ? (len_max + 1u) : DEF_INT_CPU_U_MAX_VAL;

  str_len = Str_Len_N(p_str, len_max);
  str_len_srch = Str_Len_N(p_str_srch, len_max_srch);
  if (str_len_srch < 1) {                                       // Rtn ptr to str if srch str len = 0 (see Note #2).
    return ((CPU_CHAR *)p_str);
  }
  if (str_len_srch > str_len) {                                 // Rtn NULL if srch str len > str len (see Note #3d).
    return (DEF_NULL);
  }

#if 0
  //                                                               Rtn NULL if NULL ptr found (see Note #3b).
  p_str_str = p_str + str_len;
  if (p_str_str == DEF_NULL) {
    return (DEF_NULL);
  }
  p_str_str = p_str_srch + str_len_srch;
  if (p_str_str == DEF_NULL) {
    return (DEF_NULL);
  }
#endif

  srch_len = str_len - str_len_srch;                            // Calc srch len (see Note #3e).
  srch_ix = 0u;

  do {
    p_str_srch_ix = (const CPU_CHAR *)(p_str + srch_ix);
    srch_cmp = Str_Cmp_N(p_str_srch_ix, p_str_srch, str_len_srch);
    srch_done = (srch_cmp == 0) ? DEF_YES : DEF_NO;
    srch_ix++;
  } while ((srch_done == DEF_NO) && (srch_ix <= srch_len));

  if (srch_cmp != 0) {                                          // Rtn NULL if srch str NOT found (see Note #3e).
    return (DEF_NULL);
  }

  return ((CPU_CHAR *)p_str_srch_ix);                           // Else rtn ptr to found srch str (see Note #3f).
}

/****************************************************************************************************//**
 *                                           Str_FmtNbr_Int32U()
 *
 * @brief    Formats a 32-bit unsigned integer into a multi-digit character string.
 *
 * @param    nbr         Number to format.
 *
 * @param    nbr_dig     Number of digits to format.
 *                       The following may be used to specify the number of digits to format :
 *                           - DEF_INT_32U_NBR_DIG_MIN     Minimum number of 32-bit unsigned digits
 *                           - DEF_INT_32U_NBR_DIG_MAX     Maximum number of 32-bit unsigned digits
 *
 * @param    nbr_base    Base of number to format. The number's base MUST be between 2 and 36,
 *                       inclusively.
 *                       The following may be used to specify the number base :
 *                           - DEF_NBR_BASE_BIN            Base  2
 *                           - DEF_NBR_BASE_OCT            Base  8
 *                           - DEF_NBR_BASE_DEC            Base 10
 *                           - DEF_NBR_BASE_HEX            Base 16
 *
 * @param    lead_char   Prepend leading character. Leading character option prepends leading
 *                       characters prior to the first non-zero digit. Leading character MUST be a
 *                       printable ASCII character. Leading character MUST NOT be a number base digit,
 *                       with the exception of '0'.
 *                           - '\0'                    Do NOT prepend leading character to the string.
 *                           - Printable character     Prepend leading character to the string.
 *                           - Unprintable character   Format invalid string.
 *
 * @param    lower_case  Format alphabetic characters (if any) in lower case :
 *                           - DEF_NO          Format alphabetic characters in upper case.
 *                           - DEF_YES         Format alphabetic characters in lower case.
 *
 * @param    nul         Append terminating NULL-character. NULL-character terminate option DISABLED
 *                       prevents overwriting previous character array formatting. Unless 'p_str'
 *                       character array is pre-/post-terminated, NULL-character terminate option
 *                       DISABLED will cause character string run-on.
 *                           - DEF_NO          Do NOT append terminating NULL-character to the string.
 *                           - DEF_YES         Append terminating NULL-character to the string.
 *
 * @param    p_str       Pointer to the character array to return formatted number string. Format
 *                       buffer size is NOT validated; buffer overruns MUST be prevented by caller.
 *                       To prevent character buffer overrun, character array size MUST be >=
 *                       ('nbr_dig' + 1 'NUL' terminator) characters.
 *
 * @return   Pointer to the formatted string, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) See Str_FmtNbr_Int32()'s notes.
 *******************************************************************************************************/
CPU_CHAR *Str_FmtNbr_Int32U(CPU_INT32U  nbr,
                            CPU_INT08U  nbr_dig,
                            CPU_INT08U  nbr_base,
                            CPU_CHAR    lead_char,
                            CPU_BOOLEAN lower_case,
                            CPU_BOOLEAN nul,
                            CPU_CHAR    *p_str)
{
  CPU_CHAR *p_str_fmt;

  p_str_fmt = Str_FmtNbr_Int32(nbr,                             // Fmt unsigned int into str.
                               nbr_dig,
                               nbr_base,
                               DEF_NO,
                               lead_char,
                               lower_case,
                               nul,
                               p_str);

  return (p_str_fmt);
}

/****************************************************************************************************//**
 *                                           Str_FmtNbr_Int32S()
 *
 * @brief    Formats a 32-bit signed integer into a multi-digit character string.
 *
 * @param    nbr         Number to format.
 *
 * @param    nbr_dig     Number of digits to format.
 *                       The following may be used to specify the number of digits to format :
 *                           - DEF_INT_32S_NBR_DIG_MIN + 1     Minimum number of 32-bit signed digits
 *                           - DEF_INT_32S_NBR_DIG_MAX + 1     Maximum number of 32-bit signed digits
 *                       (plus 1 digit for possible negative sign)
 *
 * @param    nbr_base    Base of number to format. The number's base MUST be between 2 and 36,
 *                       inclusively.
 *                       The following may be used to specify the number base :
 *                           - DEF_NBR_BASE_BIN            Base  2
 *                           - DEF_NBR_BASE_OCT            Base  8
 *                           - DEF_NBR_BASE_DEC            Base 10
 *                           - DEF_NBR_BASE_HEX            Base 16
 *
 * @param    lead_char   Prepend leading character. Leading character option prepends leading
 *                       characters prior to the first non-zero digit. Leading character MUST be a
 *                       printable ASCII character. Leading character MUST NOT be a number base digit,
 *                       with the exception of '0'.
 *                           -  '\0'                    Do NOT prepend leading character to the string.
 *                           -  Printable character     Prepend leading character to the string.
 *                           -  Unprintable character   Format invalid string.
 *
 * @param    lower_case  Format alphabetic characters (if any) in lower case :
 *                           - DEF_NO          Format alphabetic characters in upper case.
 *                           - DEF_YES         Format alphabetic characters in lower case.
 *
 * @param    nul         Append terminating NULL-character. NULL-character terminate option DISABLED
 *                       prevents overwriting previous character array formatting. Unless 'p_str'
 *                       character array is pre-/post-terminated, NULL-character terminate option
 *                       DISABLED will cause character string run-on.
 *                           - DEF_NO          Do NOT append terminating NULL-character to the string.
 *                           - DEF_YES         Append terminating NULL-character to the string.
 *
 * @param    p_str       Pointer to the character array to return formatted number string. Format
 *                       buffer size is NOT validated; buffer overruns MUST be prevented by caller.
 *                       To prevent character buffer overrun, character array size MUST be >=
 *                       ('nbr_dig' + 1 negative sign + 1 'NUL' terminator) characters.
 *
 * @return   Pointer to the formatted string, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) See Str_FmtNbr_Int32()'s notes.
 *******************************************************************************************************/
CPU_CHAR *Str_FmtNbr_Int32S(CPU_INT32S  nbr,
                            CPU_INT08U  nbr_dig,
                            CPU_INT08U  nbr_base,
                            CPU_CHAR    lead_char,
                            CPU_BOOLEAN lower_case,
                            CPU_BOOLEAN nul,
                            CPU_CHAR    *p_str)
{
  CPU_CHAR    *p_str_fmt;
  CPU_INT32S  nbr_fmt;
  CPU_BOOLEAN nbr_neg;

  if (nbr < 0) {                                                // If nbr neg, ...
    nbr_fmt = -nbr;                                             // ... negate nbr.
    nbr_neg = DEF_YES;
  } else {
    nbr_fmt = nbr;
    nbr_neg = DEF_NO;
  }

  p_str_fmt = Str_FmtNbr_Int32((CPU_INT32U)nbr_fmt,             // Fmt signed int into str.
                               nbr_dig,
                               nbr_base,
                               nbr_neg,
                               lead_char,
                               lower_case,
                               nul,
                               p_str);

  return (p_str_fmt);
}

/****************************************************************************************************//**
 *                                               Str_FmtNbr_32()
 *
 * @brief    Formats a number into a multi-digit character string.
 *
 * @param    nbr         Number to format.
 *
 * @param    nbr_dig     Number of decimal digits to format (see Note #2).
 *
 * @param    nbr_dp      Number of decimal point digits to format.
 *
 * @param    lead_char   Prepend leading character. Leading character option prepends leading
 *                       characters prior to the first non-zero digit. Leading character MUST be a
 *                       printable ASCII character. Leading character MUST NOT be a number base digit,
 *                       with the exception of '0'.
 *                           - '\0'                    Do NOT prepend leading character to the string.
 *                           - Printable character     Prepend leading character to the string.
 *                           - Unprintable character   Format invalid string.
 *
 * @param    nul         Append terminating NULL-character. NULL-character terminate option DISABLED
 *                       prevents overwriting previous character array formatting. Unless 'p_str'
 *                       character array is pre-/post-terminated, NULL-character terminate option
 *                       DISABLED will cause character string run-on.
 *                           - DEF_NO          Do NOT append terminating NULL-character to the string.
 *                           - DEF_YES         Append terminating NULL-character to the string.
 *
 * @param    p_str       Pointer to the character array to return formatted number string. Format
 *                       buffer size is NOT validated; buffer overruns MUST be prevented by caller.
 *                       To prevent character buffer overrun, character array size MUST be >=
 *                       ('nbr_dig' + 'nbr_dp' + 1 negative sign + 1 decimal point + 1 'NUL'
 *                       terminator) characters.
 *
 * @return   Pointer to the formatted string, if NO error(s)..
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) The maximum accuracy for 32-bit floating-point numbers :
 *               @verbatim
 *                           Maximum Accuracy            log [Internal-Base ^ (Number-Internal-Base-Digits)]
 *                       32-bit Floating-point Number  =  -----------------------------------------------------
 *                                                                       log [External-Base]
 *
 *                                                       log [2 ^ 24]
 *                                                   =  --------------
 *                                                           log [10]
 *
 *                                                   <  7.225  Base-10 Digits
 *
 *                           where
 *                                   Internal-Base                   Internal number base of floating-
 *                                                                       point numbers (i.e.  2)
 *                                   External-Base                   External number base of floating-
 *                                                                       point numbers (i.e. 10)
 *                                   Number-Internal-Base-Digits     Number of internal number base
 *                                                                       significant digits (i.e. 24)
 *               @endverbatim
 *               Some CPUs' &/or compilers' floating-point implementations MAY further reduce the
 *               maximum accuracy.
 *
 * @note     (2) See usage examples below.
 *           - (a) If the total number of digits to format ('nbr_dig + nbr_dp') is zero; then NO
 *                   formatting is performed except possible NULL-termination of the string.
 *                       Example :
 *           @verbatim
 *                           nbr     = -23456.789
 *                           nbr_dig =  0
 *                           nbr_dp  =  0
 *
 *                           p_str   = ""                        See Note #4a
 *           @endverbatim
 *           - (b) If the number of digits to format ('nbr_dig') is less than the number of
 *                   significant integer digits of the number to format ('nbr'); then an invalid
 *                   string is formatted instead of truncating any significant integer digits.
 *                       Example :
 *           @verbatim
 *                           nbr     = 23456.789
 *                           nbr_dig = 3
 *                           nbr_dp  = 2
 *
 *                           p_str   = "??????"                  See Note #4d
 *           @endverbatim
 *           - (c) If the number to format ('nbr') is negative but the number of digits to format
 *                   ('nbr_dig') is equal to the number of significant integer digits of the number to
 *                   format ('nbr'); then an invalid string is formatted instead of truncating the
 *                   negative sign.
 *                       Example :
 *           @verbatim
 *                           nbr     = -23456.789
 *                           nbr_dig =  5
 *                           nbr_dp  =  2
 *
 *                           p_str   = "????????"                See Note #4d
 *           @endverbatim
 *           - (d) If the number to format ('nbr') is negative but the number of significant integer
 *                   digits is zero, and the number of digits to format ('nbr_dig') is one but the
 *                   number of decimal point digits to format ('nbr_dp') is zero; then an invalid
 *                   string is formatted instead of truncating the negative sign.
 *                       Example :
 *           @verbatim
 *                           nbr     = -0.7895
 *                           nbr_dig =  1
 *                           nbr_dp  =  0
 *
 *                           p_str   = "?"                       See Note #4d
 *           @endverbatim
 *           - (e) If the number to format ('nbr') is negative but the number of significant integer
 *                   digits is zero, and the number of digits to format ('nbr_dig') is zero but the
 *                   number of decimal point digits to format ('nbr_dp') is non-zero; then the
 *                   negative sign immediately prefixes the decimal point -- with NO decimal digits
 *                   formatted, NOT even a single decimal digit of '0'.
 *                       Example :
 *           @verbatim
 *                           nbr     = -0.7895
 *                           nbr_dig =  0
 *                           nbr_dp  =  2
 *
 *                           p_str   = "-.78"
 *           @endverbatim
 *           - (f) If the number to format ('nbr') is positive but the number of significant integer
 *                   digits is zero, and the number of digits to format ('nbr_dig') is zero but the
 *                   number of decimal point digits to format ('nbr_dp') is non-zero; then a single
 *                   decimal digit of '0' prefixes the decimal point.
 *                   This '0' digit is used whenever a negative sign is not formatted so that the
 *                   formatted string's decimal point is not floating, but fixed in the string as the
 *                   second character.
 *                       Example :
 *           @verbatim
 *                           nbr     =  0.7895
 *                           nbr_dig =  0
 *                           nbr_dp  =  2
 *
 *                           p_str   = "0.78"
 *           @endverbatim
 *           - (g) If the total number of digits to format ('nbr_dig + nbr_dp') is greater than :
 *                   the maximum accuracy of the CPU's &/or compiler's 32-bit floating-point numbers,
 *                   digits following all significantly-accurate digits of the number to format
 *                   ('nbr') will be inaccurate. The configured maximum accuracy
 *                   ('LIB_STR_CFG_FP_MAX_NBR_DIG_SIG'), all digits or decimal places following all
 *                   significantly-accurate digits of the number to format ('nbr') will be replaced &
 *                   formatted with zeros ('0').
 *                       Example :
 *           @verbatim
 *                           nbr                            = 123456789.012345
 *                           nbr_dig                        = 9
 *                           nbr_dp                         = 6
 *                           LIB_STR_CFG_FP_MAX_NBR_DIG_SIG = 7
 *
 *                           p_str                          = "123456700.000000"
 *           @endverbatim
 *                   Therefore, one or more least-significant digit(s) of the number to format ('nbr')
 *                   MAY be rounded & not necessarily truncated due to the inaccuracy of the CPU's
 *                   &/or compiler's floating-point implementation.
 *
 * @note     (3) String format terminates when :
 *               - (a) Format string pointer is passed a NULL pointer. No string formatted; NULL pointer
 *                     is returned.
 *               - (b) Total number of digits to format ('nbr_dig + nbr_dp') is zero. NULL string
 *                     formatted. NULL pointer is returned.
 *               - (c) Number of digits to format ('nbr_dig') is less than number of significant integer
 *                     digits of the number to format ('nbr'), including possible negative sign. Invalid
 *                     string formatted. NULL pointer is returned.
 *               - (d) Lead character is NOT a valid, printable character. Invalid string formatted.
 *                     NULL pointer is returned.
 *               - (e) Number successfully formatted into character string array.
 *
 * @note     (4) For any unsuccessful string format or error(s), an invalid string of question marks
 *               ('?') will be formatted, where the number of question marks is determined by the
 *               number of digits ('nbr_dig') & number of decimal point digits ('nbr_dp') to format :
 *               @verbatim
 *                                       {  (a)    0 (NULL string)          ,  if 'nbr_dig' = 0  AND
 *                                       {                                        'nbr_dp'  = 0
 *                                       {
 *                                       {  (b)   'nbr_dig'                 ,  if 'nbr_dig' > 0  AND
 *                                       {                                        'nbr_dp'  = 0
 *                  Invalid string's     {
 *                      number of     =  {  (c)  ['nbr_dp'               +  ,  if 'nbr_dig' = 0  AND
 *                   question marks      {         1 (for decimal point) +        'nbr_dp'  > 0
 *                                       {         1 (for negative sign) ]
 *                                       {
 *                                       {  (d)  ['nbr_dig'              +  ,  if 'nbr_dig' > 0  AND
 *                                       {        'nbr_dp'               +        'nbr_dp'  > 0
 *                                       {         1 (for decimal point) ]
 *               @endverbatim
 *******************************************************************************************************/

#if (LIB_STR_CFG_FP_EN == DEF_ENABLED)
CPU_CHAR *Str_FmtNbr_32(CPU_FP32    nbr,
                        CPU_INT08U  nbr_dig,
                        CPU_INT08U  nbr_dp,
                        CPU_CHAR    lead_char,
                        CPU_BOOLEAN nul,
                        CPU_CHAR    *p_str)
{
  CPU_CHAR    *p_str_fmt;
  CPU_DATA    i;
  CPU_FP32    nbr_fmt;
  CPU_FP32    nbr_log;
  CPU_INT32U  nbr_shiftd;
  CPU_INT16U  nbr_dig_max;
  CPU_INT16U  nbr_dig_sig = 0;
  CPU_INT08U  nbr_neg_sign;
  CPU_INT08U  dig_val;
  CPU_FP32    dig_exp;
  CPU_FP32    dp_exp;
  CPU_BOOLEAN lead_char_dig;
  CPU_BOOLEAN lead_char_fmtd = DEF_NO;
  CPU_BOOLEAN lead_char_0;
  CPU_BOOLEAN fmt_invalid;
  CPU_BOOLEAN print_char;
  CPU_BOOLEAN nbr_neg;
  CPU_BOOLEAN nbr_neg_fmtd = DEF_NO;

  //                                                               ---------------- VALIDATE FMT ARGS -----------------
  if (p_str == DEF_NULL) {                                      // Rtn NULL if str ptr NULL (see Note #3a).
    return (DEF_NULL);
  }

  dig_exp = 1.0f;
  fmt_invalid = DEF_NO;
  lead_char_0 = (lead_char == '0') ? DEF_YES : DEF_NO;          // Chk if lead char a '0' dig.
  nbr_fmt = 0.0f;
  nbr_neg = DEF_NO;

  if ((nbr_dig < 1) && (nbr_dp < 1)) {                          // If nbr digs/dps = 0, ...
    fmt_invalid = DEF_YES;                                      // ... fmt invalid str (see Note #3b).
  }

  if (lead_char != (CPU_CHAR)'\0') {
    print_char = ASCII_IsPrint(lead_char);
    if (print_char != DEF_YES) {                                // If lead char non-printable, ...
      fmt_invalid = DEF_YES;                                    // ... fmt invalid str (see Note #3d).
    } else if (lead_char != '0') {                              // Chk lead char for non-0 dig.
      lead_char_dig = ASCII_IsDig(lead_char);
      if (lead_char_dig == DEF_YES) {                           // If lead char non-0 dig, ...
        fmt_invalid = DEF_YES;                                  // ... fmt invalid str (see Note #3d).
      }
    }
  }

  //                                                               ----------------- PREPARE NBR FMT ------------------
  p_str_fmt = p_str;

  if (fmt_invalid == DEF_NO) {
    if (nbr < 0.0f) {                                           // If nbr neg, ...
      nbr_fmt = -nbr;                                           // ... negate nbr.
      nbr_neg_sign = 1u;
      nbr_neg = DEF_YES;
    } else {
      nbr_fmt = nbr;
      nbr_neg_sign = 0u;
      nbr_neg = DEF_NO;
    }

    nbr_log = nbr_fmt;
    nbr_dig_max = 0u;
    while (nbr_log >= 1.0f) {                                   // While base-10 digs avail, ...
      nbr_dig_max++;                                            // ... calc max nbr digs.
      nbr_log /= 10.0f;
    }

    if (((nbr_dig >= (nbr_dig_max + nbr_neg_sign))              // If req'd nbr digs >= (max nbr digs + neg sign)    ..
         || (nbr_dig_max < 1))                                  // .. or NO nbr digs,                                ..
        && ((nbr_dig > 1)                                       // .. but NOT [(req'd nbr dig = 1) AND               ..
            || (nbr_dp > 0)                                     // ..          (req'd nbr dp  = 0) AND               ..
            || (nbr_neg == DEF_NO))) {                          // ..          (      nbr neg    )]   (see Note #2d).
                                                                // .. prepare nbr digs to fmt.
      for (i = 1u; i < nbr_dig; i++) {
        dig_exp *= 10.0f;
      }

      nbr_neg_fmtd = DEF_NO;
      nbr_dig_sig = 0u;
      lead_char_fmtd = DEF_NO;
    } else {                                                    // Else if nbr trunc'd, ...
      fmt_invalid = DEF_YES;                                    // ... fmt invalid str (see Note #3c).
    }
  }

  //                                                               ------------------- FMT NBR STR --------------------
  for (i = nbr_dig; i > 0; i--) {                               // Fmt str for desired nbr digs :
    if (fmt_invalid == DEF_NO) {
      if (nbr_dig_sig < LIB_STR_CFG_FP_MAX_NBR_DIG_SIG) {       // If nbr sig digs < max, fmt str digs; ...
        nbr_shiftd = (CPU_INT32U)(nbr_fmt / dig_exp);
        if ((nbr_shiftd > 0)                                    // If shifted nbr > 0 ...
            || (i == 1u)) {                                     // ... OR on one's dig to fmt, ...
                                                                // ... calc & fmt dig val; ...
          if ((nbr_neg == DEF_YES)                              // If  nbr neg ...
              && (nbr_neg_fmtd == DEF_NO)) {                    // ... but neg sign NOT yet fmt'd; ...
            if (lead_char_fmtd == DEF_YES) {                    // ... & if lead char(s) fmt'd, ...
              p_str_fmt--;                                      // ... replace last lead char w/ ...
            }
            *p_str_fmt++ = '-';                                 // ... prepend neg sign (see Note #2b).
            nbr_neg_fmtd = DEF_YES;
          }

          if (nbr_shiftd > 0) {                                 // If shifted nbr > 0, ...
            dig_val = (CPU_INT08U)(nbr_shiftd % 10u);
            *p_str_fmt++ = (CPU_CHAR)(dig_val    + '0');

            nbr_dig_sig++;                                      // ... inc nbr sig digs; ...
          } else if ((nbr_dig > 1)                              // ... else if req'd digs > 1 ...
                     || (nbr_neg == DEF_NO)) {                  // ... or non-neg nbr, ...
            *p_str_fmt++ = '0';                                 // ... fmt one '0' char.
          }
        } else if ((nbr_neg == DEF_YES)                         // ... else if nbr neg ...
                   && (lead_char_0 == DEF_YES)                  // ... & lead char a '0' dig ...
                   && (nbr_neg_fmtd == DEF_NO)) {               // ... but neg sign NOT yet fmt'd, ...
          *p_str_fmt++ = '-';                                   // ... prepend neg sign; ...
          nbr_neg_fmtd = DEF_YES;
        } else if (lead_char != (CPU_CHAR)'\0') {               // ... else if avail, ...
          *p_str_fmt++ = lead_char;                             // ... fmt lead char.
          lead_char_fmtd = DEF_YES;
        }

        dig_exp /= 10.0f;                                       // Shift to next least-sig dig.
      } else {                                                  // ... else append non-sig 0's (see Note #2h).
        *p_str_fmt++ = '0';
      }
    } else {                                                    // Else fmt '?' for invalid str (see Note #4).
      *p_str_fmt++ = '?';
    }
  }

  if (nbr_dp > 0) {                                             // Fmt str for desired nbr dp :
    if (nbr_dig < 1) {                                          // If NO digs fmt'd; ...
      if (fmt_invalid == DEF_NO) {                              // ... nbr fmt valid, ...
        if ((nbr_neg == DEF_YES)                                // ... nbr neg ...
            && (nbr_neg_fmtd == DEF_NO)) {                      // ... but neg sign NOT yet fmt'd, ...
          *p_str_fmt++ = '-';                                   // ... prepend neg sign (see Note #2b); ...
        } else {                                                // ... else prepend 1 dig of '0' ...
          *p_str_fmt++ = '0';
        }
      } else {                                                  // ... else fmt '?' for invalid str (see Note #4).
        *p_str_fmt++ = '?';
      }
    }

    if (fmt_invalid == DEF_NO) {                                // If nbr fmt valid, ...
      *p_str_fmt++ = '.';                                       // ... append dp prior to dp conversion.
    } else {                                                    // Else fmt '?' for invalid str (see Note #4).
      *p_str_fmt++ = '?';
    }

    dp_exp = 10.0f;
    for (i = 0u; i < nbr_dp; i++) {
      if (fmt_invalid == DEF_NO) {
        //                                                         If nbr sig digs < max, fmt str dps; ...
        if (nbr_dig_sig < LIB_STR_CFG_FP_MAX_NBR_DIG_SIG) {
          nbr_shiftd = (CPU_INT32U)(nbr_fmt * dp_exp);
          dig_val = (CPU_INT08U)(nbr_shiftd % 10u);
          *p_str_fmt++ = (CPU_CHAR)(dig_val    + '0');
          dp_exp *= 10.0f;                                      // Shift to next least-sig dp.

          if ((nbr_shiftd > 0)                                  // If shifted nbr > 0 ...
              || (nbr_dig_sig > 0)) {                           // ... OR  > 0 sig digs already fmt'd, ...
            nbr_dig_sig++;                                      // ... inc nbr sig digs.
          }
        } else {                                                // ... else append non-sig 0's (see Note #2c2).
          *p_str_fmt++ = '0';
        }
      } else {                                                  // Else fmt '?' for invalid str (see Note #4).
        *p_str_fmt++ = '?';
      }
    }
  }

  if (nul != DEF_NO) {                                          // If NOT DISABLED, append NULL char.
    *p_str_fmt = (CPU_CHAR)'\0';
  }

  if (fmt_invalid != DEF_NO) {                                  // Rtn NULL for invalid str fmt (see Notes #3a - #3d).
    return (DEF_NULL);
  }

  return (p_str);                                               // Rtn ptr to fmt'd str (see Note #3e).
}
#endif

/****************************************************************************************************//**
 *                                           Str_ParseNbr_Int32U()
 *
 * @brief    Parses a 32-bit unsigned integer from a string.
 *
 * @param    p_str       Pointer to the string. String buffer NOT modified.
 *
 * @param    p_str_next  Optional pointer to a variable to :
 *                       Return a pointer to first character following the integer string, if NO
 *                       error(s);
 *                       Return a pointer to 'p_str', otherwise.
 *
 * @param    nbr_base    Base of number to parse.
 *
 * @return   Parsed integer, if integer parsed with NO overflow.
 *           DEF_INT_32U_MAX_VAL, if integer parsed but overflowed.
 *           0, otherwise.
 *
 * @note     (1) See Str_ParseNbr_Int32()'s notes.
 *******************************************************************************************************/
CPU_INT32U Str_ParseNbr_Int32U(const CPU_CHAR *p_str,
                               CPU_CHAR       **p_str_next,
                               CPU_INT08U     nbr_base)
{
  CPU_INT32U nbr;

  nbr = Str_ParseNbr_Int32(p_str,                               // Parse/convert str ...
                           p_str_next,
                           nbr_base,
                           DEF_NO,                              // ... as unsigned int (see Note #1).
                           DEF_NULL);

  return (nbr);
}

/****************************************************************************************************//**
 *                                           Str_ParseNbr_Int32S()
 *
 * @brief    Parses a 32-bit signed integer from a string.
 *
 * @param    p_str       Pointer to the string. String buffer NOT modified.
 *
 * @param    p_str_next  Optional pointer to a variable to :
 *                       Return a pointer to first character following the integer string, if NO
 *                       error(s);
 *                       Return a pointer to 'p_str', otherwise.
 *
 * @param    nbr_base    Base of number to parse.
 *
 * @return   Parsed integer, if integer parsed with NO overflow.
 *           DEF_INT_32S_MIN_VAL, if integer parsed but negatively underflowed.
 *           DEF_INT_32U_MAX_VAL, if integer parsed but positively overflowed.
 *           0, otherwise.
 *
 * @note     (1) See Str_ParseNbr_Int32()'s notes.
 *******************************************************************************************************/
CPU_INT32S Str_ParseNbr_Int32S(const CPU_CHAR *p_str,
                               CPU_CHAR       **p_str_next,
                               CPU_INT08U     nbr_base)
{
  CPU_INT32S  nbr;
  CPU_INT32U  nbr_abs;
  CPU_BOOLEAN nbr_neg;

  nbr_abs = Str_ParseNbr_Int32(p_str,                           // Parse/convert str ...
                               p_str_next,
                               nbr_base,
                               DEF_YES,                         // ... as signed int.
                               &nbr_neg);

  if (nbr_neg == DEF_NO) {                                      // Chk for neg nbr & ovf/undf.
    nbr = (nbr_abs > (CPU_INT32U) DEF_INT_32S_MAX_VAL)          ?  (CPU_INT32S)DEF_INT_32S_MAX_VAL
          :  (CPU_INT32S)nbr_abs;
  } else {
    nbr = (nbr_abs > (CPU_INT32U)-DEF_INT_32S_MIN_VAL_ONES_CPL) ?  (CPU_INT32S)DEF_INT_32S_MIN_VAL
          : -(CPU_INT32S)nbr_abs;
  }

  return (nbr);
}

/****************************************************************************************************//**
 *                                               Str_Printf()
 *
 * @brief    printf()-like function that accepts a character output callback.
 *
 * @param    out_cb          Character output callback.
 *
 * @param    p_out_cb_arg    Character output callback argument.
 *
 * @param    format          Format string.
 *
 * @param    ...         Variable number of arguments that will be formatted according to the
 *                       format specifiers in the format string.
 *
 * @return   See Str_PrintfImpl() return value description.
 *
 * @note     (1) See Str_PrintfImpl() notes.
 *******************************************************************************************************/
CPU_INT16U Str_Printf(STR_PRINTF_OUT_CB out_cb,
                      void              *p_out_cb_arg,
                      const CPU_CHAR    *format,
                      ...)
{
  CPU_INT16U char_cnt;
  va_list    args;

  va_start(args, format);
  char_cnt = Str_PrintfImpl(out_cb,
                            p_out_cb_arg,
                            (CPU_INT16U)-1,
                            format,
                            args);
  va_end(args);

  return (char_cnt);
}

/****************************************************************************************************//**
 *                                               Str_Sprintf()
 *
 * @brief    sprintf() implementation.
 *
 * @param    p_str   Buffer that will receive the formatted output.
 *
 * @param    format  Format string.
 *
 * @param    ...     Variable number of arguments that will be formatted according to the
 *                   format specifiers in the format string.
 *
 * @return   See Str_PrintfImpl() return value description.
 *
 * @note     (1) See Str_PrintfImpl() notes.
 *
 * @note     (2) The number of written characters excludes the ending null character.
 *******************************************************************************************************/
CPU_INT16U Str_Sprintf(CPU_CHAR       *p_str,
                       const CPU_CHAR *format,
                       ...)
{
  CPU_INT16U         char_cnt;
  STR_SPRINTF_CB_ARG cb_arg;
  va_list            args;

  cb_arg.BufPtr = p_str;
  cb_arg.CurIx = 0u;

  va_start(args, format);
  //                                                               Format string according to format specifiers and...
  //                                                               ...store formatted in given buffer.
  char_cnt = Str_PrintfImpl(Str_SprintfCb,
                            &cb_arg,
                            (CPU_INT16U)-1,
                            format,
                            args);
  va_end(args);

  if (char_cnt != 0u) {
    p_str[char_cnt] = '\0';                                     // Append automatically ending null character.
  }

  return (char_cnt);                                            // See Note #2.
}

/****************************************************************************************************//**
 *                                               Str_Snprintf()
 *
 * @brief    snprintf() implementation.
 *
 * @param    p_str   Buffer that will receive the formatted output.
 *
 * @param    size    Total capacity of the given string buffer.
 *
 * @param    format  Format string.
 *
 * @param    ...     Variable number of arguments that will be formatted according to the
 *                   format specifiers in the format string.
 *
 * @return   See Str_PrintfImpl() return value description.
 *
 * @note     (1) See Str_PrintfImpl() notes.
 *
 * @note     (2) The number of written characters excludes the ending null character.
 *******************************************************************************************************/
CPU_INT16U Str_Snprintf(CPU_CHAR       *p_str,
                        CPU_INT16U     size,
                        const CPU_CHAR *format,
                        ...)
{
  CPU_INT16U         char_cnt;
  STR_SPRINTF_CB_ARG cb_arg;
  va_list            args;

  cb_arg.BufPtr = p_str;
  cb_arg.CurIx = 0u;

  va_start(args, format);
  //                                                               Format string according to format specifiers and...
  //                                                               ...store formatted up to N char in given buffer.
  char_cnt = Str_PrintfImpl(Str_SprintfCb,
                            &cb_arg,
                            size - 1u,
                            format,
                            args);
  va_end(args);

  if (char_cnt != 0u) {
    p_str[char_cnt] = '\0';                                     // Append automatically ending null character.
  }

  return (char_cnt);                                            // See Note #2.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Str_PrintfImpl()
 *
 * @brief    Generic printf()-like function implementation.
 *
 * @param    out_cb          Character output callback (putchar()-like with an extra callback-specific
 *                           argument).
 *
 * @param    p_out_cb_arg    Character output callback argument.
 *
 * @param    format          Format string.
 *
 * @param    argp            Variable length argument list.
 *
 * @return   In accordance with the C99 specification, this function returns:
 *               - (1) the number of written characters when no truncation occurs;
 *               - (2) the number of characters that would have been written given that the buffer was
 *                     big enough when truncation occurs.
 *
 * @note     (1) A truncation has occurred whenever the returned value is equal or greater than the
 *               'max_char_cnt' parameter.
 *
 * @note     (2) A ending null character is not appended automatically by this function. If the null
 *               character is needed, this is the responsibility of the caller function.
 *******************************************************************************************************/
static CPU_INT16U Str_PrintfImpl(STR_PRINTF_OUT_CB out_cb,
                                 void              *p_out_cb_arg,
                                 CPU_INT16U        max_char_cnt,
                                 const CPU_CHAR    *format,
                                 va_list           argp)
{
  CPU_CHAR    *p_cur_char;
  CPU_CHAR    pad_char;
  CPU_SIZE_T  output_char_cnt;
  CPU_SIZE_T  min_field_width;
  CPU_SIZE_T  max_field_width;
  CPU_BOOLEAN left_justify;
  CPU_BOOLEAN long_long_flag;
  CPU_BOOLEAN long_flag;
  CPU_BOOLEAN dot_flag;
  CPU_BOOLEAN arg_parsed;

  p_cur_char = (CPU_CHAR *)format;
  output_char_cnt = 0u;

  while (*p_cur_char != '\0') {
    //                                                             move format string chars to buffer until a
    //                                                             format control is found.
    while ((*p_cur_char != '%') && (*p_cur_char != '\0')) {
      if (output_char_cnt < max_char_cnt) {
        out_cb((int)(*p_cur_char), p_out_cb_arg);
      }
      output_char_cnt += 1u;
      p_cur_char += 1u;
    }

    if (*p_cur_char == '\0') {
      break;
    }

    p_cur_char += 1u;

    //                                                             initialize all the flags for this format.
    dot_flag = DEF_NO;
    long_flag = DEF_NO;
    long_long_flag = DEF_NO;
    left_justify = DEF_NO;
    min_field_width = 0u;
    max_field_width = (CPU_SIZE_T)-1;
    pad_char = ' ';

    arg_parsed = DEF_NO;
    while (!arg_parsed && (*p_cur_char != '\0')) {
      if (ASCII_IsDig(*p_cur_char)) {
        if (dot_flag) {
          min_field_width = (CPU_SIZE_T)Str_ParseNbr_Int32U(p_cur_char,
                                                            &p_cur_char,
                                                            DEF_NBR_BASE_DEC);
        } else {
          if (*p_cur_char == '0') {
            pad_char = '0';
            p_cur_char += 1u;
          } else {
            min_field_width = (CPU_SIZE_T)Str_ParseNbr_Int32U(p_cur_char,
                                                              &p_cur_char,
                                                              DEF_NBR_BASE_DEC);
          }
        }
      } else {
        switch (ASCII_ToLower(*p_cur_char)) {
          case '%':
            if (output_char_cnt < max_char_cnt) {
              out_cb((int)'%', p_out_cb_arg);
            }
            output_char_cnt += 1u;
            break;

          case '-':
            left_justify = DEF_YES;
            break;

          case '.':
            dot_flag = DEF_YES;
            break;

          case 'l':
            if (long_flag) {
              long_long_flag = DEF_YES;
            } else {
              long_flag = DEF_YES;
            }
            break;

          case '*':
            if (dot_flag) {
              min_field_width = va_arg(argp, int);
            } else {
              max_field_width = va_arg(argp, int);
            }
            break;
          case 'd':
            if (long_long_flag) {
              output_char_cnt += Str_SngIntFmtOutput(va_arg(argp, long long),
                                                     min_field_width,
                                                     left_justify,
                                                     pad_char,
                                                     out_cb,
                                                     p_out_cb_arg,
                                                     max_char_cnt - output_char_cnt);
            } else if (long_flag) {
              output_char_cnt += Str_SngIntFmtOutput(va_arg(argp, long),
                                                     min_field_width,
                                                     left_justify,
                                                     pad_char,
                                                     out_cb,
                                                     p_out_cb_arg,
                                                     max_char_cnt - output_char_cnt);
            } else {
              output_char_cnt += Str_SngIntFmtOutput(va_arg(argp, int),
                                                     min_field_width,
                                                     left_justify,
                                                     pad_char,
                                                     out_cb,
                                                     p_out_cb_arg,
                                                     max_char_cnt - output_char_cnt);
            }
            arg_parsed = DEF_YES;
            break;

          case 'u':
            if (long_long_flag) {
              output_char_cnt += Str_UnsgnIntFmtOutput(va_arg(argp, unsigned long long),
                                                       10u,
                                                       min_field_width,
                                                       left_justify,
                                                       pad_char,
                                                       out_cb,
                                                       p_out_cb_arg,
                                                       max_char_cnt - output_char_cnt);
            } else if (long_flag) {
              output_char_cnt += Str_UnsgnIntFmtOutput(va_arg(argp, unsigned long),
                                                       10u,
                                                       min_field_width,
                                                       left_justify,
                                                       pad_char,
                                                       out_cb,
                                                       p_out_cb_arg,
                                                       max_char_cnt - output_char_cnt);
            } else {
              output_char_cnt += Str_UnsgnIntFmtOutput(va_arg(argp, unsigned int),
                                                       10u,
                                                       min_field_width,
                                                       left_justify,
                                                       pad_char,
                                                       out_cb,
                                                       p_out_cb_arg,
                                                       max_char_cnt - output_char_cnt);
            }
            arg_parsed = DEF_YES;
            break;

          case 'p':
          case 'x':
            if (output_char_cnt < max_char_cnt) {
              out_cb((int)'0', p_out_cb_arg);
            }
            output_char_cnt += 1u;
            if (output_char_cnt < max_char_cnt) {
              if (*p_cur_char == 'X') {
                out_cb((int)'X', p_out_cb_arg);
              } else {
                out_cb((int)'x', p_out_cb_arg);
              }
            }
            output_char_cnt += 1u;
            if (long_flag) {
              output_char_cnt += Str_UnsgnIntFmtOutput(va_arg(argp, unsigned long),
                                                       16u,
                                                       min_field_width,
                                                       left_justify,
                                                       pad_char,
                                                       out_cb,
                                                       p_out_cb_arg,
                                                       max_char_cnt - output_char_cnt);
            } else if (long_long_flag) {
              output_char_cnt += Str_UnsgnIntFmtOutput(va_arg(argp, unsigned long long),
                                                       16u,
                                                       min_field_width,
                                                       left_justify,
                                                       pad_char,
                                                       out_cb,
                                                       p_out_cb_arg,
                                                       max_char_cnt - output_char_cnt);
            } else {
              if (*p_cur_char == 'p') {
                output_char_cnt += Str_UnsgnIntFmtOutput((CPU_ADDR)va_arg(argp, void *),
                                                         16u,
                                                         min_field_width,
                                                         left_justify,
                                                         pad_char,
                                                         out_cb,
                                                         p_out_cb_arg,
                                                         max_char_cnt - output_char_cnt);
              } else {
                output_char_cnt += Str_UnsgnIntFmtOutput(va_arg(argp, unsigned int),
                                                         16u,
                                                         min_field_width,
                                                         left_justify,
                                                         pad_char,
                                                         out_cb,
                                                         p_out_cb_arg,
                                                         max_char_cnt - output_char_cnt);
              }
            }
            arg_parsed = DEF_YES;
            break;

          case 's':
            output_char_cnt += Str_FmtOutput(va_arg(argp, CPU_CHAR *),
                                             min_field_width,
                                             max_field_width,
                                             left_justify,
                                             out_cb,
                                             p_out_cb_arg,
                                             max_char_cnt - output_char_cnt);
            arg_parsed = DEF_YES;
            break;

          case 'c':
            if (output_char_cnt < max_char_cnt) {
              out_cb(va_arg(argp, int), p_out_cb_arg);
            }
            output_char_cnt += 1u;
            arg_parsed = DEF_YES;
            break;

          case '\\':
            if (output_char_cnt < max_char_cnt) {
              switch (*p_cur_char) {
                case 'a':
                  out_cb(0x07, p_out_cb_arg);
                  break;
                case 'h':
                  out_cb(0x08, p_out_cb_arg);
                  break;
                case 'r':
                  out_cb(0x0D, p_out_cb_arg);
                  break;
                case 'n':
                  out_cb(0x0A, p_out_cb_arg);
                  break;
                default:
                  out_cb((int)(*p_cur_char), p_out_cb_arg);
                  break;
              }
            }
            output_char_cnt += 1u;
            break;

          default:
            break;
        }
        p_cur_char += 1u;
      }
    }
  }

  return (output_char_cnt);
}

/****************************************************************************************************//**
 *                                               Str_SprintfCb()
 *
 * @brief    Str_Sprintf() and Str_Snprintf() output callback.
 *
 * @param    c       Character to be written.
 *
 * @param    p_arg   Pointer to an STR_SPRINTF_CB_ARG structure.
 *
 * @return   The written character.
 *******************************************************************************************************/
static int Str_SprintfCb(int  c,
                         void *p_arg)
{
  STR_SPRINTF_CB_ARG *p_sprintf_cb_arg;

  p_sprintf_cb_arg = (STR_SPRINTF_CB_ARG *)p_arg;
  p_sprintf_cb_arg->BufPtr[p_sprintf_cb_arg->CurIx++] = (CPU_CHAR)c;

  return (c);
}

/****************************************************************************************************//**
 *                                               Str_FmtOutput()
 *
 * @brief    Justifies and outputs a given string.
 *
 * @param    p_str           Pointer to the string to output.
 *
 * @param    min_char_cnt    Minimum character count to output (pad if string shorter).
 *
 * @param    max_char_cnt    Maximum character count to output (truncate if string longer).
 *
 * @param    left_justify    Indicates whether to left or right justify.
 *
 * @param    out_cb          Character output callback.
 *
 * @param    p_out_cb_arg    Character output callback argument.
 *
 * @param    rem_size        Remaining character count to process.
 *******************************************************************************************************/
static CPU_SIZE_T Str_FmtOutput(CPU_CHAR          *p_str,
                                CPU_SIZE_T        min_char_cnt,
                                CPU_SIZE_T        max_char_cnt,
                                CPU_BOOLEAN       left_justify,
                                STR_PRINTF_OUT_CB out_cb,
                                void              *p_out_cb_arg,
                                CPU_INT32S        rem_size)
{
  CPU_CHAR   *p_cur_char;
  CPU_CHAR   prev_char;
  CPU_SIZE_T rem_char_cnt;
  CPU_SIZE_T pad_char_cnt;
  CPU_SIZE_T output_char_cnt;
  CPU_SIZE_T len;

  //                                                               Chk if padding needed.
  len = Str_Len(p_str);
  pad_char_cnt = (len < min_char_cnt) ? min_char_cnt - len : 0u;
  output_char_cnt = 0u;

  //                                                               Pad left if needed.
  if (!left_justify && (pad_char_cnt > 0u)) {
    while (pad_char_cnt > 0u) {
      if (rem_size > 0) {
        out_cb((int)' ', p_out_cb_arg);
      }
      output_char_cnt += 1u;
      pad_char_cnt -= 1u;
      rem_size -= 1u;
    }
  }

  //                                                               Move string to the buffer
  rem_char_cnt = DEF_MIN(DEF_MAX(min_char_cnt, len), max_char_cnt);
  p_cur_char = p_str;
  prev_char = '\0';
  while ((*p_cur_char != 0u) && (rem_char_cnt > 0u)) {
    if (rem_size > 0) {
      out_cb((int)(*p_cur_char), p_out_cb_arg);
    }
    prev_char = *p_cur_char;
    output_char_cnt += 1u;
    p_cur_char += 1u;
    rem_char_cnt -= 1u;
    rem_size -= 1u;
  }
  //                                                               Pad right if needed.
  if (left_justify && (pad_char_cnt > 0u) && (prev_char != '\n') && (prev_char != '\r')) {
    while (pad_char_cnt > 0u) {
      if (rem_size > 0) {
        out_cb((int)' ', p_out_cb_arg);
      }
      output_char_cnt += 1u;
      pad_char_cnt -= 1u;
      rem_size -= 1u;
    }
  }

  return (output_char_cnt);
}

/****************************************************************************************************//**
 *                                           Str_UnsgnIntFmtOutput()
 *
 * @brief    Formats and outputs an unsigned integer.
 *
 * @param    nbr             Number to output as a string.
 *
 * @param    base            Base used for the string conversion.
 *
 * @param    min_digit_cnt   Minimum number of digits to output (pad if less digits).
 *
 * @param    left_justify    Indicates whether to left or right justify.
 *
 * @param    pad_char        Character used for padding (normally either ' ' or '0').
 *
 * @param    out_cb          Character output callback.
 *
 * @param    p_out_cb_arg    Character output callback argument.
 *
 * @param    rem_size        Remaining character count to process.
 *******************************************************************************************************/
static CPU_SIZE_T Str_UnsgnIntFmtOutput(CPU_INT64U        nbr,
                                        CPU_INT08U        base,
                                        CPU_SIZE_T        min_digit_cnt,
                                        CPU_BOOLEAN       left_justify,
                                        CPU_CHAR          pad_char,
                                        STR_PRINTF_OUT_CB out_cb,
                                        void              *p_out_cb_arg,
                                        CPU_INT32S        rem_size)
{
  CPU_CHAR              *p_cur_char;
  CPU_CHAR              outbuf[32];
  CPU_SIZE_T            len;
  CPU_SIZE_T            pad_char_cnt;
  CPU_SIZE_T            output_char_cnt;
  static const CPU_CHAR digits[] = "0123456789ABCDEF";

  //                                                               Build number.
  p_cur_char = outbuf;
  do {
    *p_cur_char = digits[nbr % base];
    p_cur_char += 1u;
  } while ((nbr /= base) > 0);

  len = p_cur_char - outbuf;
  pad_char_cnt = (len < min_digit_cnt) ? min_digit_cnt - len : 0u;
  output_char_cnt = 0u;

  //                                                               Pad left if needed.
  if (!left_justify && (pad_char_cnt > 0u)) {
    while (pad_char_cnt > 0u) {
      if (rem_size > 0) {
        out_cb((int)pad_char, p_out_cb_arg);
      }
      output_char_cnt += 1u;
      pad_char_cnt -= 1u;
      rem_size -= 1u;
    }
  }

  //                                                               Output number.
  p_cur_char -= 1u;
  while (p_cur_char >= outbuf) {
    if (rem_size > 0) {
      out_cb((int)(*p_cur_char), p_out_cb_arg);
    }
    output_char_cnt += 1u;
    p_cur_char -= 1u;
    rem_size -= 1u;
  }

  //                                                               Pad right if needed.
  if (left_justify && (pad_char_cnt > 0u)) {
    while (pad_char_cnt > 0u) {
      if (rem_size > 0) {
        out_cb((int)pad_char, p_out_cb_arg);
      }
      output_char_cnt += 1u;
      pad_char_cnt -= 1u;
      rem_size -= 1u;
    }
  }

  return (output_char_cnt);
}

/****************************************************************************************************//**
 *                                           Str_SngIntFmtOutput()
 *
 * @brief    Formats and outputs an signed integer.
 *
 * @param    nbr             Number to output as a string.
 *
 * @param    min_digit_cnt   Minimum number of digits to output (pad if less digits).
 *
 * @param    left_justify    Indicates whether to left or right justify.
 *
 * @param    pad_char        Character used for padding (normally either ' ' or '0').
 *
 * @param    out_cb          putchar-like callback.
 *
 * @param    p_out_cb_arg    Argument passed to callback.
 *
 * @param    rem_size        Remaining character count to process.
 *******************************************************************************************************/
static CPU_SIZE_T Str_SngIntFmtOutput(CPU_INT64S        nbr,
                                      CPU_SIZE_T        min_digit_cnt,
                                      CPU_BOOLEAN       left_justify,
                                      CPU_CHAR          pad_char,
                                      STR_PRINTF_OUT_CB out_cb,
                                      void              *p_out_cb_arg,
                                      CPU_INT32S        rem_size)
{
  CPU_CHAR              *p_cur_char;
  CPU_CHAR              outbuf[32];
  CPU_INT64U            abs_val;
  CPU_SIZE_T            len;
  CPU_SIZE_T            pad_char_cnt;
  CPU_SIZE_T            output_char_cnt;
  static const CPU_CHAR digits[] = "0123456789ABCDEF";

  //                                                               Build number (absolute value).
  abs_val = (nbr < 0u) ? -nbr : nbr;
  p_cur_char = outbuf;
  do {
    *p_cur_char = digits[abs_val % 10];
    abs_val /= 10;
    p_cur_char += 1u;
  } while (abs_val > 0);

  //                                                               Add minus sign if needed.
  if (nbr < 0) {
    *p_cur_char = '-';
    p_cur_char += 1u;
  }

  len = p_cur_char - outbuf;
  pad_char_cnt = (len < min_digit_cnt) ? min_digit_cnt - len : 0u;
  output_char_cnt = 0u;

  //                                                               Pad left if needed.
  if (!left_justify && (pad_char_cnt > 0u)) {
    while (pad_char_cnt > 0u) {
      if (rem_size > 0) {
        out_cb((int)pad_char, p_out_cb_arg);
      }
      output_char_cnt += 1u;
      pad_char_cnt -= 1u;
      rem_size -= 1u;
    }
  }

  //                                                               Output number.
  p_cur_char -= 1u;
  while (p_cur_char >= outbuf) {
    if (rem_size > 0) {
      out_cb((int)(*p_cur_char), p_out_cb_arg);
    }
    output_char_cnt += 1u;
    p_cur_char -= 1u;
    rem_size -= 1u;
  }

  //                                                               Pad right if needed.
  if (left_justify && (pad_char_cnt > 0u)) {
    while (pad_char_cnt > 0u) {
      if (rem_size > 0) {
        out_cb((int)pad_char, p_out_cb_arg);
      }
      output_char_cnt += 1u;
      pad_char_cnt -= 1u;
      rem_size -= 1u;
    }
  }

  return (output_char_cnt);
}

/****************************************************************************************************//**
 *                                           Str_FmtNbr_Int32()
 *
 * @brief    Formats a 32-bit integer into a multi-digit character string.
 *
 * @param    nbr         Number to format.
 *
 * @param    nbr_dig     Number of digits to format (see Note #1).
 *
 * @param    nbr_base    Base of number to format. The number's base MUST be between 2 and 36,
 *                       inclusively.
 *                       The following may be used to specify the number base :
 *                           - DEF_NBR_BASE_BIN            Base  2
 *                           - DEF_NBR_BASE_OCT            Base  8
 *                           - DEF_NBR_BASE_DEC            Base 10
 *                           - DEF_NBR_BASE_HEX            Base 16
 *
 * @param    nbr_neg     Indicates whether number to format is negative :
 *                           - DEF_NO          Number is non-negative.
 *                           - DEF_YES         Number is     negative.
 *                       Argument validated in Str_FmtNbr_Int32U(),
 *                       Str_FmtNbr_Int32S().
 *
 * @param    lead_char   Prepend leading character. Leading character option prepends leading
 *                       characters prior to the first non-zero digit. Leading character MUST be a
 *                       printable ASCII character. Leading character MUST NOT be a number base digit,
 *                       with the exception of '0'.
 *                           - '\0'                    Do NOT prepend leading character to the string.
 *                           - Printable character     Prepend leading character to the string.
 *                           - Unprintable character   Format invalid string.
 *
 * @param    lower_case  Format alphabetic characters (if any) in lower case :
 *                           - DEF_NO          Format alphabetic characters in upper case.
 *                           - DEF_YES         Format alphabetic characters in lower case.
 *
 * @param    nul         Append terminating NULL-character. NULL-character terminate option DISABLED
 *                       prevents overwriting previous character array formatting. Unless 'p_str'
 *                       character array is pre-/post-terminated, NULL-character terminate option
 *                       DISABLED will cause character string run-on.
 *                           - DEF_NO          Do NOT append terminating NULL-character to the string.
 *                           - DEF_YES         Append terminating NULL-character to the string.
 *
 * @param    p_str       Pointer to the character array to return formatted number string. Format
 *                       buffer size is NOT validated; buffer overruns MUST be prevented by caller.
 *                       To prevent character buffer overrun, character array size MUST be >=
 *                       ('nbr_dig' + 1 negative sign + 1 'NUL' terminator) characters.
 *
 * @return   Pointer to the formatted string, if NO error(s).
 *           Pointer to NULL, otherwise.
 *
 * @note     (1) The maximum number of digits to format for 32-bit integer numbers :
 *                           @verbatim
 *                           Maximum Number of             [  log (Number)      ]
 *                       32-bit Integer Digits  =  floor [ -------------- + 1 ]
 *                               to Format                 [   log (Base)       ]
 *
 *                           where
 *                                   Number                  Number to format
 *                                   Base            Base of number to format
 *                           @endverbatim
 *           - (a) If the number of digits to format ('nbr_dig') is zero; then NO formatting is
 *                   performed except possible NULL-termination of the string.
 *                       Example :
 *                           @verbatim
 *                           nbr      = -23456
 *                           nbr_dig  =  0
 *                           nbr_base = 10
 *
 *                           p_str    = ""                       See Note #4a
 *                           @endverbatim
 *           - (b) If the number of digits to format ('nbr_dig') is less than the number of
 *                   significant integer digits of the number to format ('nbr'); then an invalid
 *                   string is formatted instead of truncating any significant integer digits.
 *                       Example :
 *                           @verbatim
 *                           nbr      = 23456
 *                           nbr_dig  = 3
 *                           nbr_base = 10
 *
 *                           p_str    = "???"                    See Note #4b
 *                           @endverbatim
 *           - (c) If the number to format ('nbr') is negative but the number of digits to format
 *                   ('nbr_dig') is equal to the number of significant integer digits of the number to
 *                   format ('nbr'); then an invalid string is formatted instead of truncating the
 *                   negative sign.
 *                       Example :
 *                           @verbatim
 *                           nbr      = -23456
 *                           nbr_dig  =  5
 *                           nbr_base = 10
 *
 *                           p_str    = "?????"                  See Note #4b
 *                           @endverbatim
 *
 * @note     (2) See usage examples below.
 *           - (a) The number of leading characters is such that the total number of significant
 *                   integer digits plus the number of leading characters plus possible negative sign
 *                   character is equal to the requested number of integer digits to format
 *                   ('nbr_dig').
 *                       Examples :
 *                           @verbatim
 *                           nbr       = 23456
 *                           nbr_dig   = 7
 *                           nbr_base  = 10
 *                           lead_char = ' '
 *
 *                           p_str     = "  23456"
 *
 *                           nbr       = -23456
 *                           nbr_dig   = 7
 *                           nbr_base  = 10
 *                           lead_char = ' '
 *
 *                           p_str     = " -23456"
 *                           @endverbatim
 *           - (b) If the number to format ('nbr') is negative AND the leading character
 *                   ('lead_char') is a '0' digit; then the negative sign character prefixes all
 *                   leading characters prior to the formatted number.
 *                       Examples :
 *                           @verbatim
 *                           nbr        = -23456
 *                           nbr_dig    =  8
 *                           nbr_base   = 10
 *                           lead_char  = '0'
 *
 *                           p_str      = "-0023456"
 *
 *                           nbr        = -43981
 *                           nbr_dig    =  8
 *                           nbr_base   = 16
 *                           lead_char  = '0'
 *                           lower_case = DEF_NO
 *
 *                           p_str      = "-000ABCD"
 *                           @endverbatim
 *           - (c) If the number to format ('nbr') is negative AND the leading character
 *                   ('lead_char') is NOT a '0' digit; then the negative sign character immediately
 *                   prefixes the most significant digit of the formatted number.
 *                       Examples :
 *                           @verbatim
 *                           nbr        = -23456
 *                           nbr_dig    =  8
 *                           nbr_base   = 10
 *                           lead_char  = '#'
 *
 *                           p_str      = "##-23456"
 *
 *                           nbr        = -43981
 *                           nbr_dig    =  8
 *                           nbr_base   = 16
 *                           lead_char  = '#'
 *                           lower_case = DEF_YES
 *
 *                           p_str      = "###-abcd"
 *                           @endverbatim
 *           - (d) If the value of the number to format is zero & the number of digits to format is
 *                   non-zero, but NO leading character available; then one digit of '0' value is
 *                   formatted. This is NOT a leading character; but a single integer digit of '0'
 *                   value.
 *
 * @note     (3) String format terminates when :
 *               - (a) Format string pointer is passed a NULL pointer. No string formatted. NULL
 *                     pointer is returned.
 *               - (b) Number of digits to format ('nbr_dig') is zero. NULL string formatted. NULL
 *                     pointer is returned.
 *               - (c) Number of digits to format ('nbr_dig') is less than number of significant integer
 *                     digits of the number to format ('nbr'), including possible negative sign. Invalid
 *                     string formatted. NULL pointer is returned.
 *               - (d) Base is passed an invalid base. Invalid string format performed; NULL pointer is
 *                     returned.
 *               - (e) Lead character is NOT a valid, printable character. Invalid string formatted.
 *                     NULL pointer is returned.
 *               - (f) Number successfully formatted into character string array.
 *
 * @note     (4) For any unsuccessful string format or error(s), an invalid string of question marks
 *               ('?') will be formatted, where the number of question marks is determined by the
 *               number of digits to format ('nbr_dig') :
 *                   @verbatim
 *                   Invalid string's     {  (a)   0 (NULL string)  ,  if 'nbr_dig' = 0
 *                       number of     =  {
 *                   question marks      {  (b)  'nbr_dig'         ,  if 'nbr_dig' > 0
 *                   @endverbatim
 *******************************************************************************************************/
static CPU_CHAR *Str_FmtNbr_Int32(CPU_INT32U  nbr,
                                  CPU_INT08U  nbr_dig,
                                  CPU_INT08U  nbr_base,
                                  CPU_BOOLEAN nbr_neg,
                                  CPU_CHAR    lead_char,
                                  CPU_BOOLEAN lower_case,
                                  CPU_BOOLEAN nul,
                                  CPU_CHAR    *p_str)
{
  CPU_CHAR    *p_str_fmt;
  CPU_DATA    i;
  CPU_INT32U  nbr_fmt = 0;
  CPU_INT32U  nbr_log;
  CPU_INT08U  nbr_dig_max;
  CPU_INT08U  nbr_dig_min;
  CPU_INT08U  nbr_dig_fmtd = 0;
  CPU_INT08U  nbr_neg_sign;
  CPU_INT08U  nbr_lead_char;
  CPU_INT08U  dig_val;
  CPU_INT08U  lead_char_delta_0;
  CPU_INT08U  lead_char_delta_a;
  CPU_BOOLEAN lead_char_dig;
  CPU_BOOLEAN lead_char_0;
  CPU_BOOLEAN fmt_valid = DEF_YES;
  CPU_BOOLEAN print_char;
  CPU_BOOLEAN nbr_neg_fmtd = DEF_NO;

  //                                                               ---------------- VALIDATE FMT ARGS -----------------
  if (p_str == DEF_NULL) {                                      // Rtn NULL if str ptr NULL (see Note #3a).
    return (DEF_NULL);
  }

  if (nbr_dig < 1) {                                            // If nbr digs = 0, ...
    fmt_valid = DEF_NO;                                         // ... fmt valid str (see Note #3b).
  }
  //                                                               If invalid base, ...
  if ((nbr_base < 2u)
      || (nbr_base > 36u)) {
    fmt_valid = DEF_NO;                                         // ... fmt valid str (see Note #3d).
  }

  if (lead_char != (CPU_CHAR)'\0') {
    print_char = ASCII_IsPrint(lead_char);
    if (print_char != DEF_YES) {                                // If lead char non-printable, ...
      fmt_valid = DEF_NO;                                       // ... fmt valid str (see Note #3e).
    } else if (lead_char != '0') {                              // Chk lead char for non-0 nbr base dig.
      lead_char_delta_0 = (CPU_INT08U)(lead_char - '0');
      if (lower_case != DEF_YES) {
        lead_char_delta_a = (CPU_INT08U)(lead_char - 'A');
      } else {
        lead_char_delta_a = (CPU_INT08U)(lead_char - 'a');
      }

      lead_char_dig = (((nbr_base <= 10u) &&  (lead_char_delta_0 < nbr_base))
                       || ((nbr_base > 10u) && ((lead_char_delta_0 < 10u)
                                                || (lead_char_delta_a < (nbr_base - 10u))))) ? DEF_YES : DEF_NO;

      if (lead_char_dig == DEF_YES) {                           // If lead char non-0 nbr base dig, ...
        fmt_valid = DEF_NO;                                     // ... fmt valid str               (see Note #3e).
      }
    }
  }

  //                                                               ----------------- PREPARE NBR FMT ------------------
  p_str_fmt = p_str;
  lead_char_0 = DEF_NO;

  if (fmt_valid == DEF_YES) {
    nbr_fmt = nbr;
    nbr_log = nbr;
    nbr_dig_max = 1u;
    while (nbr_log >= nbr_base) {                               // While nbr base digs avail, ...
      nbr_dig_max++;                                            // ... calc max nbr digs.
      nbr_log /= nbr_base;
    }

    nbr_neg_sign = (nbr_neg == DEF_YES) ? 1u : 0u;
    if (nbr_dig >= (nbr_dig_max + nbr_neg_sign)) {              // If req'd nbr digs >= (max nbr digs + neg sign), ...
      nbr_neg_fmtd = DEF_NO;
      nbr_dig_min = DEF_MIN(nbr_dig_max, nbr_dig);
      //                                                           ... calc nbr digs to fmt & nbr lead chars.
      if (lead_char != (CPU_CHAR)'\0') {
        nbr_dig_fmtd = nbr_dig;
        nbr_lead_char = nbr_dig
                        - (nbr_dig_min + nbr_neg_sign);
      } else {
        nbr_dig_fmtd = nbr_dig_min + nbr_neg_sign;
        nbr_lead_char = 0u;
      }

      if (nbr_lead_char > 0) {                                  // If lead chars to fmt, ...
        lead_char_0 = (lead_char == '0')                        // ... chk if lead char a '0' dig.
                      ?  DEF_YES : DEF_NO;
      }
    } else {                                                    // Else if nbr trunc'd, ...
      fmt_valid = DEF_NO;                                       // ... fmt valid str (see Note #3c).
    }
  }

  if (fmt_valid == DEF_NO) {
    nbr_dig_fmtd = nbr_dig;
  }

  //                                                               ------------------- FMT NBR STR --------------------
  p_str_fmt += nbr_dig_fmtd;                                    // Start fmt @ least-sig dig.

  if (nul != DEF_NO) {                                          // If NOT DISABLED, append NULL char.
    *p_str_fmt = (CPU_CHAR)'\0';
  }
  p_str_fmt--;

  for (i = 0u; i < nbr_dig_fmtd; i++) {                         // Fmt str for desired nbr digs :
    if (fmt_valid == DEF_YES) {
      if ((nbr_fmt > 0)                                         // If fmt nbr > 0 ...
          || (i == 0u)) {                                       // ... OR on one's  dig to fmt (see Note #2d), ...
                                                                // ... calc & fmt dig val;
        dig_val = (CPU_INT08U)(nbr_fmt % nbr_base);
        if (dig_val < 10u) {
          *p_str_fmt-- = (CPU_CHAR)(dig_val + '0');
        } else {
          if (lower_case != DEF_YES) {
            *p_str_fmt-- = (CPU_CHAR)((dig_val - 10u) + 'A');
          } else {
            *p_str_fmt-- = (CPU_CHAR)((dig_val - 10u) + 'a');
          }
        }

        nbr_fmt /= nbr_base;                                    // Shift to next more-sig dig.
      } else if ((nbr_neg == DEF_YES)                           // ... else if nbr neg AND ...
                 && (((lead_char_0 == DEF_NO)                   // ... lead char NOT a '0' dig ...
                      && (nbr_neg_fmtd == DEF_NO))              // ... but neg sign NOT yet fmt'd OR ...
                     || ((lead_char_0 != DEF_NO)                // ... lead char is  a '0' dig ...
                         && (i == (nbr_dig_fmtd - 1u))))) {     // ... & on most-sig dig to fmt, ...
        *p_str_fmt-- = '-';                                     // ... prepend neg sign (see Note #2a); ...
        nbr_neg_fmtd = DEF_YES;
      } else if (lead_char != (CPU_CHAR)'\0') {                 // ... else if avail, ...
        *p_str_fmt-- = lead_char;                               // ... fmt lead char.
      }
    } else {                                                    // Else fmt '?' for invalid str (see Note #4).
      *p_str_fmt-- = '?';
    }
  }

  if (fmt_valid == DEF_NO) {                                    // Rtn NULL for invalid str fmt (see Notes #3a - #3e).
    return (DEF_NULL);
  }

  return (p_str);                                               // Rtn ptr to fmt'd str (see Note #3f).
}

/****************************************************************************************************//**
 *                                           Str_ParseNbr_Int32()
 *
 * @brief    Parses a 32-bit integer from a string.
 *
 * @param    p_str       Pointer to the string. String buffer NOT modified.
 *
 * @param    p_str_next  Optional pointer to a variable to :
 *                       Return a pointer to first character following the integer string, if NO
 *                       error(s);
 *                       Return a pointer to 'p_str', otherwise.
 *
 * @param    nbr_base    Base of number to parse.
 *
 * @param    nbr_signed  Indicates whether number to parse is signed :
 *                           - DEF_NO                  Number is unsigned.
 *                           - DEF_YES                 Number is signed.
 *
 * @param    p_nbr_neg   Pointer to the variable to return if the parsed (signed) number is negative :
 *                           - DEF_NO                  Number is non-negative.
 *                           - DEF_YES                 Number is negative.
 *
 * @return   Parsed integer, if integer parsed with NO overflow.
 *           DEF_INT_32U_MAX_VAL, if integer parsed but overflowed.
 *           0, otherwise.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'strtoul() : DESCRIPTION' states that "these
 *               functions shall convert the initial portion of the string pointed to by 'str'
 *               ('p_str') to a type unsigned long or long [...] representation" :
 *           - (a) "First, they decompose the input string into three parts" :
 *               - (1) "An initial, possibly empty, sequence of white-space characters [as specified
 *                       by isspace()]. The subject sequence is defined as the longest initial
 *                       subsequence of the input string, starting with the first non-white-space
 *                       character that is of the expected form. The subject sequence shall contain no
 *                       characters if the input string is empty or consists entirely of white-space
 *                       characters."
 *               - (2) "A subject sequence interpreted as an integer.  represented in some radix
 *                       determined by the value of 'base' ('nbr_base'). A subject sequence [...] may
 *                       be preceded by a '+' or '-' sign." However, it does NOT seem reasonable to
 *                       parse & convert a negative number integer string into an unsigned integer.
 *                   - (A) "If the value of 'base' ('nbr_base') is 0, the expected form of the
 *                           subject sequence is that of a decimal constant, octal constant, or
 *                           hexadecimal constant. [...] A decimal constant begins with a non-zero
 *                           digit, and consists of a sequence of decimal digits. [...] An octal
 *                           constant consists of the prefix '0' optionally followed by a sequence of
 *                           the digits '0' to '7' only. [...] A hexadecimal constant consists of the
 *                           prefix '0x' or '0X' followed by a sequence of the decimal digits and
 *                           letters 'a' (or 'A') to 'f' (or 'F') with values 10 to 15 respectively."
 *                   - (B) "If the value of 'base' ('nbr_base') is between 2 and 36, the expected
 *                           form of the subject sequence is a sequence of letters and digits
 *                           representing an integer with the radix specified by 'base' ('nbr_base').
 *                           [...] The letters from 'a' (or 'A') to 'z' (or 'Z') inclusive are
 *                           ascribed the values 10 to 35. [...] only letters whose ascribed values
 *                           are less than that of base are permitted. [...] If the value of 'base'
 *                           ('nbr_base') is 16, the characters '0x' or '0X' may optionally precede
 *                           the sequence of letters and digits." Although NO specification states
 *                           that "if the value of 'base' ('nbr_base') is" 8, the '0' character "may
 *                           optionally precede the sequence of letters and digits"; it seems
 *                           reasonable to allow the '0' character to be optionally parsed.
 *               - (3) "A final string of one or more unrecognized characters, [...] including the
 *                       terminating null byte of the input string [...] other than a sign or a
 *                       permissible letter or digit."
 *           - (b) Second, "they shall attempt to convert the subject sequence to [either an unsigned
 *                   integer or an integer]".
 *               - (1) "If the subject sequence is empty or does not have the expected form [...] no
 *                       conversion [is] performed [and] the value of 'str' ('p_str') [is] stored in
 *                       the object pointed to by 'endptr' ('p_str_next'), provided that 'endptr'
 *                       ('p_str_next') is not a null pointer."
 *               - (2) "If the subject sequence has the expected form [...] and the value of 'base'
 *                       ('nbr_base') is 0, the sequence of characters starting with the first digit
 *                       shall be interpreted as an integer constant. [If instead] the value of 'base'
 *                       ('nbr_base') is between 2 and 36, it shall be used as the base for
 *                       conversion, ascribing to each letter its value as given above."
 *                       "A pointer to the final string shall be stored in the object pointed to by
 *                       'endptr' ('p_str_next'), provided that 'endptr' ('p_str_next') is not a null
 *                       pointer."
 *           - (c) Lastly, IEEE Std 1003.1, 2004 Edition, Section 'strtoul() : RETURN VALUE' states
 *                   that :
 *                   "Upon successful completion, these functions shall return the converted value. If
 *                   the correct value is outside the range of representable values [either]
 *                   {LONG_MIN} [or] {LONG_MAX} [...] shall be returned. [...] If no conversion could
 *                   be performed, 0 shall be returned."
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'strtoul() : ERRORS' states that "these
 *               functions shall fail if [...] [EINVAL] - The value of 'base' ('nbr_base') is not
 *               supported [or if] [ERANGE] - The value to be returned is not representable."
 *
 * @note     (3) IEEE Std 1003.1, 2004 Edition, Section 'strtoul() : ERRORS' states that "these
 *               functions may fail if [...] [EINVAL] - No conversion could be performed."
 *
 * @note     (4) Return integer value and next string pointer should be used to diagnose parse success
 *               or failure :
 *           - (a) Valid parse string integer :
 *                       @verbatim
 *                       p_str      = "     ABCDE xyz"
 *                       nbr_base   = 16
 *
 *                       nbr        = 703710
 *                       p_str_next = " xyz"
 *                       @endverbatim
 *           - (b) Invalid parse string integer :
 *                       @verbatim
 *                       p_str      = "     ABCDE"
 *                       nbr_base   = 10
 *
 *                       nbr        =  0
 *                       p_str_next = p_str = "     ABCDE"
 *                       @endverbatim
 *           - (c) Valid hexadecimal parse string integer :
 *                       @verbatim
 *                       p_str      = "     0xGABCDE"
 *                       nbr_base   = 16
 *
 *                       nbr        =  0
 *                       p_str_next = "xGABCDE"
 *                       @endverbatim
 *           - (d) Valid decimal parse string integer ('0x' prefix ignored
 *                           following invalid hexadecimal characters) :
 *                       @verbatim
 *                       p_str      = "     0xGABCDE"
 *                       nbr_base   =  0
 *
 *                       nbr        =  0
 *                       p_str_next = "xGABCDE"
 *                       @endverbatim
 *           - (e) Valid decimal parse string integer ('0'  prefix ignored
 *                           following invalid octal       characters) :
 *                       @verbatim
 *                       p_str      = "     0GABCDE"
 *                       nbr_base   =  0
 *
 *                       nbr        =  0
 *                       p_str_next = "GABCDE"
 *                       @endverbatim
 *           - (f) Parse string integer overflow :
 *                       @verbatim
 *                       p_str      = "   12345678901234567890*123456"
 *                       nbr_base   = 10
 *
 *                       nbr        = DEF_INT_32U_MAX_VAL
 *                       p_str_next = "*123456"
 *                       @endverbatim
 *           - (g) Parse string integer underflow :
 *                       @verbatim
 *                       p_str      = "  -12345678901234567890*123456"
 *                       nbr_base   = 10
 *
 *                       nbr        = DEF_INT_32S_MIN_VAL
 *                       p_str_next = "*123456"
 *                       @endverbatim
 *
 * @note     (5) String parse terminates when :
 *           - (a) Base passed an invalid base (see Note #1a2B). No conversion performed; 0
 *                   returned.
 *           - (b) Parse string passed a NULL pointer OR empty integer sequence (see Note #1b1).
 *                   No conversion performed; 0 returned.
 *           - (c) Invalid parse string character found (see Note #1a3). Parsed integer returned.
 *                   'p_str_next' points to invalid character.
 *           - (d) Entire parse string converted (see Note #1b2). Parsed integer returned.
 *                   'p_str_next' points to terminating NULL character.
 *******************************************************************************************************/
static CPU_INT32U Str_ParseNbr_Int32(const CPU_CHAR *p_str,
                                     CPU_CHAR       **p_str_next,
                                     CPU_INT08U     nbr_base,
                                     CPU_BOOLEAN    nbr_signed,
                                     CPU_BOOLEAN    *p_nbr_neg)
{
  const CPU_CHAR *p_str_parse;
  const CPU_CHAR *p_str_parse_nbr;
  CPU_CHAR       *p_str_parse_unused;
  CPU_CHAR       parse_char;
  CPU_INT08U     parse_dig;
  CPU_INT32U     nbr;
  CPU_BOOLEAN    nbr_neg_unused;
  CPU_BOOLEAN    nbr_dig;
  CPU_BOOLEAN    nbr_alpha;
  CPU_BOOLEAN    nbr_hex;
  CPU_BOOLEAN    nbr_hex_lower;
  CPU_BOOLEAN    whitespace;
  CPU_BOOLEAN    neg;
  CPU_BOOLEAN    ovf;
  CPU_BOOLEAN    done;

  //                                                               --------------- VALIDATE PARSE ARGS ----------------
  if (p_str_next == DEF_NULL) {                                 // If NOT avail, ...
    p_str_next = (CPU_CHAR **)&p_str_parse_unused;              // ... re-cfg NULL rtn ptr to unused local var.
    PP_UNUSED_PARAM(p_str_parse_unused);
  }
  *p_str_next = (CPU_CHAR *)p_str;                              // Init rtn str in case of err.

  if (p_nbr_neg == DEF_NULL) {                                  // If NOT avail, ...
    p_nbr_neg = (CPU_BOOLEAN *)&nbr_neg_unused;                 // ... re-cfg NULL rtn ptr to unused local var.
    PP_UNUSED_PARAM(nbr_neg_unused);
  }
  *p_nbr_neg = DEF_NO;                                          // Init nbr neg in case of err.

  if (p_str == DEF_NULL) {                                      // Rtn zero if str ptr NULL (see Note #5b).
    return (0u);
  }
  //                                                               Rtn zero if invalid base (see Note #5a).
  if ((nbr_base == 1u)
      || (nbr_base > 36u)) {
    return (0u);
  }

  //                                                               ------------- IGNORE PRECEDING CHAR(S) -------------
  p_str_parse = p_str;                                          // Save ptr to init'l str for err (see Note #1b1).

  whitespace = ASCII_IsSpace(*p_str_parse);
  while (whitespace == DEF_YES) {                               // Ignore initial white-space char(s) (see Note #1a1).
    p_str_parse++;
    whitespace = ASCII_IsSpace(*p_str_parse);
  }

  switch (*p_str_parse) {
    case '+':                                                   // Ignore pos sign (see Note #1a2).
      p_str_parse++;
      neg = DEF_NO;
      break;

    case '-':                                                   // Validate neg sign (see Note #1a2).
      if (nbr_signed == DEF_YES) {
        p_str_parse++;
      }
      neg = DEF_YES;
      break;

    default:
      neg = DEF_NO;
      break;
  }

  //                                                               --------- IGNORE NBR BASE PRECEDING CHAR(S) --------
  p_str_parse_nbr = p_str_parse;                                // Save ptr to str's nbr (see Note #1a1).

  switch (nbr_base) {
    case  0u:                                                   // Determine unspecified nbr base (see Notes #1a2A).
      if (*p_str_parse == '0') {                                // If avail, ...
        p_str_parse++;                                          // ... adv past '0' prefix (see Note #1a2B).
        switch (*p_str_parse) {
          case 'x':                                             // For '0x' prefix, ...
          case 'X':
            nbr_base = 16u;                                     // ... set nbr base = 16 (see Note #1a2A).
            parse_char = (CPU_CHAR)(*(p_str_parse + 1));
            nbr_hex = ASCII_IsDigHex(parse_char);
            if (nbr_hex == DEF_YES) {                           // If next char is valid hex dig, ...
              p_str_parse++;                                    // ... adv past '0x' prefix (see Note #1a2B).
            }
            break;

          default:                                              // For '0'  prefix, ...
            nbr_base = 8u;                                      // ... set nbr base =  8 (see Note #1a2A).
            break;
        }
      } else {                                                  // For non-'0' prefix, ...
        nbr_base = 10u;                                         // ... set nbr base = 10 (see Note #1a2A).
      }
      break;

    case  8u:                                                   // See Note #1a2A.
      if (*p_str_parse == '0') {                                // If avail, ...
        p_str_parse++;                                          // ... adv past '0' prefix (see Note #1a2B).
      }
      break;

    case 16u:                                                   // See Note #1a2A.
      if (*p_str_parse == '0') {                                // If avail, ...
        p_str_parse++;                                          // ... adv past '0'  prefix (see Note #1a2B).
        switch (*p_str_parse) {
          case 'x':
          case 'X':
            parse_char = (CPU_CHAR)(*(p_str_parse + 1));
            nbr_hex = ASCII_IsDigHex(parse_char);
            if (nbr_hex == DEF_YES) {                           // If next char is valid hex dig, ...
              p_str_parse++;                                    // ... adv past '0x' prefix (see Note #1a2B).
            }
            break;

          default:
            break;
        }
      }
      break;

    default:                                                    // See Note #1a2B.
      break;
  }

  //                                                               ------------------ PARSE INT STR -------------------
  nbr = 0u;
  ovf = DEF_NO;
  done = DEF_NO;

  while (done == DEF_NO) {                                      // Parse str for desired nbr base digs (see Note #1b).
    parse_char = (CPU_CHAR)*p_str_parse;
    nbr_alpha = ASCII_IsAlphaNum(parse_char);
    if (nbr_alpha == DEF_YES) {                                 // If valid alpha num nbr dig avail, ...
                                                                // ... convert parse char into nbr dig.
      nbr_dig = ASCII_IsDig(parse_char);
      if (nbr_dig == DEF_YES) {
        parse_dig = (CPU_INT08U)(parse_char - '0');
      } else {
        nbr_hex_lower = ASCII_IsLower(parse_char);
        if (nbr_hex_lower == DEF_YES) {
          parse_dig = ((CPU_INT08U)(parse_char - 'a') + 10u);
        } else {
          parse_dig = ((CPU_INT08U)(parse_char - 'A') + 10u);
        }
      }

      if (parse_dig < nbr_base) {                               // If parse char valid for nbr base ...
        if (ovf == DEF_NO) {                                    // ... & nbr NOT yet ovf'd, ...
          if (nbr <= Str_MultOvfThTbl_Int32U[nbr_base]) {
            //                                                     ... merge parse char dig into nbr.
            nbr *= nbr_base;
            nbr += parse_dig;
            if (nbr < parse_dig) {
              ovf = DEF_YES;
            }
          } else {
            ovf = DEF_YES;
          }
        }
        p_str_parse++;
      } else {                                                  // Invalid char parsed (see Note #1a3).
        done = DEF_YES;
      }
    } else {                                                    // Invalid OR NULL char parsed (see Note #1a3).
      done = DEF_YES;
    }
  }

  if (ovf == DEF_YES) {                                         // If nbr ovf'd, ...
    nbr = DEF_INT_32U_MAX_VAL;                                  // ... rtn max int val (see Note #1c).
  }

  if (p_str_parse != p_str_parse_nbr) {                         // If final parse str != init'l parse nbr str, ...
    *p_str_next = (CPU_CHAR *)p_str_parse;                      // ... rtn parse str's next char (see Note #1b2); ...
  } else {
    *p_str_next = (CPU_CHAR *)p_str;                            // ... else rtn initial parse str (see Note #1b1).
  }

  *p_nbr_neg = neg;                                             // Rtn neg nbr status.

  return (nbr);
}
