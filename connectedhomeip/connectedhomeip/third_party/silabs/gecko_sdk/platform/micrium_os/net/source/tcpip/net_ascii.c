/***************************************************************************//**
 * @file
 * @brief Network Ascii Library
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

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_cfg_net.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_util.h>

#include  "net_ascii_priv.h"

#ifdef  NET_IPv4_MODULE_EN
#include  "net_ipv4_priv.h"
#include  <net/include/net_ipv4.h>
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  "net_ipv6_priv.h"
#endif

#include  <cpu/include/cpu.h>
#include  <common/include/lib_ascii.h>
#include  <common/include/lib_str.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetASCII_Str_to_MAC()
 *
 * @brief    Converts an Ethernet MAC address ASCII string to an Ethernet MAC address.
 *
 * @param    p_addr_mac_ascii    Pointer to an ASCII string that contains a MAC address.
 *
 * @param    p_addr_mac          Pointer to an ASCII string that contains a MAC address.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NET_STR_ADDR_INVALID
 *
 * @note     (1) MAC Address representation
 *               - (a) RFC #1700, Section 'ETHERNET VENDOR ADDRESS COMPONENTS'
 *                     - (1) states that "Ethernet addresses ... should be written hyphenated by octets
 *                           (e.g., 12-34-56-78-9A-BC)".
 *                     - (2) In other words, the (Ethernet) MAC address notation separates six hexadecimal
 *                           octet values by the hyphen character ('-') or by the colon character (':').
 *                           Each hexadecimal value represents one octet of the MAC address starting with
 *                           the most significant octet in network-order.
 *                           @n
 *                           MAC Address Examples : "00-1A-07-AC-22-09" = 0x001A07AC2209
 *               - (b) Therefore, the MAC address ASCII string MUST :
 *                     - (1) Include ONLY hexadecimal values & the hyphen character ('-') or the colon
 *                           character (':') ; ALL other characters are trapped as invalid, including
 *                           any leading or trailing characters.
 *                     - (2) Include EXACTLY six hexadecimal values separated by EXACTLY five hyphen
 *                           characters or colon  characters & MUST be terminated with the NULL character.
 *                     - (3) Ensure that each hexadecimal value's number of digits does NOT exceed the
 *                           maximum number of digits (2).
 *                           - (A) However, any hexadecimal value's number of digits MAY be less than the
 *                                 maximum number of digits.
 *
 * @note     (2) The size of the memory buffer that will receive the converted MAC address MUST
 *               be greater than or equal to NET_ASCII_NBR_OCTET_ADDR_MAC.
 *               MAC address memory buffer array accessed by octets.
 *               MAC address memory buffer array cleared in case of any error(s).
 *******************************************************************************************************/
void NetASCII_Str_to_MAC(CPU_CHAR   *p_addr_mac_ascii,
                         CPU_INT08U *p_addr_mac,
                         RTOS_ERR   *p_err)
{
  CPU_CHAR   *p_char_cur;
  CPU_CHAR   *p_char_prev;
  CPU_INT08U *p_addr_cur;
  CPU_INT08U addr_octet_val;
  CPU_INT08U addr_octet_val_dig;
  CPU_INT08U addr_nbr_octet;
  CPU_INT08U addr_nbr_octet_dig;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_mac != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (p_addr_mac_ascii == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    return;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------- CONVERT MAC ADDR STR -----------
  p_char_cur = p_addr_mac_ascii;
  p_char_prev = DEF_NULL;
  p_addr_cur = p_addr_mac;
  addr_octet_val = 0x00u;
  addr_nbr_octet = 0u;
  addr_nbr_octet_dig = 0u;

  while ((p_char_cur != DEF_NULL)                                       // Parse ALL non-NULL chars in ASCII str.
         && (*p_char_cur != '\0')    ) {
    switch (*p_char_cur) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
        addr_nbr_octet_dig++;                                           // If nbr digs > max (see Note #1b3); ...
        if (addr_nbr_octet_dig > NET_ASCII_CHAR_MAX_OCTET_ADDR_MAC) {
          Mem_Clr(p_addr_mac,                                           // ... clr rtn addr  (see Note #2c)   ...
                  NET_ASCII_NBR_OCTET_ADDR_MAC);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return;
        }

        switch (*p_char_cur) {                                          // Convert ASCII char into hex val.
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            addr_octet_val_dig = (CPU_INT08U) (*p_char_cur - '0');
            break;

          case 'A':
          case 'B':
          case 'C':
          case 'D':
          case 'E':
          case 'F':
            addr_octet_val_dig = (CPU_INT08U)((*p_char_cur - 'A') + 10u);
            break;

          case 'a':
          case 'b':
          case 'c':
          case 'd':
          case 'e':
          case 'f':
            addr_octet_val_dig = (CPU_INT08U)((*p_char_cur - 'a') + 10u);
            break;

          default:                                                      // See Note #1b1.
            Mem_Clr(p_addr_mac,                                         // Clr rtn addr on err (see Note #2c).
                    NET_ASCII_NBR_OCTET_ADDR_MAC);
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
            return;
        }
        //                                                         Merge ASCII char into hex val.
        addr_octet_val <<= DEF_NIBBLE_NBR_BITS;
        addr_octet_val += addr_octet_val_dig;
        break;

      case '-':
      case ':':
        if (p_char_prev == DEF_NULL) {                                  // If NO prev char  (see Note #1b2B1); ...
          Mem_Clr(p_addr_mac,                                           // ... clr rtn addr (see Note #2c)     ...
                  NET_ASCII_NBR_OCTET_ADDR_MAC);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return;
        }

        if ((*p_char_prev == '-')                                       // If prev char a hyphen                   ...
            || (*p_char_prev == ':')) {                                 // ... or       a colon (see Note #1b2B1); ...
          Mem_Clr(p_addr_mac,                                           // ... clr rtn addr     (see Note #2c)     ...
                  NET_ASCII_NBR_OCTET_ADDR_MAC);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return;
        }

        addr_nbr_octet++;
        if (addr_nbr_octet >= NET_ASCII_NBR_OCTET_ADDR_MAC) {           // If nbr octets > max (see Note #1b2A); ...
          Mem_Clr(p_addr_mac,                                           // ... clr rtn addr    (see Note #2c)    ...
                  NET_ASCII_NBR_OCTET_ADDR_MAC);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return;
        }

        //                                                         Merge hex octet val into MAC addr.
        *p_addr_cur++ = addr_octet_val;
        addr_octet_val = 0x00u;
        addr_nbr_octet_dig = 0u;
        break;

      default:                                                          // See Note #1b1.
        Mem_Clr(p_addr_mac,                                             // Clr rtn addr on err (see Note #2c).
                NET_ASCII_NBR_OCTET_ADDR_MAC);
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
        return;
    }

    p_char_prev = p_char_cur;
    p_char_cur++;
  }

  if (p_char_cur == DEF_NULL) {                                         // If NULL ptr in ASCII str (see Note #1b1); ..
    Mem_Clr(p_addr_mac,                                                 // .. clr rtn addr          (see Note #2c).
            NET_ASCII_NBR_OCTET_ADDR_MAC);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return;
  }

  if (p_char_prev == DEF_NULL) {                                        // If NULL        ASCII str (see Note #1b2); ..
    Mem_Clr(p_addr_mac,                                                 // .. clr rtn addr          (see Note #2c).
            NET_ASCII_NBR_OCTET_ADDR_MAC);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return;
  }

  if ((*p_char_prev == '-')                                             // If last char a hyphen                   ..
      || (*p_char_prev == ':')) {                                       // .. or        a colon (see Note #1b2B1); ..
    Mem_Clr(p_addr_mac,                                                 // .. clr rtn addr      (see Note #2c).
            NET_ASCII_NBR_OCTET_ADDR_MAC);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return;
  }

  addr_nbr_octet++;
  if (addr_nbr_octet != NET_ASCII_NBR_OCTET_ADDR_MAC) {                 // If != nbr MAC addr octets (see Note #1b2A);
    Mem_Clr(p_addr_mac,                                                 // .. clr rtn addr           (see Note #2c).
            NET_ASCII_NBR_OCTET_ADDR_MAC);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return;
  }

  //                                                               Merge hex octet val into final MAC addr.
  *p_addr_cur = addr_octet_val;
}

/****************************************************************************************************//**
 *                                           NetASCII_MAC_to_Str()
 *
 * @brief    Convert an Ethernet MAC address into an Ethernet MAC address ASCII string.
 *
 * @param    p_addr_mac          Pointer to a memory buffer that contains the MAC address
 *                               (see Note #2).
 *
 * @param    p_addr_mac_ascii    Pointer to an ASCII character array that will receive the return MAC
 *
 * @param    hex_lower_case      Format alphabetic hexadecimal characters in lower case :
 *
 * @param    hex_colon_sep       Separate hexadecimal values with a colon character :
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *
 * @note     (1) MAC Address representation
 *               - (a) RFC #1700, Section 'ETHERNET VENDOR ADDRESS COMPONENTS'
 *                   - (1) states that "Ethernet addresses ... should be written hyphenated by octets
 *                         (e.g., 12-34-56-78-9A-BC)".
 *                   - (2) In other words, the (Ethernet) MAC address notation separates six hexadecimal
 *                         octet values by the hyphen character ('-') or by the colon character (':').
 *                         Each hexadecimal value represents one octet of the MAC address starting with
 *                         the most significant octet in network-order.
 *                         @n
 *                         MAC Address Examples : "00-1A-07-AC-22-09" = 0x001A07AC2209
 *               - (b) Output string:
 *                   - (1) The return MAC address ASCII string formats EXACTLY six hexadecimal values
 *                         separated by EXACTLY five hyphen characters or colon characters & terminated
 *                         with the NULL character.
 *                   - (2) The size of the ASCII character array that will receive the returned MAC address
 *                         ASCII string MUST be greater than or equal to NET_ASCII_LEN_MAX_ADDR_MAC.
 *
 * @note     (2) The size of the memory buffer that contains the MAC address SHOULD be greater than
 *               or equal to NET_ASCII_NBR_OCTET_ADDR_MAC.
 *               MAC address memory buffer array accessed by octets.
 *******************************************************************************************************/
void NetASCII_MAC_to_Str(CPU_INT08U  *p_addr_mac,
                         CPU_CHAR    *p_addr_mac_ascii,
                         CPU_BOOLEAN hex_lower_case,
                         CPU_BOOLEAN hex_colon_sep,
                         RTOS_ERR    *p_err)
{
  CPU_CHAR   *p_char;
  CPU_INT08U *p_addr;
  CPU_INT08U addr_octet_val;
  CPU_INT08U addr_octet_dig_val;
  CPU_INT08U addr_octet_nbr_shifts;
  CPU_INT08U i;
  CPU_INT08U j;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_mac != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_mac_ascii != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- CONVERT MAC ADDR -------------
  p_addr = p_addr_mac;
  p_char = p_addr_mac_ascii;

  for (i = NET_ASCII_NBR_OCTET_ADDR_MAC; i > 0u; i--) {                 // Parse ALL addr octets (see Note #1b1A).
    addr_octet_val = *p_addr;

    for (j = NET_ASCII_CHAR_MAX_OCTET_ADDR_MAC; j > 0u; j--) {          // Parse ALL octet's hex digs.
                                                                        // Calc  cur octet's hex dig val.
      addr_octet_nbr_shifts = (j - 1u) * DEF_NIBBLE_NBR_BITS;
      addr_octet_dig_val = (addr_octet_val >> addr_octet_nbr_shifts) & DEF_NIBBLE_MASK;
      //                                                           Insert    octet's hex val ASCII dig.
      if (addr_octet_dig_val < 10u) {
        *p_char++ = (CPU_CHAR)(addr_octet_dig_val + '0');
      } else {
        if (hex_lower_case != DEF_YES) {
          *p_char++ = (CPU_CHAR)((addr_octet_dig_val - 10u) + 'A');
        } else {
          *p_char++ = (CPU_CHAR)((addr_octet_dig_val - 10u) + 'a');
        }
      }
    }

    if (i > 1u) {                                                       // If NOT on last octet,                    ..
      if (hex_colon_sep != DEF_YES) {
        *p_char++ = '-';                                                // .. insert hyphen char (see Note #1b1B2a) ..
      } else {
        *p_char++ = ':';                                                // ..     or colon  char (see Note #1b1B2b).
      }
    }

    p_addr++;
  }

  *p_char = '\0';                                                       // Append NULL char (see Note #1b1C).
}

/****************************************************************************************************//**
 *                                           NetASCII_Str_to_IP()
 *
 * @brief    Convert string representation of IP address (IPv4 or IPv6) to their TCP/IP stack intern
 *           representation.
 *
 * @param    p_addr_ip_ascii     Pointer to an ASCII string that contains a decimal IP address.
 *
 * @param    p_addr              _ip_ascii     Pointer to an ASCII string that contains a decimal IP address.
 *
 * @param    addr_max_len        Size of the variable that will received the converted IP address:
 *                                   - NET_IPv4_ADDR_SIZE
 *                                   - NET_IPv6_ADDR_SIZE
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NET_STR_ADDR_INVALID
 *
 * @return   the IP family of the converted address, if no errors:
 *               - NET_IP_ADDR_FAMILY_IPv4
 *               - NET_IP_ADDR_FAMILY_IPv6; otherwise,
 *               - NET_IP_ADDR_FAMILY_UNKNOWN
 *******************************************************************************************************/
NET_IP_ADDR_FAMILY NetASCII_Str_to_IP(CPU_CHAR   *p_addr_ip_ascii,
                                      void       *p_addr,
                                      CPU_INT08U addr_max_len,
                                      RTOS_ERR   *p_err)
{
  NET_IPv4_ADDR      *p_addr_ipv4;
  NET_IPv6_ADDR      *p_addr_ipv6;
  CPU_CHAR           *p_sep;
  NET_IP_ADDR_FAMILY addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
  CPU_INT16S         result;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IP_ADDR_FAMILY_UNKNOWN);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_ip_ascii != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IP_ADDR_FAMILY_UNKNOWN);
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IP_ADDR_FAMILY_UNKNOWN);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               "localhost" string verfication.
  result = Str_Cmp(p_addr_ip_ascii, NET_ASCII_STR_LOCAL_HOST);
  if (result == 0) {
#ifdef NET_IPv4_MODULE_EN
    addr_family = NET_IP_ADDR_FAMILY_IPv4;                      // By default, return IPv4 local host address.
    p_addr_ipv4 = (NET_IPv4_ADDR *)p_addr;
    *p_addr_ipv4 = NET_IPv4_ADDR_LOCAL_HOST_ADDR;
#else
    addr_family = NET_IP_ADDR_FAMILY_IPv6;                      // If IPv4 is not enabled, return IPv6 loopback address.
    p_addr_ipv6 = (NET_IPv6_ADDR *)p_addr;
    *p_addr_ipv6 = NetIPv6_AddrLoopback;
#endif
    goto exit;
  }
  //                                                               ----------- DETERMINE IP ADDRESS FAMILY ------------
  p_sep = Str_Char(p_addr_ip_ascii, ASCII_CHAR_FULL_STOP);      // Search for IPv4 separator '.'
  if (p_sep != DEF_NULL) {
    addr_family = NET_IP_ADDR_FAMILY_IPv4;
  } else {
    p_sep = Str_Char(p_addr_ip_ascii, ASCII_CHAR_COLON);        // Search for IPv6 separator ':'
    if (p_sep != DEF_NULL) {
      addr_family = NET_IP_ADDR_FAMILY_IPv6;
    } else {
      addr_family = NET_IP_ADDR_FAMILY_UNKNOWN;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
      return(NET_IP_ADDR_FAMILY_UNKNOWN);
    }
  }

  switch (addr_family) {
    case NET_IP_ADDR_FAMILY_IPv4:
      p_addr_ipv4 = (NET_IPv4_ADDR *)p_addr;
      *p_addr_ipv4 = NetASCII_Str_to_IPv4(p_addr_ip_ascii, p_err);
      break;

    case NET_IP_ADDR_FAMILY_IPv6:
      p_addr_ipv6 = (NET_IPv6_ADDR *)p_addr;
      *p_addr_ipv6 = NetASCII_Str_to_IPv6(p_addr_ip_ascii, p_err);
      break;

    case NET_IP_ADDR_FAMILY_UNKNOWN:
    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, NET_IP_ADDR_FAMILY_UNKNOWN);
  }

  PP_UNUSED_PARAM(addr_max_len);

exit:
  return (addr_family);
}

/****************************************************************************************************//**
 *                                           NetASCII_Str_to_IPv4()
 *
 * @brief    Convert an IPv4 address ASCII string in dotted-decimal notation to a network protocol
 *           IPv4 address in host-order.
 *
 * @param    p_addr_ip_ascii     Pointer to an ASCII string that contains a dotted-decimal IPv4 address
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *
 * @return   Host-order IPv4 address represented by ASCII string, if NO error(s).
 *           NET_IPv4_ADDR_NONE, otherwise.
 *
 * @note     (1) The dotted-decimal IP address ASCII string MUST :
 *               - (a) Include ONLY decimal values & the dot, or period, character ('.') ; ALL other
 *                     characters are trapped as invalid, including any leading or trailing characters.
 *               - (b) Include UP TO four decimal values separated by UP TO three dot characters
 *                     & MUST be terminated with the NULL character.
 *               - (c) Ensure that each decimal value's number of decimal digits, including leading
 *                     zeros, does NOT exceed      the maximum number of digits (10).
 *                   - (1) However, any decimal value's number of decimal digits, including leading
 *                         zeros, MAY be less than the maximum number of digits.
 *               - (d) Ensure that each decimal value does NOT exceed the maximum value for its form:
 *                     @verbatim
 *                     - (1) a.b.c.d - 255.255.255.255
 *                     - (2) a.b.c   - 255.255.65535
 *                     - (3) a.b     - 255.16777215
 *                     - (4) a       - 4294967295
 *                     @endverbatim
 *
 * @note     (2) To avoid possible integer arithmetic overflow, the IP address octet arithmetic result
 *               MUST be declared as an integer data type with a greater resolution -- i.e. greater
 *               number of bits -- than the IP address octet data type(s).
 *******************************************************************************************************/
NET_IPv4_ADDR NetASCII_Str_to_IPv4(CPU_CHAR *p_addr_ip_ascii,
                                   RTOS_ERR *p_err)
{
  NET_IPv4_ADDR ip_addr = NET_IPv4_ADDR_NONE;
  CPU_INT08U    dot_nbr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, NET_IPv4_ADDR_NONE);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_ip_ascii != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, NET_IPv4_ADDR_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  ip_addr = NetASCII_Str_to_IPv4_Handler(p_addr_ip_ascii,
                                         &dot_nbr,
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_IPv4_ADDR_NONE);
  }

  if (dot_nbr != NET_ASCII_NBR_MAX_DOT_ADDR_IP) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(NET_IPv4_ADDR_NONE);
  }

  return (ip_addr);
}

/****************************************************************************************************//**
 *                                           NetASCII_Str_to_IPv6()
 *
 * @brief    Convert an IPv6 address ASCII string in Common-decimal notation to a network protocol
 *               IPv6 address in host-order.
 *
 * @param    p_addr_ip_ascii     Pointer to an ASCII string that contains a common-decimal IPv6 address
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NET_STR_ADDR_INVALID
 *
 * @return   Host-order IPv6 address represented by ASCII string, if NO error(s).
 *           NET_IPv6_ADDR_NONE, otherwise.
 *******************************************************************************************************/
NET_IPv6_ADDR NetASCII_Str_to_IPv6(CPU_CHAR *p_addr_ip_ascii,
                                   RTOS_ERR *p_err)
{
  NET_IPv6_ADDR addr_ip;
  CPU_CHAR      *p_char_cur;
  CPU_CHAR      *p_char_next;
  CPU_CHAR      *p_char_prev;
  CPU_INT08U    addr_nbr_octet;
  CPU_INT08U    addr_nbr_colon_cur;
  CPU_INT08U    addr_nbr_colon_rem;
  CPU_INT08U    addr_nbr_colon_tot;
  CPU_BOOLEAN   addr_msb_octet;
  CPU_INT08U    addr_nbr_octet_dig;
  CPU_INT08U    addr_octet_val_dig;
  CPU_INT08U    addr_nbr_lead_zero;
  CPU_INT08U    addr_nbr_octet_zero;
  CPU_INT08U    addr_nbr_dig_end;
  CPU_INT08U    nbr_octet_zero_cnt;

  NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, addr_ip);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_ip_ascii != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, addr_ip);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------- CONVERT IP ADDR STR ------------
  p_char_cur = p_addr_ip_ascii;
  p_char_prev = DEF_NULL;
  p_char_next = DEF_NULL;
  addr_nbr_octet = 0u;
  addr_nbr_colon_cur = 0u;
  addr_nbr_colon_rem = 0u;
  addr_nbr_colon_tot = 0u;
  addr_nbr_octet_dig = 0u;
  addr_octet_val_dig = 0u;
  addr_nbr_lead_zero = 0u;
  addr_nbr_octet_zero = 0u;
  addr_nbr_dig_end = 0u;
  addr_msb_octet = DEF_YES;
  nbr_octet_zero_cnt = 0u;

  while ((p_char_cur != DEF_NULL)                                       // Parse ALL non-NULL chars in ASCII str.
         && (*p_char_cur != '\0')    ) {
    p_char_next = p_char_cur + 1u;
    switch (*p_char_cur) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
        addr_nbr_octet_dig++;                                           // If nbr digs > max (see Note #1b3), ...
        if (addr_nbr_octet_dig > NET_ASCII_CHAR_MAX_DIG_ADDR_IPv6) {
          NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(addr_ip);
        }

        switch (*p_char_cur) {                                          // Convert ASCII char into hex val.
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            addr_octet_val_dig = (CPU_INT08U) (*p_char_cur - '0');
            break;

          case 'A':
          case 'B':
          case 'C':
          case 'D':
          case 'E':
          case 'F':
            addr_octet_val_dig = (CPU_INT08U)((*p_char_cur - 'A') + 10u);
            break;

          case 'a':
          case 'b':
          case 'c':
          case 'd':
          case 'e':
          case 'f':
            addr_octet_val_dig = (CPU_INT08U)((*p_char_cur - 'a') + 10u);
            break;

          default:                                                      // See Note #x.
            NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
            return(addr_ip);
        }

        if (addr_msb_octet == DEF_YES) {
          addr_ip.Addr[addr_nbr_octet] = addr_octet_val_dig;
          addr_ip.Addr[addr_nbr_octet] <<= DEF_NIBBLE_NBR_BITS;
          addr_msb_octet = DEF_NO;
        } else {
          addr_ip.Addr[addr_nbr_octet] |= addr_octet_val_dig;
          addr_msb_octet = DEF_YES;
          addr_nbr_octet++;
        }

        if (addr_nbr_octet > NET_ASCII_NBR_OCTET_ADDR_IPv6) {
          NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(addr_ip);
        }
        break;

      case ':':
        addr_nbr_colon_cur++;
        if (p_char_prev == DEF_NULL) {                                  // If NO prev char     (see Note #1b2B1), ...
          if (p_char_next == DEF_NULL) {
            NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
            return(addr_ip);
          }

          if (*p_char_next != ':') {
            NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
            return(addr_ip);
          } else {
            break;
          }
        }

        if (addr_nbr_colon_cur > NET_ASCII_CHAR_MAX_COLON_IPv6) {
          NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(addr_ip);
        }

        if (*p_char_prev != ':') {
          switch (addr_nbr_octet_dig) {
            case 0:
              NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
              RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
              return(addr_ip);

            case 1:
              addr_ip.Addr[addr_nbr_octet + 1] = addr_ip.Addr[addr_nbr_octet] >> DEF_NIBBLE_NBR_BITS;
              addr_ip.Addr[addr_nbr_octet] = 0x00;
              addr_nbr_lead_zero = addr_nbr_lead_zero + 3u;
              addr_nbr_octet = addr_nbr_octet     + 2u;
              addr_msb_octet = DEF_YES;
              break;

            case 2:
              addr_ip.Addr[addr_nbr_octet] = addr_ip.Addr[addr_nbr_octet - 1];
              addr_ip.Addr[addr_nbr_octet - 1] = 0x00;
              addr_nbr_lead_zero = addr_nbr_lead_zero + 2u;
              addr_nbr_octet = addr_nbr_octet     + 1u;
              break;

            case 3:
              addr_ip.Addr[addr_nbr_octet] >>= DEF_NIBBLE_NBR_BITS;
              addr_ip.Addr[addr_nbr_octet] |= (addr_ip.Addr[addr_nbr_octet - 1] << DEF_NIBBLE_NBR_BITS);
              addr_ip.Addr[addr_nbr_octet - 1] >>= DEF_NIBBLE_NBR_BITS;
              addr_nbr_lead_zero = addr_nbr_lead_zero + 1u;
              addr_nbr_octet = addr_nbr_octet     + 1u;
              addr_msb_octet = DEF_YES;
              break;

            case 4:
            default:
              break;
          }
        } else {
          if (p_char_next == DEF_NULL) {
            NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
            return(addr_ip);
          }

          if (*p_char_next == ':') {
            NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
            return(addr_ip);
          }

          if (*p_char_next == '\0') {
            addr_nbr_dig_end = 0;
          } else {
            addr_nbr_dig_end = 1;
          }

          while ((p_char_next != DEF_NULL)                              // Parse ALL non-NULL chars in ASCII str.
                 && (*p_char_next != '\0')    ) {
            if (*p_char_next == ':') {
              addr_nbr_colon_rem++;
            }
            p_char_next++;
          }

          addr_nbr_colon_tot = addr_nbr_colon_cur + addr_nbr_colon_rem;
          addr_nbr_octet_zero = ((NET_ASCII_LEN_MAX_ADDR_IPv6 - addr_nbr_colon_tot
                                  - (2 *  addr_nbr_octet)
                                  - (4 * (addr_nbr_colon_rem       + addr_nbr_dig_end))
                                  - (NET_ASCII_CHAR_MAX_COLON_IPv6 - addr_nbr_colon_tot)
                                  -  NET_ASCII_CHAR_LEN_NUL) / 2);
          for (nbr_octet_zero_cnt = 0u; nbr_octet_zero_cnt < addr_nbr_octet_zero; nbr_octet_zero_cnt++ ) {
            addr_ip.Addr[addr_nbr_octet] = DEF_BIT_NONE;
            addr_nbr_octet++;
          }
        }

        if (addr_nbr_octet > NET_ASCII_NBR_OCTET_ADDR_IPv6) {           // If nbr octets > max (see Note #1b2A), ...
          NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(addr_ip);
        }

        addr_nbr_octet_dig = 0u;
        break;

      default:                                                          // See Note #1b1.
        NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
        return(addr_ip);
    }

    p_char_prev = p_char_cur;
    p_char_cur++;
  }

  if (p_char_cur == DEF_NULL) {                                         // If NULL ptr in ASCII str (see Note #1b1), ..
    NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(addr_ip);
  }

  if (p_char_prev == DEF_NULL) {                                        // If NULL ptr in ASCII str (see Note #1b1), ..
    NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(addr_ip);
  }

  if ((*p_char_prev == ':')
      && (addr_nbr_dig_end == 1) ) {                                    // If last char a colon (see Note #1b2B1), ...
    NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(addr_ip);
  }

  switch (addr_nbr_octet_dig) {
    case 1:
      addr_ip.Addr[addr_nbr_octet + 1] = addr_ip.Addr[addr_nbr_octet] >> DEF_NIBBLE_NBR_BITS;
      addr_ip.Addr[addr_nbr_octet] = DEF_BIT_NONE;
      addr_nbr_lead_zero = addr_nbr_lead_zero + 3u;
      addr_nbr_octet = addr_nbr_octet     + 2u;
      addr_msb_octet = DEF_YES;
      break;

    case 2:
      addr_ip.Addr[addr_nbr_octet] = addr_ip.Addr[addr_nbr_octet - 1];
      addr_ip.Addr[addr_nbr_octet - 1] = DEF_BIT_NONE;
      addr_nbr_lead_zero = addr_nbr_lead_zero + 2u;
      addr_nbr_octet = addr_nbr_octet     + 1u;
      break;

    case 3:
      addr_ip.Addr[addr_nbr_octet] >>= DEF_NIBBLE_NBR_BITS;
      addr_ip.Addr[addr_nbr_octet] |= (addr_ip.Addr[addr_nbr_octet - 1] << DEF_NIBBLE_NBR_BITS);
      addr_ip.Addr[addr_nbr_octet - 1] >>= DEF_NIBBLE_NBR_BITS;
      addr_nbr_lead_zero = addr_nbr_lead_zero + 1u;
      addr_nbr_octet = addr_nbr_octet     + 1u;
      addr_msb_octet = DEF_YES;
      break;

    case 0:
    case 4:
    default:
      break;
  }

  if (addr_nbr_octet != NET_ASCII_NBR_OCTET_ADDR_IPv6) {                // If != nbr IPv6 addr octets (see Note #1b2A),
    NET_UTIL_IPv6_ADDR_SET_UNSPECIFIED(addr_ip);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(addr_ip);
  }

  return (addr_ip);
}

/****************************************************************************************************//**
 *                                           NetASCII_IPv4_to_Str()
 *
 * @brief    Convert a network protocol IPv4 address in host-order into an IPv4 address ASCII string
 *           in dotted-decimal notation.
 *
 * @param    addr_ip             IPv4 address.
 *
 * @param    p_addr_ip_ascii     Pointer to an ASCII character array that will receive the return IPv4
 *
 * @param    lead_zeros          Prepend leading zeros option (see Note #2) :
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *
 * @note     (1) The return dotted-decimal IPv4 address ASCII string formats EXACTLY four decimal values
 *               separated by EXACTLY three dot characters & terminated with the NULL character.
 *               The size of the ASCII character array that will receive the returned IP address
 *               ASCII string SHOULD be greater than or equal to NET_ASCII_LEN_MAX_ADDR_IP.
 *
 * @note     (2) Leading zeros option prepends leading '0's prior to the first non-zero digit in each
 *               decimal octet value.  The number of leading zeros is such that the decimal octet's
 *               number of decimal digits is equal to the maximum number of digits (3).
 *               If leading zeros option DISABLED & the decimal value of the octet is zero;
 *               then one digit of '0' value is formatted.
 *               This is NOT a leading zero; but a single decimal digit of '0' value.
 *******************************************************************************************************/
void NetASCII_IPv4_to_Str(NET_IPv4_ADDR addr_ip,
                          CPU_CHAR      *p_addr_ip_ascii,
                          CPU_BOOLEAN   lead_zeros,
                          RTOS_ERR      *p_err)
{
  CPU_CHAR   *p_char;
  CPU_INT08U base_10_val_start;
  CPU_INT08U base_10_val;
  CPU_INT08U addr_octet_nbr_shifts;
  CPU_INT08U addr_octet_val;
  CPU_INT08U addr_octet_dig_nbr;
  CPU_INT08U addr_octet_dig_val;
  CPU_INT08U i;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_ip_ascii != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------ VALIDATE NBR CHAR -------------
#if ((NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv4 < NET_ASCII_CHAR_MIN_OCTET) \
  || (NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv4 > NET_ASCII_CHAR_MAX_OCTET_08))
  *p_addr_ip_ascii = '\0';
  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  //                                                               ------------- CONVERT IP ADDR --------------
  p_char = p_addr_ip_ascii;

  base_10_val_start = 1u;
  for (i = 1u; i < NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv4; i++) {           // Calc starting dig val.
    base_10_val_start *= 10u;
  }

  for (i = NET_ASCII_NBR_OCTET_ADDR_IPv4; i > 0u; i--) {                // Parse ALL addr octets
                                                                        // Calc  cur addr octet val.
    addr_octet_nbr_shifts = (i - 1u) * DEF_OCTET_NBR_BITS;
    addr_octet_val = (CPU_INT08U)((addr_ip >> addr_octet_nbr_shifts) & DEF_OCTET_MASK);

    base_10_val = base_10_val_start;
    while (base_10_val > 0u) {                                          // Parse ALL octet digs.
      addr_octet_dig_nbr = addr_octet_val / base_10_val;

      if ((addr_octet_dig_nbr > 0u)                                     // If octet dig val > 0,                     ..
          || (base_10_val == 1u)                                        // .. OR on least-sig octet dig,             ..
          || (lead_zeros == DEF_YES)) {                                 // .. OR lead zeros opt ENABLED (see Note #2),
                                                                        // .. calc & insert octet val ASCII dig.
        addr_octet_dig_val = (CPU_INT08U)(addr_octet_dig_nbr % 10u);
        *p_char++ = (CPU_CHAR)(addr_octet_dig_val + '0');
      }

      base_10_val /= 10u;                                               // Shift to next least-sig octet dig.
    }

    if (i > 1u) {                                                       // If NOT on last octet, ...
      *p_char++ = '.';                                                  // ... insert a dot char
    }
  }

  *p_char = '\0';                                                       // Append NULL char
}

/****************************************************************************************************//**
 *                                           NetASCII_IPv6_to_Str()
 *
 * @brief    Convert a network protocol IPv6 address in host-order into an IPv6 address ASCII string
 *           in dotted-decimal notation.
 *
 * @param    p_addr_ip           Pointer to IPv6 address.
 *
 * @param    p_addr_ip_ascii     Pointer to an ASCII character array that will receive the return IPv6
 *
 * @param    hex_lower_case      DEF_YES, hexadecimal value will be in lower case
 *                               DEF_NO,  otherwise.
 *
 * @param    lead_zeros          Prepend leading zeros option (see Note #2) :
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *******************************************************************************************************/
void NetASCII_IPv6_to_Str(NET_IPv6_ADDR *p_addr_ip,
                          CPU_CHAR      *p_addr_ip_ascii,
                          CPU_BOOLEAN   hex_lower_case,
                          CPU_BOOLEAN   lead_zeros,
                          RTOS_ERR      *p_err)
{
  CPU_CHAR    *p_char;
  CPU_INT08U  *p_addr;
  CPU_INT08U  addr_octet_val;
  CPU_INT08U  addr_octet_dig_val;
  CPU_INT08U  addr_octet_nbr_shifts;
  CPU_INT08U  addr_nbr_octet_dig;
  CPU_INT08U  i;
  CPU_INT08U  j;
  CPU_BOOLEAN addr_zero_lead;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_ip != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_ip_ascii != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- CONVERT IP ADDR --------------
  p_addr = &p_addr_ip->Addr[0];
  p_char = p_addr_ip_ascii;
  addr_nbr_octet_dig = 0u;
  addr_zero_lead = DEF_YES;

  for (i = NET_ASCII_NBR_OCTET_ADDR_IPv6; i > 0; i--) {                 // Parse ALL addr octets (see Note #1b1A).
    addr_octet_val = *p_addr;

    for (j = NET_ASCII_CHAR_MAX_OCTET_ADDR_IPv6; j > 0; j--) {          // Parse ALL octet's hex digs.
                                                                        // Calc  cur octet's hex dig val.
      addr_octet_nbr_shifts = (j - 1) * DEF_NIBBLE_NBR_BITS;
      addr_octet_dig_val = (addr_octet_val >> addr_octet_nbr_shifts) & DEF_NIBBLE_MASK;
      //                                                           Insert    octet's hex val ASCII dig.
      addr_nbr_octet_dig++;
      if (addr_octet_dig_val < 10) {
        if ((lead_zeros == DEF_YES)
            || (addr_zero_lead == DEF_NO)
            || (addr_nbr_octet_dig == NET_ASCII_CHAR_MAX_DIG_ADDR_IPv6)
            || (addr_octet_dig_val != DEF_BIT_NONE)) {
          *p_char++ = (CPU_CHAR)(addr_octet_dig_val + '0');
          addr_zero_lead = DEF_NO;
        }
      } else {
        if (hex_lower_case != DEF_YES) {
          *p_char++ = (CPU_CHAR)((addr_octet_dig_val - 10u) + 'A');
        } else {
          *p_char++ = (CPU_CHAR)((addr_octet_dig_val - 10u) + 'a');
        }
        addr_zero_lead = DEF_NO;
      }
    }

    if ((i % 2 != 0)
        && (i > 1)) {                                                   // If NOT on last octet,                    ..
      *p_char++ = ':';                                                  // .. insert colon char (see Note #1b1B2b).
      addr_zero_lead = DEF_YES;
      addr_nbr_octet_dig = 0u;
    }

    p_addr++;
  }

  *p_char = '\0';                                                       // Append NULL char (see Note #1b1C).
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetASCII_Str_to_IPv4_Handler()
 *
 * @brief    Convert an IPv4 address ASCII string in dotted-decimal notation to a network protocol
 *           IPv4 address in host-order.
 *
 * @param    p_addr_ip_ascii     Pointer to an ASCII string that contains a dotted-decimal IPv4 address
 *                               (see Note #1).
 *
 * @param    p_dot_nbr           Pointer to the number of dot found in the ASCII string.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   Host-order IPv4 address represented by ASCII string, if NO error(s).
 *           NET_IPv4_ADDR_NONE, otherwise.
 *
 * @note     (1) RFC #1983 states that "dotted decimal notation ... refers [to] IP addresses of the
 *               form A.B.C.D; where each letter represents, in decimal, one byte of a four byte IP
 *               address".
 *               @n
 *               In other words, the dotted-decimal IP address notation separates four decimal octet
 *               values by the dot, or period, character ('.').  Each decimal value represents one
 *               octet of the IP address starting with the most significant octet in network-order.
 *               @verbatim
 *                   IP Address Examples :
 *
 *                           DOTTED DECIMAL NOTATION     HEXADECIMAL EQUIVALENT
 *
 *                               127.0.0.1           =       0x7F000001
 *                               192.168.1.64        =       0xC0A80140
 *                               255.255.255.0       =       0xFFFFFF00
 *                               ---         -                 --    --
 *                               ^          ^                 ^      ^
 *                               |          |                 |      |
 *                               MSO        LSO               MSO    LSO
 *
 *                       where
 *                               MSO        Most  Significant Octet in Dotted Decimal IP Address
 *                               LSO        Least Significant Octet in Dotted Decimal IP Address
 *               @endverbatim
 * @note     (2) IEEE Std 1003.1, 2004 Edition - inet_addr, inet_ntoa - IPv4 address manipulation:
 *               - (a) Values specified using IPv4 dotted decimal notation take one of the following forms:
 *                   - (1) a.b.c.d - When four parts are specified, each shall be interpreted
 *                                   as a byte of data and assigned, from left to right,
 *                                   to the four bytes of an Internet address.
 *                   - (2) a.b.c   - When a three-part address is specified, the last part shall
 *                                   be interpreted as a 16-bit quantity and placed in the
 *                                   rightmost two bytes of the network address. This makes
 *                                   the three-part address format convenient for specifying
 *                                   Class B network addresses as "128.net.host".
 *                   - (3) a.b     - When a two-part address is supplied, the last part shall be
 *                                   interpreted as a 24-bit quantity and placed in the
 *                                   rightmost three bytes of the network address. This makes
 *                                   the two-part address format convenient for specifying
 *                                       Class A network addresses as "net.host".
 *                   - (4) a       - When only one part is given, the value shall be stored
 *                                   directly in the network address without any byte rearrangement.
 *               - (b) IP Address Examples :
 *                     @verbatim
 *                                       DOTTED DECIMAL NOTATION     HEXADECIMAL EQUIVALENT
 *
 *                                           127.0.0.1           =       0x7F000001
 *                                           192.168.1.64        =       0xC0A80140
 *                                           192.168.320         =       0xC0A80140
 *                                           192.11010368        =       0xC0A80140
 *                                           3232235840          =       0xC0A80140
 *                                           255.255.255.0       =       0xFFFFFF00
 *                                           ---         -                 --    --
 *                                           ^          ^                 ^      ^
 *                                           |          |                 |      |
 *                                           MSO        LSO               MSO    LSO
 *
 *                                   where
 *                                           MSO        Most  Significant Octet in Dotted Decimal IP Address
 *                                           LSO        Least Significant Octet in Dotted Decimal IP Address
 *                     @endverbatim
 *
 * @note     (3) Therefore, the dotted-decimal IP address ASCII string MUST :
 *               - (a) Include ONLY decimal values & the dot, or period, character ('.') ; ALL other
 *                     characters are trapped as invalid, including any leading or trailing characters.
 *               - (b) Include UP TO four decimal values separated by UP TO three dot characters;
 *                     and MUST be terminated with the NULL character.
 *               - (c) Ensure that each decimal value's number of decimal digits, including leading
 *                     zeros, does NOT exceed      the maximum number of digits (10).
 *                   - (1) However, any decimal value's number of decimal digits, including leading
 *                         zeros, MAY be less than the maximum number of digits.
 *               - (d) Ensure that each decimal value does NOT exceed the maximum value for its form:
 *                     @verbatim
 *                     - (1) a.b.c.d - 255.255.255.255
 *                     - (2) a.b.c   - 255.255.65535
 *                     - (3) a.b     - 255.16777215
 *                     - (4) a       - 4294967295
 *                     @endverbatim
 * @note     (4) To avoid possible integer arithmetic overflow, the IP address octet arithmetic result
 *               MUST be declared as an integer data type with a greater resolution -- i.e. greater
 *               number of bits -- than the IP address octet data type(s).
 *******************************************************************************************************/
NET_IPv4_ADDR NetASCII_Str_to_IPv4_Handler(CPU_CHAR   *p_addr_ip_ascii,
                                           CPU_INT08U *p_dot_nbr,
                                           RTOS_ERR   *p_err)
{
  CPU_INT64U    addr_parts[NET_ASCII_NBR_MAX_DEC_PARTS];
  CPU_CHAR      *p_char_cur;
  CPU_CHAR      *p_char_prev;
  NET_IPv4_ADDR addr_ip;
  CPU_INT64U    *p_addr_part_val;                                       // See Note #4.
  CPU_INT08U    addr_nbr_octet;
  CPU_INT08U    addr_nbr_part_dig;

  //                                                               ----------- CONVERT IP ADDR STR ------------
  p_char_cur = p_addr_ip_ascii;
  p_char_prev = DEF_NULL;
  addr_ip = NET_IPv4_ADDR_NONE;
  p_addr_part_val = &addr_parts[0];
  *p_addr_part_val = 0u;
  addr_nbr_octet = 0u;
  addr_nbr_part_dig = 0u;
  *p_dot_nbr = 0u;

  while ((p_char_cur != DEF_NULL)                                       // Parse ALL non-NULL chars in ASCII str.
         && (*p_char_cur != '\0')    ) {
    switch (*p_char_cur) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        addr_nbr_part_dig++;                                            // If nbr digs > max (see Note #1), ...
        if (addr_nbr_part_dig > NET_ASCII_CHAR_MAX_PART_ADDR_IP) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(NET_IPv4_ADDR_NONE);
        }

        //                                                         Convert & merge ASCII char into decimal val.
        *p_addr_part_val *= 10u;
        *p_addr_part_val += (CPU_INT16U)(*p_char_cur - '0');

        if (*p_addr_part_val > NET_ASCII_VAL_MAX_PART_ADDR_IP) {            // If octet val > max (see Note #1), ...
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(NET_IPv4_ADDR_NONE);
        }
        break;

      case '.':
        if (p_char_prev == DEF_NULL) {                                  // If NO prev char     (see Note #1), ...
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(NET_IPv4_ADDR_NONE);
        }

        if (*p_char_prev == '.') {                                      // If prev char a dot  (see Note #1), ...
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(NET_IPv4_ADDR_NONE);
        }

        addr_nbr_octet++;
        if (addr_nbr_octet >= NET_ASCII_NBR_OCTET_ADDR_IPv4) {          // If nbr octets > max (see Note #1), ...
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
          return(NET_IPv4_ADDR_NONE);
        }

        *p_dot_nbr += 1;
        p_addr_part_val++;
        *p_addr_part_val = 0x0000000000000000u;
        addr_nbr_part_dig = 0u;
        break;

      default:                                                          // See Note #1.
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
        return(NET_IPv4_ADDR_NONE);
    }

    p_char_prev = p_char_cur;
    p_char_cur++;
  }

  if (p_char_cur == DEF_NULL) {                                         // If NULL ptr in ASCII str (see Note #1), ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(NET_IPv4_ADDR_NONE);
  }

  if (p_char_prev == DEF_NULL) {                                        // If NULL        ASCII str (see Note #1), ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(NET_IPv4_ADDR_NONE);
  }

  if (*p_char_prev == '.') {                                            // If last char a dot (see Note #1), ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
    return(NET_IPv4_ADDR_NONE);
  }

  addr_ip = (NET_IPv4_ADDR)addr_parts[0];

  switch (*p_dot_nbr) {
    case 0:                                                             // IP addr format : a       - 32 bits.
      if (addr_parts[0] > NET_ASCII_MASK_32_01_BIT) {                   // (See Note #3d4)
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
        return(NET_IPv4_ADDR_NONE);
      }
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      return (addr_ip);

    case 1:                                                             // IP addr format : a.b     - 24 bits.
      if ((addr_ip > NET_ASCII_MASK_08_01_BIT)                          // (See Note #3d3)
          | (addr_parts[1] > NET_ASCII_MASK_24_01_BIT)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
        return(NET_IPv4_ADDR_NONE);
      }
      addr_ip |= ((addr_parts[1] & NET_ASCII_MASK_08_01_BIT) << (DEF_OCTET_NBR_BITS * 3))
                 | ((addr_parts[1] & NET_ASCII_MASK_16_09_BIT) <<  DEF_OCTET_NBR_BITS)
                 | ((addr_parts[1] & NET_ASCII_MASK_24_17_BIT) >>  DEF_OCTET_NBR_BITS);
      break;

    case 2:                                                             // IP addr format : a.b.c   - 16 bits.
      if ((addr_ip > NET_ASCII_MASK_08_01_BIT)                          // (See Note #3d2)
          | (addr_parts[1] > NET_ASCII_MASK_08_01_BIT)
          | (addr_parts[2] > NET_ASCII_MASK_16_01_BIT)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
        return(NET_IPv4_ADDR_NONE);
      }
      addr_ip |= ((addr_parts[1] & NET_ASCII_MASK_08_01_BIT) <<  DEF_OCTET_NBR_BITS)
                 | ((addr_parts[2] & NET_ASCII_MASK_08_01_BIT) << (DEF_OCTET_NBR_BITS * 3))
                 | ((addr_parts[2] & 0x0000FF00) <<  DEF_OCTET_NBR_BITS);
      break;

    case 3:                                                             // IP addr format : a.b.c.d -  8 bits.
      if ((addr_ip > NET_ASCII_MASK_08_01_BIT)                          // (See Note #3d1)
          | (addr_parts[1] > NET_ASCII_MASK_08_01_BIT)
          | (addr_parts[2] > NET_ASCII_MASK_08_01_BIT)
          | (addr_parts[3] > NET_ASCII_MASK_08_01_BIT)) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
        return(NET_IPv4_ADDR_NONE);
      }
      addr_ip |= ((addr_parts[1] & NET_ASCII_MASK_08_01_BIT) <<  DEF_OCTET_NBR_BITS)
                 | ((addr_parts[2] & NET_ASCII_MASK_08_01_BIT) << (DEF_OCTET_NBR_BITS * 2))
                 | ((addr_parts[3] & NET_ASCII_MASK_08_01_BIT) << (DEF_OCTET_NBR_BITS * 3));
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_STR_ADDR_INVALID);
      return(NET_IPv4_ADDR_NONE);
  }

  addr_ip = NET_UTIL_VAL_SWAP_ORDER_32(addr_ip);

  return (addr_ip);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
