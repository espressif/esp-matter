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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <common/source/ring_buf/ring_buf_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RING_BUF_IX_SIZE_BYTES             2u

#define  LOG_DFLT_CH                       (COMMON, RING_BUF)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void RingBufRdIxNextRefresh(RING_BUF *p_ring_buf);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               RingBufCreate()
 *
 * @brief    Create and initializes a ring buffer.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @param    p_mem_seg   Pointer to memory segment to use to allocate buffer. If DEF_NULL, the LIB Mem
 *                       heap will be used.
 *
 * @param    buf_size    Size of the ring buffer, in bytes.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_SEG_OVF
 *
 * @note     (1) Buffer size must be at least 128 bytes and less than 32767 bytes.
 *******************************************************************************************************/
void RingBufCreate(RING_BUF   *p_ring_buf,
                   MEM_SEG    *p_mem_seg,
                   CPU_INT16U buf_size,
                   RTOS_ERR   *p_err)
{
  RTOS_ASSERT_DBG_ERR_SET((buf_size >= 128u), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((buf_size < 32767u), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_ring_buf->StartPtr = (CPU_INT08U *)Mem_SegAlloc("Ring Buf Data",
                                                    p_mem_seg,
                                                    buf_size,
                                                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_ring_buf->Size = buf_size;
  p_ring_buf->RdIx = 0u;
  p_ring_buf->RdIxNext = 0u;
  p_ring_buf->WrIx = 0u;
  p_ring_buf->WrIxPrev = 0u;
  p_ring_buf->EndValidIx = 0u;
  p_ring_buf->IsReading = DEF_NO;
  p_ring_buf->RdIxDirty = DEF_NO;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                               RingBufWrAlloc()
 *
 * @brief    Allocate space to write data in the ring buffer.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @param    size        Size to allocate, in bytes. MUST be greater than 0.
 *
 * @return   Pointer to buffer location where to write, if NO error(s),
 *           DEF_NULL,                                  otherwise.
 *
 * @note     (1) This function MUST be called from a CRITICAL SECTION, the same in which the call to
 *               RingBufWrCommit() will be done.
 *******************************************************************************************************/
void *RingBufWrAlloc(RING_BUF   *p_ring_buf,
                     CPU_INT16U size)
{
  CPU_INT32U wr_size = size + RING_BUF_IX_SIZE_BYTES;           // Account for ix, at end of packet.

  //                                                               Make sure ring buf has been created.
  if (RING_BUF_IS_NULL(p_ring_buf) == DEF_YES) {
    return (DEF_NULL);
  }

  RTOS_ASSERT_DBG((size > 0u), RTOS_ERR_INVALID_ARG, DEF_NULL);
  RTOS_ASSERT_DBG((wr_size <= p_ring_buf->Size), RTOS_ERR_INVALID_ARG, DEF_NULL);

  if ((p_ring_buf->WrIx < p_ring_buf->RdIx)
      && ((p_ring_buf->WrIx + wr_size) >= p_ring_buf->RdIx)) {  // Wr is overwriting at rd location.
    if (p_ring_buf->IsReading == DEF_NO) {                      // OK only if no other task is currently reading.
      p_ring_buf->RdIxDirty = DEF_YES;                          // Indicate the rd ix will need to be re-calculated.
    } else {
      return (DEF_NULL);
    }
  }

  if ((p_ring_buf->WrIx + wr_size) > p_ring_buf->Size) {        // No more room at end of ring buf.
                                                                // Re-check is needed, after wr ix adjusted for buf len.
    if (wr_size >= p_ring_buf->RdIx) {                          // Wr is overwriting at rd location.
      if (p_ring_buf->IsReading == DEF_NO) {
        p_ring_buf->RdIxDirty = DEF_YES;                        // Indicate the rd ix will need to be re-calculated.
      } else {
        return (DEF_NULL);
      }
    }
    p_ring_buf->EndValidIx = p_ring_buf->WrIx - 1u;             // Indicate where valid data ends.
    p_ring_buf->WrIx = 0u;                                      // Re-set wr ix to start of buf.
  }

  p_ring_buf->WrIxPrev = p_ring_buf->WrIx;

  p_ring_buf->WrIx += size;

  return ((void *)&(p_ring_buf->StartPtr[p_ring_buf->WrIxPrev]));
}

/****************************************************************************************************//**
 *                                               RingBufWrCommit()
 *
 * @brief    Commit write data to ring buffer. After this call, no other writes should be done, unless
 *           a new call to RingBufWrAlloc() is made.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @note     (1) This function MUST be called from a CRITICAL SECTION, the same in which the call to
 *               RingBufWrAlloc() has been done.
 *******************************************************************************************************/
void RingBufWrCommit(RING_BUF *p_ring_buf)
{
  //                                                               Calculate and keep prev wr ix in ring buf, to ...
  //                                                               re-calculate rd ix in case of overwrite.
  MEM_VAL_SET_INT16U(&p_ring_buf->StartPtr[p_ring_buf->WrIx], p_ring_buf->WrIxPrev);

  p_ring_buf->WrIx += RING_BUF_IX_SIZE_BYTES;
  //                                                               If the end valid is 'pushed', adjust val.
  p_ring_buf->EndValidIx = DEF_MAX(p_ring_buf->EndValidIx, (p_ring_buf->WrIx - 1u));
}

/****************************************************************************************************//**
 *                                               RingBufWr()
 *
 * @brief    Wrapper function that calls both RingBufWrAlloc() and RingBufWrCommit(), in a critical
 *           section.
 *
 * @param    p_ring_buf      Pointer to ring buffer structure to use.
 *
 * @param    size            Size of the data to write in the ring buffer, in bytes.
 *
 * @param    p_start_data    Pointer to start of data to write in the ring buffer.
 *
 * @return   DEF_OK    if write was successful,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN RingBufWr(RING_BUF   *p_ring_buf,
                      CPU_INT16U size,
                      void       *p_start_data)
{
  void        *p_start_buf;
  CPU_BOOLEAN ret_val = DEF_FAIL;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_start_buf = RingBufWrAlloc(p_ring_buf,
                               size);
  if (p_start_buf != DEF_NULL) {
    ret_val = DEF_OK;

    Mem_Copy(p_start_buf, p_start_data, size);

    RingBufWrCommit(p_ring_buf);
  }

  CORE_EXIT_ATOMIC();

  return (ret_val);
}

/****************************************************************************************************//**
 *                                           RingBufDataIsAvail()
 *
 * @brief    See if there is any data that is available, in the ring buffer.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @return   DEF_YES,  if there is data that is available to read in the ring buffer,
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN RingBufDataIsAvail(RING_BUF *p_ring_buf)
{
  CPU_BOOLEAN ret_val = DEF_NO;
  CORE_DECLARE_IRQ_STATE;

  if (RING_BUF_IS_NULL(p_ring_buf) == DEF_YES) {
    return (ret_val);
  }

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((p_ring_buf->IsReading == DEF_NO), RTOS_ERR_ASSERT_CRITICAL_FAIL, ret_val);

  if ((p_ring_buf->RdIxDirty == DEF_YES)
      || (p_ring_buf->WrIx != p_ring_buf->RdIx)) {
    ret_val = DEF_YES;
  }
  CORE_EXIT_ATOMIC();

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               RingBufRdStart()
 *
 * @brief    Start reading from the next available read location, in the ring buffer.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @return   DEF_OK    if there is data to read in the ring buffer,
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) This function must be called before any call to RingBufRd() is made. Once finished
 *               reading a packet, a corresponding RingBufRdEnd() must be called.
 *******************************************************************************************************/
CPU_BOOLEAN RingBufRdStart(RING_BUF *p_ring_buf)
{
  CPU_BOOLEAN ret_val = DEF_FAIL;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((p_ring_buf->IsReading == DEF_NO), RTOS_ERR_ASSERT_CRITICAL_FAIL, ret_val);

  if (p_ring_buf->RdIxDirty == DEF_YES) {
    p_ring_buf->RdIxDirty = DEF_NO;
    RingBufRdIxNextRefresh(p_ring_buf);
  } else if (p_ring_buf->WrIx == p_ring_buf->RdIx) {
    goto end;
  }

  p_ring_buf->IsReading = DEF_YES;
  ret_val = DEF_OK;

end:
  CORE_EXIT_ATOMIC();

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               RingBufRd()
 *
 * @brief    Read from the next available read location, in the ring buffer.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @param    size        Size read from the buffer.
 *
 * @return   Pointer to buffer where to read, if NO error(s),
 *           DEF_NULL,                        otherwise.
 *
 * @note     (1) RingBufRdStart() must be called before making any call to this function. This
 *               function can be called any number of times to read a SINGLE packet from the buffer.
 *               Once finished reading a packet, a single call to RingBufRdEnd() must be made.
 *******************************************************************************************************/
void *RingBufRd(RING_BUF   *p_ring_buf,
                CPU_SIZE_T size)
{
  void *ret_val = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((p_ring_buf->IsReading == DEF_YES), RTOS_ERR_ASSERT_CRITICAL_FAIL, ret_val);

  //                                                               Rd attempts to go beyond wr location.
  RTOS_ASSERT_CRITICAL(!((p_ring_buf->RdIxNext < p_ring_buf->WrIx)
                         && ((p_ring_buf->RdIxNext + size) > p_ring_buf->WrIx)), RTOS_ERR_ASSERT_CRITICAL_FAIL, ret_val);

  //                                                               No more room at end of ring buf.
  if ((p_ring_buf->RdIxNext + size) > p_ring_buf->EndValidIx) {
    p_ring_buf->RdIxNext = 0u;                                  // Re-set rd ix to start of buf.

    //                                                             Re-check is needed, after rd ix adjusted for buf len.
    //                                                             Rd attempts to go beyond wr location.
    RTOS_ASSERT_CRITICAL(!((p_ring_buf->RdIxNext + size) > p_ring_buf->WrIx), RTOS_ERR_ASSERT_CRITICAL_FAIL, ret_val);
  }

  p_ring_buf->RdIx = p_ring_buf->RdIxNext;                      // Update, in case RdStart is called more than once before End is called.
  p_ring_buf->RdIxNext = p_ring_buf->RdIx + size;

  ret_val = &p_ring_buf->StartPtr[p_ring_buf->RdIx];

  CORE_EXIT_ATOMIC();

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               RingBufRdEnd()
 *
 * @brief    Finish reading from the ring buffer.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @note     (1) This function must be called after the call(s) to RingBufRd() is/are made and a
 *               complete packet has been read.
 *******************************************************************************************************/
void RingBufRdEnd(RING_BUF *p_ring_buf)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((p_ring_buf->IsReading == DEF_YES), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  p_ring_buf->IsReading = DEF_NO;
  p_ring_buf->RdIxNext += RING_BUF_IX_SIZE_BYTES;
  p_ring_buf->RdIx = p_ring_buf->RdIxNext;
  CORE_EXIT_ATOMIC();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           RingBufRdIxNextRefresh()
 *
 * @brief    Calculates and updates the RdIxNext if the rd location had been overwritten by a write.
 *
 * @param    p_ring_buf  Pointer to ring buffer structure to use.
 *
 * @note     (1) This function MUST be called from a CRITICAL SECTION, the same in which the
 *               'IsReading' flag is set to DEF_YES.
 *******************************************************************************************************/
static void RingBufRdIxNextRefresh(RING_BUF *p_ring_buf)
{
  CPU_INT16U rd_ix;
  CPU_INT16U prev_rd_ix;
  CPU_INT16U wr_ix;

  if (p_ring_buf->WrIx > p_ring_buf->EndValidIx) {              // WrIx is at the end of the buf. Rd should start at 0.
    p_ring_buf->RdIxNext = 0u;
    return;
  }

  wr_ix = p_ring_buf->WrIx;
  rd_ix = wr_ix;

  while (DEF_YES) {
    if (rd_ix != 0u) {
      prev_rd_ix = rd_ix;
      rd_ix = MEM_VAL_GET_INT16U(&p_ring_buf->StartPtr[rd_ix - RING_BUF_IX_SIZE_BYTES]);
    } else {                                                    // RdIx re-starts at end of buf.
      rd_ix = MEM_VAL_GET_INT16U(&p_ring_buf->StartPtr[p_ring_buf->EndValidIx - 1u]);
      //                                                           Indicate prev_rd_ix has looped.
      prev_rd_ix = p_ring_buf->EndValidIx;
    }

    if (rd_ix == wr_ix) {                                       // If equal, rd packet is considered lost, since ...
                                                                // equal ix should mean an empty buf.
      p_ring_buf->RdIxNext = prev_rd_ix;
      return;
    }
    if ((prev_rd_ix > wr_ix)
        && (rd_ix < wr_ix)) {
      p_ring_buf->RdIxNext = prev_rd_ix;
      return;
    }

    if ((rd_ix > wr_ix)
        && (((CPU_INT16U)(rd_ix - wr_ix)) <= RING_BUF_IX_SIZE_BYTES)) {
      //                                                           Stop if RdIx obtained is within 2 bytes of WrIx. ...
      //                                                           That means that the prev ix cannot be computed   ...
      //                                                           and could be corrupted.
      p_ring_buf->RdIxNext = rd_ix;
      return;
    }
  }
}
