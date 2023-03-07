/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server String Module
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Server Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "http_server_priv.h"

#include  <common/include/lib_str.h>
#include  <common/include/lib_ascii.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           HTTPs_StrPathFormat()
 *
 * @brief    (1) Format file path:
 *               - (a) Copy folder    in path destination buffer.
 *               - (b) Copy file name in path destination buffer.
 *               - (c) Replace path separator character.
 *
 * @param    p_filename      Pointer to file name string.
 *
 * @param    ----------      Argument validated in HTTPsSock_ConnAccept().
 *
 * @param    p_folder        Pointer to the folder string.
 *
 * @param    --------        Argument validated in HTTPsSock_ConnAccept().
 *
 * @param    p_path_dst      Pointer to string buffer where to format the path.
 *
 * @param    ----------      Argument validated in HTTPsSock_ConnAccept().
 *
 * @param    path_len_max    Maximum length of the path.
 *
 * @param    path_sep        Path character separator.
 *
 * @return   DEF_OK,   path successfully formated.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
CPU_BOOLEAN HTTPs_StrPathFormat(CPU_CHAR   *p_filename,
                                CPU_CHAR   *p_folder,
                                CPU_CHAR   *p_path_dst,
                                CPU_SIZE_T path_len_max,
                                CPU_CHAR   path_sep)
{
  CPU_CHAR   *p_str_file;
  CPU_CHAR   *p_str_folder;
  CPU_CHAR   *p_str;
  CPU_CHAR   *p_dst;
  CPU_SIZE_T len_folder;
  CPU_SIZE_T len;

  p_str_file = p_filename;
  p_dst = p_path_dst;
  len = path_len_max;

  //                                                               ----------- COPY FOLDER IN PATH DST BUF ------------
  if (p_folder != DEF_NULL) {                                   // Only if folder is not null.
    p_str_folder = p_folder;
    while ((*p_str_folder == ASCII_CHAR_SOLIDUS)                // Must NOT start with a path sep.
           || (*p_str_folder == ASCII_CHAR_REVERSE_SOLIDUS)) {
      p_str_folder++;
    }

    (void)Str_Copy_N(p_dst, p_str_folder, path_len_max);        // Copy folder to the dst.
    len_folder = Str_Len_N(p_str_folder, path_len_max);

    if (len_folder > 0) {                                       // Verify that folder name is not empty.
      p_dst += (len_folder - 1);                                // Verify if folder name end with path sep.
      len -= (len_folder - 1);
      //                                                           Must have a path sep before filename.
      if ((*p_dst == ASCII_CHAR_SOLIDUS)                        // Verify if folder name end with path sep.
          || (*p_dst == ASCII_CHAR_REVERSE_SOLIDUS)) {
        if (len_folder > 1) {
          p_dst++;
          len--;
        }
      } else {                                                  // Add a path sep. at end of folder name.
        p_dst++;
        *p_dst = ASCII_CHAR_REVERSE_SOLIDUS;
        p_dst++;
        len -= 2;
      }
    }
  }

  //                                                               ---------- COPY FILE NAME IN PATH DST BUF ----------
  while ((*p_str_file == ASCII_CHAR_SOLIDUS)                    // Must NOT start with a path sep.
         || (*p_str_file == ASCII_CHAR_REVERSE_SOLIDUS)) {
    p_str_file++;
  }

  (void)Str_Copy_N(p_dst, p_str_file, path_len_max);            // Copy file name.

  //                                                               --------- REPLACE PATH SEP IN PATH DST BUF ---------
  if (path_sep != ASCII_CHAR_SOLIDUS) {
    p_str = Str_Char_Replace_N(p_dst,
                               ASCII_CHAR_SOLIDUS,
                               path_sep,
                               len);
    if (p_str == DEF_NULL) {
      return (DEF_FAIL);
    }
  }

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                           HTTPs_StrFilePathGet()
 *
 * @brief    Retrieve file path in a file path that contains the host in the path.
 *
 * @param    p_path              Pointer to the path that can contains the host.
 *
 * @param    path_len_max        Path length maximum.
 *
 * @param    p_host              Pointer to a string that contains the host name.
 *
 * @param    host_len_max        Host string length maximum.
 *
 * @param    p_resp_location     Pointer to a variable that will be set if the location field header should be send.
 *
 * @return   Pointer to the file path.
 *******************************************************************************************************/
#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
CPU_CHAR *HTTPs_StrPathGet(CPU_CHAR    *p_path,
                           CPU_INT16U  path_len_max,
                           CPU_CHAR    *p_host,
                           CPU_INT16U  host_len_max,
                           CPU_BOOLEAN *p_resp_location)
{
  CPU_CHAR   *p_file_path;
  CPU_INT32U host_len;

  p_file_path = Str_Str_N(p_path, p_host, path_len_max);
  if (p_file_path != DEF_NULL) {
    host_len = Str_Len_N(p_host, host_len_max);
    p_file_path += host_len;
    if (p_resp_location != DEF_NULL) {
      *p_resp_location = DEF_YES;
    }
  } else {
    p_file_path = p_path;
  }

  return (p_file_path);
}
#endif

/****************************************************************************************************//**
 *                                           HTTPs_StrMemSrch()
 *
 * @brief    Search for a string into a memory section.
 *
 * @param    p_data      Pointer to a buffer that may contain the boundary token.
 *
 * @param    data_len    Length of the buffer.
 *
 * @param    p_str       Pointer to string.
 *
 * @param    str_len     String length.
 *
 * @return   Pointer to the beginning of boundary token, if found,
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
CPU_CHAR *HTTPs_StrMemSrch(const CPU_CHAR *p_data,
                           CPU_INT32U     data_len,
                           const CPU_CHAR *p_str,
                           CPU_INT32U     str_len)
{
  CPU_CHAR    *p_search;
  CPU_INT32S  search_len;
  CPU_INT32S  i;
  CPU_BOOLEAN cmp_identical;

  //                                                               ----------------- INPUT ARG CHECK ------------------
  if (p_data == DEF_NULL) {
    return (DEF_NULL);
  }

  if (p_str == DEF_NULL) {
    return (DEF_NULL);
  }

  if (data_len < 1) {
    return (DEF_NULL);
  }

  if (str_len < 1) {
    return (DEF_NULL);
  }

  if (data_len < str_len) {
    return (DEF_NULL);
  }

  //                                                               ------------------ MEM SEARCH-CMP ------------------
  p_search = (CPU_CHAR *)p_data;
  search_len = (CPU_INT32S)(data_len - str_len);
  for (i = search_len; i >= 0; i--) {
    cmp_identical = Mem_Cmp(p_search, p_str, str_len);
    if (cmp_identical == DEF_YES) {
      return (p_search);
    }

    p_search++;
  }

  return (DEF_NULL);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
