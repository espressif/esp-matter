/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
/*******************************************************************************
 * iss/xf-board.h
 *******************************************************************************/

#ifndef __XF_H
#error "xf-board.h mustn't be included directly"
#endif

/*******************************************************************************
 * System includes
 ******************************************************************************/
#include "xos-msgq-if.h"

/*******************************************************************************
 * Global constants definitions
 ******************************************************************************/

/* ...proxy shared memory address */
#define XF_CFG_SHMEM_ADDRESS(core)      0x06000000

/* ...inter-processor interrupt number (8(HiFi) or 2) */
#define XF_PROXY_IPI_NUM(core)          XCHAL_EXTINT8_NUM

/* ...tracing buffer configuration (place at the end of memory) */
#define XF_CFG_TRACE_START(core)           0x06800000
#define XF_CFG_TRACE_END(core)             0x07000000

/*******************************************************************************
 * Public proxy API
 ******************************************************************************/

/* ...notify remote side about status change */
static inline void XF_PROXY_NOTIFY_PEER(UWORD32 core)
{
    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);
    XosEvent *msgq_event = ro->ipc.msgq_event;
        
    if (core == 0)
    {
        xos_event_set(msgq_event, RESP_MSGQ_READY);
    }
    else 
    {
        /* ...multiple DSPs case, use IPI? - tbd */
    }
}

/* ...clear pending interrupt mask - do nothing for iss */
#define XF_PROXY_SYNC_PEER(core)        

/*******************************************************************************
 * Auxiliary helper functions
 ******************************************************************************/

/* ...memory protection? - tbd */
static inline void write_dtlb_entry (int entry, int way)
{
    asm volatile("wdtlb %0, %1; dsync\n" : : "r" (entry), "r" (way) );
}
static inline void write_dtlbcfg (int cfg)
{
    asm volatile("wsr.dtlbcfg %0 ; dsync\n" : : "r" (cfg) );
}

/*******************************************************************************
 * Board specific init
 ******************************************************************************/

static inline void xf_board_init(void)
{
#if 0
    /*  For Shabu setup only!:  */
#if XCHAL_HW_CONFIGID0 == 0xC1B3DBFE && XCHAL_HAVE_PTP_MMU && XCHAL_HAVE_SPANNING_WAY
    /*
     *  Mask addresses 0x0000_0000 thru 0x05FF_FFFF (96MB).
     *
     *  Have to replace existing 512MB mapping at 0x0000_0000..0x1FFF_FFFF
     *  to do this (with way 5 128MB mapping at 0x0800_0000..0x0FFF_FFFF,
     *  and way 4 16MB mapping at 0x0600_0000..0x07FF_FFFF).  This takes
     *  assembly sequence to do.
     *  For now, cheat and just map multihits with 16MB pages of way 4 at:
     *		0x0000_0000..0x00FF_FFFF (entry 0)
     *		0x0500_0000..0x05FF_FFFF (entry 1)
     *		0x0200_0000..0x02FF_FFFF (entry 2)
     *		0x0300_0000..0x03FF_FFFF (entry 3)
     */
    write_dtlbcfg(0x00020000);	/* way 4 = 16MB, others = default */
    write_dtlb_entry(0xD000000C, 0x00000004);	/* way 4, 00xx_xxxx => illegal */
    write_dtlb_entry(0xD000000C, 0x05000004);	/* way 4, 05xx_xxxx => illegal */
    write_dtlb_entry(0xD000000C, 0x02000004);	/* way 4, 02xx_xxxx => illegal */
    write_dtlb_entry(0xD000000C, 0x03000004);	/* way 4, 03xx_xxxx => illegal */
#endif
#endif
}
