/***************************************************************************//**
 * @file
 * @brief Zigbee Over-the-air bootload cluster for upgrading firmware and
 * downloading device specific file.
 * This file handles the page request feature for the client.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

enum {
  EM_AF_NO_PAGE_REQUEST              = 0,
  EM_AF_WAITING_PAGE_REQUEST_REPLIES = 1,
  EM_AF_RETRY_MISSED_PACKETS         = 2,
  EM_AF_PAGE_REQUEST_COMPLETE        = 3,
  EM_AF_BLOCK_ALREADY_RECEIVED       = 4,
  EM_AF_PAGE_REQUEST_ERROR           = 0xFF
};
typedef uint8_t EmAfPageRequestClientStatus;

#define EM_AF_PAGE_REQUEST_BLOCK_SIZE 32

// This routine returns a timer indicating how long we should wait for
// the page request responses to come in.  0 if there was an error.
uint32_t emAfInitPageRequestClient(uint32_t offsetForPageRequest,
                                   uint32_t totalImageSize);
void emAfPageRequestTimerExpired(void);
bool emAfHandlingPageRequestClient(void);
EmAfPageRequestClientStatus emAfGetCurrentPageRequestStatus(void);
EmAfPageRequestClientStatus emAfNoteReceivedBlockForPageRequestClient(uint32_t offset);
EmAfPageRequestClientStatus emAfNextMissedBlockRequestOffset(uint32_t* nextOffset);

uint32_t emAfGetPageRequestMissedPacketDelayMs(void);
uint32_t emAfGetFinishedPageRequestOffset(void);
void emAfAbortPageRequest(void);
