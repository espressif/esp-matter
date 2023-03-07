/***************************************************************************//**
 * @file
 * @brief Custom tags for Gecko Bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "btl_gbl_custom_tags.h"

#if defined(BTL_PARSER_SUPPORT_LZ4)
#include "parser/compression/btl_decompress_lz4.h"
#endif

#if defined(BTL_PARSER_SUPPORT_LZMA)
#include "parser/compression/btl_decompress_lzma.h"
#endif

#if defined(BTL_PARSER_SUPPORT_LZ4) || defined(BTL_PARSER_SUPPORT_LZMA)
// At least one custom tag is supported
#define HAVE_CUSTOM_TAGS
#endif

#if defined(HAVE_CUSTOM_TAGS)
const GblCustomTag_t gblCustomTags[] = {
#if defined(BTL_PARSER_SUPPORT_LZ4)
  {
    .tagId = 0xFD0505FDUL,
    .enterTag = gbl_lz4EnterProgTag,
    .parseTag = gbl_lz4ParseProgTag,
    .exitTag = gbl_lz4ExitProgTag,
    .numBytesRequired = gbl_lz4NumBytesRequired
  },
#endif
#if defined(BTL_PARSER_SUPPORT_LZMA)
  {
    .tagId = 0xFD0707FDUL,
    .enterTag = gbl_lzmaEnterProgTag,
    .parseTag = gbl_lzmaParseProgTag,
    .exitTag = gbl_lzmaExitProgTag,
    .numBytesRequired = gbl_lzmaNumBytesRequired
  },
#endif
};
#endif

bool gbl_isCustomTag(GblTagHeader_t *tagHeader)
{
#if defined(HAVE_CUSTOM_TAGS)
  for (size_t i = 0; i < sizeof(gblCustomTags) / sizeof(GblCustomTag_t); i++) {
    if (gblCustomTags[i].tagId == tagHeader->tagId) {
      return true;
    }
  }
#else
  (void) tagHeader;
#endif
  return false;
}

const GblCustomTag_t * gbl_getCustomTagProperties(uint32_t tagId)
{
#if defined(HAVE_CUSTOM_TAGS)
  for (size_t i = 0; i < sizeof(gblCustomTags) / sizeof(GblCustomTag_t); i++) {
    if (gblCustomTags[i].tagId == tagId) {
      return &gblCustomTags[i];
    }
  }
#else
  (void) tagId;
#endif
  return NULL;
}
