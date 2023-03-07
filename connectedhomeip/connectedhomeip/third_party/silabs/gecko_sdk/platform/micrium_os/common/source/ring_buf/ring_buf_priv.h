/***************************************************************************//**
 * @file
 * @brief Common - Ring Buffer Data Structure
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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _RING_BUF_PRIV_H_
#define  _RING_BUF_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       COMPILE-TIME NOT INIT MACRO
 *******************************************************************************************************/

#define  RING_BUF_INIT_NULL()     \
  {                               \
    /* .StartPtr   = */ DEF_NULL, \
    /* .Size       = */ 0u,       \
    /* .RdIx       = */ 0u,       \
    /* .RdIxNext   = */ 0u,       \
    /* .WrIx       = */ 0u,       \
    /* .WrIxPrev   = */ 0u,       \
    /* .EndValidIx = */ 0u,       \
    /* .IsReading  = */ DEF_NO,   \
    /* .RdIxDirty  = */ DEF_NO    \
  }

/********************************************************************************************************
 *                                           INIT CHK MACRO
 *******************************************************************************************************/

#define  RING_BUF_IS_NULL(p_ring_buf)         (((p_ring_buf)->StartPtr == DEF_NULL) ? DEF_YES : DEF_NO)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct ring_buf {
  CPU_INT08U  *StartPtr;                                        // Ptr to start of ring buf data.
  CPU_INT32U  Size;                                             // Size of ring buf alloc'd or to allocate.
  CPU_INT16U  RdIx;                                             // Ix to where consumer will take/ is taking data.
  CPU_INT16U  RdIxNext;                                         // Ix to where consumer will take next data.
  CPU_INT16U  WrIx;                                             // Ix to where producer(s) will put  data.
  CPU_INT16U  WrIxPrev;                                         // Ix to where producer was before writing to buf.
  CPU_INT16U  EndValidIx;                                       // Ix to end of valid data, in the ring buf.
  CPU_BOOLEAN IsReading;                                        // Flag indicating if a read is currently in progress.
  CPU_BOOLEAN RdIxDirty;                                        // Flag indicating if the rd ix needs to be refreshed.
} RING_BUF;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void RingBufCreate(RING_BUF   *p_ring_buf,
                   MEM_SEG    *p_mem_seg,
                   CPU_INT16U buf_size,
                   RTOS_ERR   *p_err);

void *RingBufWrAlloc(RING_BUF   *p_ring_buf,
                     CPU_INT16U size);

void RingBufWrCommit(RING_BUF *p_ring_buf);

CPU_BOOLEAN RingBufWr(RING_BUF   *p_ring_buf,
                      CPU_INT16U size,
                      void       *p_start_data);

CPU_BOOLEAN RingBufDataIsAvail(RING_BUF *p_ring_buf);

CPU_BOOLEAN RingBufRdStart(RING_BUF *p_ring_buf);

void *RingBufRd(RING_BUF   *p_ring_buf,
                CPU_SIZE_T size);

void RingBufRdEnd(RING_BUF *p_ring_buf);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of private ring buf module include.
