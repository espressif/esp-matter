/******************************************************************************

 @file  oad_image_header.h

 @brief This file contains the image header information, for an "off-chip" solution.

 Note: BIM (the Boot Image Manager) supports two configurations.

  Configuration 1 - is an "on chip solution"
  Configuration 2 - (what this header file supports) is an off chip.

  The off chip solution stores the new image on an external flash, typically a SPI flash.

 Group: WCS, BTS, LPRF
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 ******************************************************************************/

#ifndef OAD_IMAGE_HEADER_H_
#define OAD_IMAGE_HEADER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * DEFINES
 ******************************************************************************/
#define OAD_SINGLE_APP   1

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/


// Image types
#define OAD_IMG_TYPE_PERSISTENT_APP       0
#define OAD_IMG_TYPE_APP                  1 /* <-- Thread Application uses this */
#define OAD_IMG_TYPE_STACK                2
#define OAD_IMG_TYPE_APP_STACK            3
#define OAD_IMG_TYPE_NP                   4
#define OAD_IMG_TYPE_FACTORY              5
#define OAD_IMG_TYPE_BIM                  6
#define OAD_IMG_TYPE_APPSTACKLIB          7
#define OAD_IMG_TYPE_USR_BEGIN            16
#define OAD_IMG_TYPE_USR_END              31
#define OAD_IMG_TYPE_HOST_BEGIN           32
#define OAD_IMG_TYPE_HOST_END             63
#define OAD_IMG_TYPE_RSVD_BEGIN           64
#define OAD_IMG_TYPE_RSVD_END             255

//!< Segment types
#define OAD_SEG_BOUNDARY                  0x00
#define OAD_SEG_CONTIGUOUS                0x01
#define OAD_SEG_NON_CONTIGUOUS            0x02
#define OAD_SEG_RSVD                      0xFF

//!< Wireless Technology Types
#define OAD_WIRELESS_TECH_BLE             0xFFFE
#define OAD_WIRELESS_TECH_TIMAC_SUBG      0xFFFD
#define OAD_WIRELESS_TECH_TIMAC_2_4G      0xFFFB
#define OAD_WIRELESS_TECH_ZIGBEE          0xFFF7
#define OAD_WIRELESS_TECH_RF4CE           0xFFEF
#define OAD_WIRELESS_TECH_THREAD          0xFFDF /* <-- Thread applications use this */
#define OAD_WIRELESS_TECH_EASYLINK        0xFFBF

#if defined(DeviceFamily_CC26X2) || defined (DeviceFamily_CC26X2_V2) || \
    defined(DeviceFamily_CC26X2X7)
  #define OAD_IMG_ID_VAL                    {'C', 'C', '2', '6', 'x', '2', 'R', '1'}
#elif defined (DeviceFamily_CC13X2) ||                                        \
      defined (DeviceFamily_CC13X2X7) ||                                      \
      defined (DeviceFamily_CC13X2_V1) ||                                     \
      defined (DeviceFamily_CC13X2_V2)
  #define OAD_IMG_ID_VAL                    {'C', 'C', '1', '3', 'x', '2', 'R', '1'}
#elif defined (DeviceFamily_CC26X0R2)
  #define OAD_IMG_ID_VAL                    {'O', 'A', 'D', ' ', 'I', 'M', 'G', ' '}
#elif defined (DOXYGEN)
  /*!
   * Magic number to identify OAD image header. It is recommended that the
   * customer adjust these to uniquely identify their device
   *
   * This define will be filled in conditionally based on DeviceFamily_*
   *   - DeviceFamily_CC26X2:  @code {'C', 'C', '2', '6', 'x', '2', 'R', '1'} @endcode
   *   - DeviceFamily_CC13X2:  @code {'C', 'C', '1', '3', 'x', '2', 'R', '1'} @endcode
   *   - DeviceFamily_CC26X0R2: @code {'O', 'A', 'D', ' ', 'I', 'M', 'G', ' '} @endcode
   *   - Else an error will be thrown
   */
  #define OAD_IMG_ID_VAL
#else
  #error "Unknown Device Family"
#endif //DeviceFamily_CC26X2

/*!
 * Magic number used by entries in the image header table at the beginning of
 * external flash. Note this is different from @ref OAD_IMG_ID_VAL so that
 * the application can determine the difference between an entry in the header
 * table and the start of an OAD image
 *
 * \note This is only used by off-chip OAD
 */
#define OAD_EXTFL_ID_VAL                  {'O', 'A', 'D', ' ', 'N', 'V', 'M', '1'}
// External flash metadata identification value
#define SOFTWARE_VER                 {'0', '0', '0', '1'}

#define MAX_ONCHIP_FLASH_PAGES       44
#define IMG_DATA_OFFSET              12                                   /* Start of data after CRC bytes */
#define OAD_IMG_HDR_LEN              44                                   /* The length of just the OAD Header */
#define OAD_IMG_ID_LEN               8                                    /* OAD image identification bytes length */
#define OAD_SW_VER_LEN               4                                    /* OAD software version length in bytes */
#define CRC_OFFSET                   offsetof(img_common_header_t, crc32)            /* Supported metadata version */
#define BIM_VER_OFFSET               offsetof(img_common_header_t, bimVer)           /* BIM metadata version offset */
#define IMG_INFO_OFFSET              offsetof(img_common_header_t, imgCpStat)        /* ImageInfo Bytes offset */
#define IMG_COPY_STAT_OFFSET         offsetof(img_common_header_t, imgCpStat)        /* Image copy status */
#define CRC_STAT_OFFSET              offsetof(img_common_header_t, crcStat)          /* Offset to CRC status byte */
#define IMG_TYPE_OFFSET              offsetof(img_common_header_t, imgType)          /* Offset to CRC status byte */

#define SEG_LEN_OFFSET               4                                    /* Offset from seg header to seg len */
#define SEG_HDR_LEN                  12                                   /* Length of the segment header bytes */

#define IMG_PAYLOAD_SEG_ID           1                                    /* Id of the image payload segment */
#define IMG_BOUNDARY_SEG_ID          0                                    /* Id of RAM/stack boundary segment */
#define IMG_NONCOUNT_SEG_ID          2                                    /* Id of non contigouous image segment */
#define IMG_SECURITY_SEG_ID          3                                    /* Id of security segment */

#define BIM_V1_VER                   0x1    /* Supported BIM version */
#define BIM_V2_VER                   0x2    /* Supported BIM version */
#define BIM_V3_VER                   0x3    /* Supported BIM version */
#define META_VER                     0x1    /* Supported metadata version */
#define SECURITY_VER                 0x1
#define DEFAULT_STATE                0xFF   /* default status */
#define CRC_VALID                    0xFE
#define CRC_INVALID                  0xFC
#define NEED_COPY                    0xFE    /* Image to be copied on on-chip flash at location indicated in the image */
#define COPY_DONE                    0xFC    /* Image already copied */

#define INVALID_ADDR                 0xFFFFFFFF

/*******************************************************************************
 * Typedefs
 */

/*
 *  The complete image header is a combination of:
 *     First: struct img_common_header
 *  Followed by 1 or more, in any order:
 *     struct img_seg_payload
 *     struct img_seg_boundary
 *     struct img_seg_signature
 *  The last item is always:
 *     struct img_seg_last
 *
 * A typical true header might be as follows
 *    struct my_header {
 *         struct img_common_header   header;
 *         struct img_seg_payload     payload;
 *         struct img_seg_last        last;
 *    };
 *
 * Or
 *    struct my_header {
 *         struct img_common_header header;
 *         struct img_seg_boundary  boundary;
 *         struct img_seg_payload   payload;
 *         struct img_seg_security  security;
 *         struct img_seg_last      last;
 *    };
 */

/* Common Header */
struct img_common_header  {
    /**
     * Image signature/magic value.
     *
     * Initialize with: OAD_IMG_ID_VAL
     */
    uint8_t   imgID[8];

    /**
     * CRC32 of the payload
     *
     * Initialize with: 0xFFFFFFFF
     * Calculated by:  oad_image_tool
     */
    uint32_t  crc32;

    /**
     * Format of this header is Version 1
     *
     * Initialize with: BIM_VER
     */
    uint8_t   bimVer;         //!< BIM version */

    /**
     * Format of meta data is Version 1
     *
     * Initialize with: META_VER
     */
    uint8_t   metaVer;        //!< Metadata version */

    /**
     * Wireless Protocol, BLE/TI-MAC/ZIGBEE etc.
     *
     * Example: OAD_WIRELESS_TECH_BLE
     * Example: OAD_WIRELESS_TECH_THREAD
     */
    uint16_t  techType;

    /**
     * Image Copy Status
     *
     * gives status of the image if its copied yet or not.
     *
     * Initialized with: DEFAULT_STATE
     * Changed/Updated by BIM as it is copied.
     */
    uint8_t   imgCpStat;

    /**
     * Status of the field: crc32
     *
     * Initialize with: DEFAULT_STATE
     * Calculated by: oad_image_tool
     */
    uint8_t   crcStat;

    /**
     * Image Type
     *
     * Initialize with: (somevalue)
     *
     * Example: OAD_IMG_TYPE_APP
     *    (an "app" can be loaded)
     * Example: OAD_IMG_TYPE_FACTORY
     *    Indicates the "factory default" image
     */
    uint8_t   imgType;

    /**
     * Image Number, used to distinguish multiple images within a technology
     *
     * Using the SDK Example applications
     * Using OpenThread
     *      the cli example might be image 0
     *      the door lock example might be image 1
     * Using BLE
     *      simple peripheral might be image 0
     *      simple central might be image 1
     */
    uint8_t   imgNo;

    /**
     * Indicates the image is valid
     *
     * Initialize with: 0xFFFFFFFF
     * Modified by: ????
     */
    uint32_t  imgVld;

    /**
     * Length in bytes of the image
     *
     * Initialize with: 0xFFFFFFFF
     * Modified by: oad_image_tool
     */
    uint32_t  len;

    /**
     * Application Entry point
     *
     * This is a pointer to an array of uint32_t values.
     * Element [0] = the initial stack pointer value
     * Element [1] = is the application "startup()" function address
     */
    uint32_t  prgEntry;

    /**
     * Application software version
     *
     * This identifies the 'software version' in this image.
     */
    uint8_t   softVer[4];

    /**
     * Last address in flash where this image is located
     *
     * This is the last byte in the flash image.
     *
     */
    uint32_t  imgEndAddr;

    /**
     * Size of this structure, in bytes
     */

    uint16_t  hdrLen;

    /**
     * Reserved for future use.
     */
    uint16_t  rfu;
} __attribute__((packed)) ;

typedef struct img_common_header img_common_header_t;

struct img_seg_payload
{
    /**
     * Type of this segment, always: IMG_PAYLOAD_SEG_ID
     */
    uint8_t   segType;

    /**
     * Protocol for this segment
     *
     * Example: OAD_WIRELESS_TECH_BLE
     * Example: OAD_WIRELESS_TECH_THREAD
     */
    uint16_t  wirelessTech;

    /**
     * Reserved
     *
     * Initialize with: 0xff
     */
    uint8_t   rfu;

    /**
     * size in bytes of this segment
     *
     * Initialize with: 0xffffffff
     * Modified/Updated by oad_image_tool
     */
    uint32_t  imgSegLen;

    /**
     * On chip destination address of this segment
     *
     * Initialize with: Application dependent
     *
     * Typically: This is "0"
     * Typically: The address of the "imgId" in the image header structure.
     *
     * Example:
     *    my_hdr.p.startAddr = (uint32_t)(&(my_hdr.h));
     */
    uint32_t  startAddr;
} __attribute__((packed));

struct img_seg_boundary
{
  uint8_t   segType;          //!< Segment type, always: IMG_BOUNDARY_SEG_ID */
  uint16_t  wirelessTech;     //!< Wireless technology type */
  uint8_t   rfu;
  uint32_t  segTypeNLen;      //!< Start address of stack image on internal flash */
  uint32_t  stackStartAddr;   //!< Start address of stack image on internal flash */
  uint32_t  stackEntryAddr;   //!< Stack start adddress */
  uint32_t  ram0StartAddr;    //!< RAM entry start address */
  uint32_t  ram0EndAddr;      //!< End of Image address */
} __attribute__((packed));

/*! ECC signature pair */
typedef struct {
  uint8_t  sign_r[32];
  uint8_t  sign_s[32];
} eccSignature_t;

/*!
* Structure to hold the Signer Info and the Signature
*/
typedef struct __attribute__((packed))
{
    uint8_t         secSignerInfo[8];  //!< Security signer info */
    eccSignature_t  eccSign;           //!< Security signature */
} signPld_ECDSA_P256_t;

typedef struct __attribute__((packed))
{
    uint8_t       securityVersion;
    uint32_t      timeStamp;
} securityHdr_t;

/**
 * This structure defines the security segments.
 */
struct img_seg_signature
{
    uint8_t   segTypeSecure;     //!< Segment type - for Secuirty info payload
    uint16_t  wirelessTech;      //!< Wireless technology type
    uint8_t   verifStat;         //!< Verification status
    uint32_t  secSegLen;         //!< Payload segment length
    uint8_t   secVer;            //!< Security version */
    uint32_t  secTimestamp;      //!< Security timestamp */
    uint8_t   secSignerInfo[8];  //!< Security signer info */
    eccSignature_t  eccSign;     //!< Security signature */
} __attribute__((packed));


union img_seg_any {
    uint8_t segType;
    struct img_seg_signature s;
    struct img_seg_boundary  b;
    struct img_seg_payload   p;
} __attribute__((packed)) ;

struct img_seg_last {
    uint8_t segType; /* always 0xff */
};


struct img_header_single_app {
    struct img_common_header h;
#if (defined(SECURITY))
    struct img_seg_signature s;
#endif
    struct img_seg_payload   p;
    struct img_seg_last      last;
};

struct oad_image_boundary {
    struct img_common_header h;
    struct img_seg_boundary  b;
    struct img_seg_last      last;
};

#if OAD_SINGLE_APP
extern const struct img_header_single_app oad_image_header;
#endif

/* Complete Image Header */
typedef struct imgHdr
{
    struct img_common_header fixedHdr;
#if (defined(SECURITY))
    struct img_seg_signature s;
#endif
    struct img_seg_payload   imgPayload;
    struct img_seg_last      last;
} imgHdr_t;

#define _imgHdr oad_image_header
typedef struct img_common_header imgFixedHdr_t;

#if OAD_WITH_BOUNDARY
extern const struct oad_image_boundary oad_image_header;
#endif

#ifdef __cplusplus
}
#endif

#endif /* OAD_IMAGE_HEADER_H_ */
