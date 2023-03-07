/***************************************************************************//**
 * @file
 * @brief File System - Nand Memory Controller - Nxp K70 Mcu
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

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#if  defined(RTOS_MODULE_FS_AVAIL) && defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/source/logging/logging_priv.h>

#include  <fs/include/fs_nand.h>
#include  <fs/source/storage/nand/fs_nand_ctrlr_gen_priv.h>

//                                                                 Third Party Library Includes.
#include  <em_ebi.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR            RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                (FS, DRV, NAND)

//                                                                 -------- MEMORY-MAPPED REG SELECTION DEFINES -------
#define  FS_NAND_ALE_SELECT                    DEF_BIT_24       // Address Latch Bit is found on 24th Bit of Data Reg.
#define  FS_NAND_CLE_SELECT                    DEF_BIT_25       // Command Latch Bit is found on 25th Bit of Data Reg.

//                                                                 ------------------ TIMEOUT DEFINES -----------------
#define  FS_NAND_WAIT_TIMEOUT_VAL              0xFFFFFFFFu      // Timeout Error Value for "Wait While Busy" Function.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void FS_NAND_CtrlrDrv_Open(FS_NAND_CTRLR_DRV *p_drv,
                                  RTOS_ERR          *p_err);

static void FS_NAND_CtrlrDrv_Close(FS_NAND_CTRLR_DRV *p_drv);

static void FS_NAND_CtrlrDrv_ChipSelEn(FS_NAND_CTRLR_DRV *p_drv);

static void FS_NAND_CtrlrDrv_ChipSelDis(FS_NAND_CTRLR_DRV *p_drv);

static void FS_NAND_CtrlrDrv_CmdWr(FS_NAND_CTRLR_DRV *p_drv,
                                   CPU_INT08U        *p_cmd,
                                   CPU_SIZE_T        cnt,
                                   RTOS_ERR          *p_err);

static void FS_NAND_CtrlrDrv_AddrWr(FS_NAND_CTRLR_DRV *p_drv,
                                    CPU_INT08U        *p_addr,
                                    CPU_SIZE_T        cnt,
                                    RTOS_ERR          *p_err);

static void FS_NAND_CtrlrDrv_DataWr(FS_NAND_CTRLR_DRV *p_drv,
                                    void              *p_src,
                                    CPU_SIZE_T        cnt,
                                    CPU_INT08U        bus_width,
                                    RTOS_ERR          *p_err);

static void FS_NAND_CtrlrDrv_DataRd(FS_NAND_CTRLR_DRV *p_drv,
                                    void              *p_dest,
                                    CPU_SIZE_T        cnt,
                                    CPU_INT08U        bus_width,
                                    RTOS_ERR          *p_err);

static void FS_NAND_CtrlrDrv_WaitWhileBusy(FS_NAND_CTRLR_DRV *p_drv,
                                           void              *poll_fnct_arg,
                                           CPU_BOOLEAN (*poll_fnct)(void *p_arg),
                                           CPU_INT32U        to_us,
                                           RTOS_ERR          *p_err);

/********************************************************************************************************
 *                                           INTERFACE STRUCTURE
 *******************************************************************************************************/

const FS_NAND_CTRLR_DRV_API FS_NAND_CtrlDrvAPI_Silabs_EFM32GG_EBI = {
  .Open = FS_NAND_CtrlrDrv_Open,
  .Close = FS_NAND_CtrlrDrv_Close,
  .ChipSelEn = FS_NAND_CtrlrDrv_ChipSelEn,
  .ChipSelDis = FS_NAND_CtrlrDrv_ChipSelDis,
  .CmdWr = FS_NAND_CtrlrDrv_CmdWr,
  .AddrWr = FS_NAND_CtrlrDrv_AddrWr,
  .DataWr = FS_NAND_CtrlrDrv_DataWr,
  .DataRd = FS_NAND_CtrlrDrv_DataRd,
  .WaitWhileBusy = FS_NAND_CtrlrDrv_WaitWhileBusy
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrDrv_Open()
 *
 * @brief    Open (initialize) the NAND memory controller.
 *
 * @param    p_drv   Pointer to driver data.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) This function will be called EVERY time the device is opened.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_Open(FS_NAND_CTRLR_DRV *p_drv,
                                  RTOS_ERR          *p_err)
{
  FS_NAND_CTRLR_BSP_API *p_bsp_api = (FS_NAND_CTRLR_BSP_API *)p_drv->HW_InfoPtr->BspApiPtr;
  CPU_BOOLEAN           ok = DEF_FAIL;

  //                                                               ----------------- GENERAL BSP INIT -----------------
  if (p_bsp_api->Init != DEF_NULL) {
    ok = p_bsp_api->Init(DEF_NULL,                              // DEF_NULL = no intr used by this NAND drv.
                         p_drv);
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }
  //                                                               Cfg clock(s) required by NAND ctrlr.
  if (p_bsp_api->ClkCfg != DEF_NULL) {
    ok = p_bsp_api->ClkCfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }
  //                                                               Cfg I/O needed by NAND ctrlr if necessary.
  if (p_bsp_api->IO_Cfg != DEF_NULL) {
    ok = p_bsp_api->IO_Cfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }
  //                                                               Cfg interrupt controller if necessary.
  if (p_bsp_api->IntCfg != DEF_NULL) {
    ok = p_bsp_api->IntCfg();
    if (ok != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
      return;
    }
  }

  DEF_BIT_CLR(EBI->CMD, EBI_CMD_ECCSTART);                      // Disable MCU-Side ECC Generation.
  DEF_BIT_SET(EBI->NANDCTRL, EBI_NANDCTRL_EN);                  // Enable NAND Flash Control for the Memory Bank.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrDrv_Close()
 *
 * @brief    Close (uninitialize) the NAND memory controller.
 *
 * @param    p_drv   Pointer to driver data.
 *
 * @note     (1) This function will be called EVERY time the device is closed.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_Close(FS_NAND_CTRLR_DRV *p_drv)
{
  PP_UNUSED_PARAM(p_drv);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrDrv_ChipSelEn()
 *
 * @brief    Enable NAND chip select/enable.
 *
 * @param    p_drv   Pointer to driver data.
 *
 * @note     (1) If you are sharing the bus/hardware with other software, this function MUST get an
 *               exclusive resource lock and configure any peripheral that could have been setup
 *               differently by the other software.
 *
 * @note     (2) The NAND chip enable signal can be controlled via a memory controller's register or
 *               via a GPIO register. In the case of a GPIO register, the function should
 *               call the BSP function ChipSelEn() to drive the signal properly.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_ChipSelEn(FS_NAND_CTRLR_DRV *p_drv)
{
  FS_NAND_CTRLR_BSP_API *p_bsp_api = (FS_NAND_CTRLR_BSP_API *)p_drv->HW_InfoPtr->BspApiPtr;
  CPU_BOOLEAN           ok = DEF_FAIL;

  //                                                               En chip sel.
  if (p_bsp_api->ChipSelEn != DEF_NULL) {
    ok = p_bsp_api->ChipSelEn(p_drv->PartSlaveID);
    if (ok != DEF_OK) {
      LOG_ERR(("Enabling NAND chip select failed"));
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrDrv_ChipSelDis()
 *
 * @brief    Disable NAND chip select/enable.
 *
 * @param    p_drv   Pointer to driver data.
 *
 * @note     (1) If you are sharing the bus/hardware with other software, this function MUST release
 *               the exclusive resource lock obtained in function FS_NAND_CtrlrDrv_ChipSelEn().
 *
 * @note     (2) The NAND chip enable signal can be controlled via a memory controller's register or
 *               via a GPIO register. In the case of a GPIO register, the function should
 *               call the BSP function ChipSelDis() to drive the signal properly.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_ChipSelDis(FS_NAND_CTRLR_DRV *p_drv)
{
  FS_NAND_CTRLR_BSP_API *p_bsp_api = (FS_NAND_CTRLR_BSP_API *)p_drv->HW_InfoPtr->BspApiPtr;
  CPU_BOOLEAN           ok = DEF_FAIL;

  //                                                               Dis chip sel.
  if (p_bsp_api->ChipSelDis != DEF_NULL) {
    ok = p_bsp_api->ChipSelDis(p_drv->PartSlaveID);
    if (ok != DEF_OK) {
      LOG_ERR(("Disabling NAND chip select failed"));
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrDrv_CmdWr()
 *
 * @brief    Write command to NAND.
 *
 * @param    p_drv   Pointer to driver data.
 *
 * @param    p_cmd   Pointer to buffer that holds command.
 *
 * @param    cnt     Number of octets to write.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) A NAND transaction is composed of the following phases:
 *               - (a) Command cycle 1
 *               - (b) Address cycles
 *               - (c) Command cycle 2
 *               - (d) Data cycles (IN or OUT)
 *
 * @note     (2) This function must send 'cnt' octets on the data bus of the NAND. The pins of this
 *               bus are usually labeled IO0_0 - IO7_0 or DQ0_0 - DQ7_0 in the datasheet of the
 *               NAND flash, as specified by the ONFI 3.0 specification. The CLE (Command Latch Enable)
 *               pin must also be set high to indicate the data is a command.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_CmdWr(FS_NAND_CTRLR_DRV *p_drv,
                                   CPU_INT08U        *p_cmd,
                                   CPU_SIZE_T        cnt,
                                   RTOS_ERR          *p_err)
{
  CPU_INT32U *p_nand_cmd;
  CPU_INT32U flash_base_addr = p_drv->HW_InfoPtr->FlashMemMapStartAddr;

  //                                                               Memory-Mapped Address w/ CLE Bit to Write to Cmd Reg.
  p_nand_cmd = (CPU_INT32U *)(flash_base_addr + FS_NAND_CLE_SELECT);

  Mem_Copy((void *)p_nand_cmd, (void *)p_cmd, cnt);             // Write Command to NAND Mem Location, with needed 'cnt'

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrDrv_AddrWr()
 *
 * @brief    Write address to NAND.
 *
 * @param    p_drv   Pointer to driver data.
 *
 * @param    p_addr  Pointer to buffer that holds address.
 *
 * @param    cnt     Number of octets to write (size of the address).
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) A NAND transaction is composed of the following phases:
 *               - (a) Command cycle 1
 *               - (b) Address cycles
 *               - (c) Command cycle 2
 *               - (d) Data cycles (IN or OUT)
 *
 * @note     (2) This function must send 'cnt' octets on the data bus of the NAND. The pins of this
 *               bus are usually labeled IO0_0 - IO7_0 or DQ0_0 - DQ7_0 in the datasheet of the
 *               NAND flash, as specified by the ONFI 3.0 specification. The ALE (Address Latch Enable)
 *               pin must also be set high to indicate the data is an address.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_AddrWr(FS_NAND_CTRLR_DRV *p_drv,
                                    CPU_INT08U        *p_addr,
                                    CPU_SIZE_T        cnt,
                                    RTOS_ERR          *p_err)
{
  CPU_INT32U *p_nand_addr;
  CPU_INT32U flash_base_addr = p_drv->HW_InfoPtr->FlashMemMapStartAddr;

  //                                                               Memory-Mapped Address w/ ALE Bit to Write to Addr Reg
  p_nand_addr = (CPU_INT32U *)(flash_base_addr + FS_NAND_ALE_SELECT);

  Mem_Copy((void *)p_nand_addr, (void *)p_addr, cnt);           // Write Address to NAND Mem Location, with needed 'cnt'

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrDrv_DataWr()
 *
 * @brief    Write data to NAND.
 *
 * @param    p_drv       Pointer to driver data.
 *
 * @param    p_src       Pointer to source memory buffer.
 *
 * @param    cnt         Number of octets to write.
 *
 * @param    bus_width   Parallel bus width. Usually 8 or 16 bits.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) A NAND transaction is composed of the following phases:
 *               - (a) Command cycle 1
 *               - (b) Address cycles
 *               - (c) Command cycle 2
 *               - (d) Data cycles (IN or OUT)
 *
 * @note     (2) This function must send 'cnt' octets on the data bus of the NAND. The pins of this
 *               bus are usually labeled IO0_0 - IO7_0 or DQ0_0 - DQ7_0 in the datasheet of the
 *               NAND flash, as specified by the ONFI 3.0 specification. The CLE (Command Latch Enable)
 *               and ALE (Address Latch Enabled) pins must both be set low.
 *
 * @note     (3) When calling this function, the generic controller layer has already sent the proper
 *               command and address to the NAND Flash. The purpose of this function is only to write
 *               data at a specific address.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_DataWr(FS_NAND_CTRLR_DRV *p_drv,
                                    void              *p_src,
                                    CPU_SIZE_T        cnt,
                                    CPU_INT08U        bus_width,
                                    RTOS_ERR          *p_err)
{
  CPU_INT32U *p_nand_data;
  CPU_INT32U flash_base_addr = p_drv->HW_InfoPtr->FlashMemMapStartAddr;

  PP_UNUSED_PARAM(bus_width);                                   // NAND BSP Pre-Configured to 8-Bit Bus Width.

  p_nand_data = (CPU_INT32U *)flash_base_addr;                  // Memory-Mapped Address w/ no Bits to Write to Data Reg

  Mem_Copy((void *)p_nand_data, p_src, cnt);                    // Write Source Data to NAND Mem Location, w/ 'cnt'

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           FS_NAND_CtrlrDrv_DataRd()
 *
 * @brief    Read data from NAND.
 *
 * @param    p_drv       Pointer to driver data.
 *
 * @param    p_dest      Pointer to destination memory buffer.
 *
 * @param    cnt         Number of octets to read.
 *
 * @param    bus_width   Parallel bus width. Usually 8 or 16 bits.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) A NAND transaction is composed of the following phases:
 *               - (a) Command cycle 1
 *               - (b) Address cycles
 *               - (c) Command cycle 2
 *               - (d) Data cycles (IN or OUT)
 *
 * @note     (2) This function must read 'cnt' octets on the data bus of the NAND. The pins of this
 *               bus are usually labeled IO0_0 - IO7_0 or DQ0_0 - DQ7_0 in the datasheet of the
 *               NAND flash, as specified by the ONFI 3.0 specification.
 *
 * @note     (3) When calling this function, the generic controller layer has already sent the proper
 *               command and address to the NAND Flash. The purpose of this function is only to read
 *               data at a specific address.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_DataRd(FS_NAND_CTRLR_DRV *p_drv,
                                    void              *p_dest,
                                    CPU_SIZE_T        cnt,
                                    CPU_INT08U        bus_width,
                                    RTOS_ERR          *p_err)
{
  CPU_INT32U *p_nand_data;
  CPU_INT32U flash_base_addr = p_drv->HW_InfoPtr->FlashMemMapStartAddr;

  PP_UNUSED_PARAM(bus_width);                                   // NAND BSP Pre-Configured to 8-Bit Bus Width.

  p_nand_data = (CPU_INT32U *)flash_base_addr;                  // Memory-Mapped Addr w/ no Bits to Read from Data Reg.

  Mem_Copy(p_dest, (void *)p_nand_data, cnt);                   // Read Data from NAND Mem Location to Destination.

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       FS_NAND_CtrlrDrv_WaitWhileBusy()
 *
 * @brief    Wait while NAND is busy.
 *
 * @param    p_drv           Pointer to driver data.
 *
 * @param    poll_fnct       Pointer to function to poll, if there is no hardware ready/busy signal.
 *
 * @param    poll_fnct_arg   Pointer to argument that must be passed to the poll_fnct, if needed.
 *
 * @param    to_us           Timeout, in microseconds.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This template show how to implement this function using the 'poll_fnct'. However,
 *               reading the appropriate register should be more efficient, if available. In this
 *               case, you should replace all the code in the function.
 *
 * @note     (2) Usually the NAND flash device has a Ready/Busy signal that is asserted low when an
 *               ERASE, PROGRAM or READ operation is in progress.
 *
 * @note     (3) The NAND chip's Ready/Busy signal can be monitored via a memory controller's
 *               register or via a GPIO register. In the case of a GPIO register, the function should
 *               call the BSP function IsChipRdy() to know if the chip is ready for a new command.
 *******************************************************************************************************/
static void FS_NAND_CtrlrDrv_WaitWhileBusy(FS_NAND_CTRLR_DRV *p_drv,
                                           void              *poll_fnct_arg,
                                           CPU_BOOLEAN (*poll_fnct)(void *p_arg),
                                           CPU_INT32U        to_us,
                                           RTOS_ERR          *p_err)
{
  FS_NAND_CTRLR_BSP_API *p_bsp_api = (FS_NAND_CTRLR_BSP_API *)p_drv->HW_InfoPtr->BspApiPtr;
  CPU_INT32U            timeout_val;
  CPU_BOOLEAN           rdy = DEF_NO;

  PP_UNUSED_PARAM(poll_fnct_arg);
  PP_UNUSED_PARAM(poll_fnct);
  PP_UNUSED_PARAM(to_us);

  timeout_val = FS_NAND_WAIT_TIMEOUT_VAL;                       // Init Var(s).

  //                                                               Wait until the EBI AHB Transaction is Done. Used...
  //                                                               ... when 'Write Buffer' is Enabled.
  while ((DEF_BIT_IS_SET(EBI->STATUS, EBI_STATUS_AHBACT) == DEF_YES)
         && (timeout_val > 0u)) {
    timeout_val--;
  }

  if (timeout_val == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);                     // Timeout Occurred, EBI AHB Transaction didn't finish.
    return;
  }

  timeout_val = FS_NAND_WAIT_TIMEOUT_VAL;                       // Reset the Timeout Value.

  //                                                               Wait until the Ready/Busy Pin becomes 'HIGH', NAND...
  //                                                               ... has finished transaction.
  rdy = p_bsp_api->IsChipRdy();
  while ((rdy == DEF_NO)
         && (timeout_val > 0u)) {
    timeout_val--;
    rdy = p_bsp_api->IsChipRdy();
  }

  if (timeout_val == 0u) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);                     // Timeout Occurred, NAND Transaction didn't finish.
    return;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL && RTOS_MODULE_FS_STORAGE_NAND_AVAIL
