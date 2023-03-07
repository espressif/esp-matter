/* Copyright Statement:
 *
 * (C) 2005-2021  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */


/****************************************************************************
 *
 * HEADER FILES
 *
 ****************************************************************************/

#include <stdio.h>
#include "LZMA_decoder.h"
#include "lzma_decode_interface.h"
#include "hal_flash.h"

#if defined(BL_FOTA_ENABLE) || (MOD_CFG_FOTA_BL_RESERVED)
#include "hal_wdt.h"
#endif


/****************************************************************************
 *
 * CONSTANTS AND MACROS
 *
 ****************************************************************************/


#if (defined(__ICCARM__) || defined(__CC_ARM)) && !defined(LZMA_LOG)
#define LZMA_LOG(...) do {} while (0)
#endif

#if defined(MTK_FOTA_ON_7687) && !defined(LZMA_LOG)
#define LZMA_LOG(...)    printf(__VA_ARGS__)
#endif

#if defined(UT)
extern void hw_uart_printf( const char *str, ... );
#define LZMA_LOG(fmt...) do { \
        hw_uart_printf("%s %d: ", __func__, __LINE__); \
        hw_uart_printf(fmt); \
        fflush(NULL); \
    } while (0)
#endif

#define LZMA_LOG(...) do {} while (0)

#if defined(MTK_FOTA_V3_ENABLE) && !defined(LZMA_LOG)
//#define LZMA_LOG(...) do {} while (0)
extern void hw_uart_printf( const char *str, ... );
#define LZMA_LOG(fmt...) do { \
        hw_uart_printf("%s %d: ", __func__, __LINE__); \
        hw_uart_printf(fmt); \
    } while (0)
#endif

#if defined(BL_FOTA_ENABLE) && !defined(LZMA_LOG)
#include "bl_common.h"
#define LZMA_LOG(...)    BL_PRINT(LOG_DEBUG, __VA_ARGS__)
#endif

#if !defined(LZMA_LOG)
#include "syslog.h"
#define LZMA_LOG(...)    LOG_I(common, __VA_ARGS__)
#endif


#define IN_BUF_SIZE         (1 << 12)
#define OUT_BUF_SIZE        (1 << 12)
#define FLASH_BLOCK_SIZE    (1 << 12)


// default, overriden by MT2523
#define RAM_ADDRESS_BASE 0x00000000


// PRODUCT_VERSION to be defined in chip.mk
#if PRODUCT_VERSION == 2523
#undef  RAM_ADDRESS_BASE
#define RAM_ADDRESS_BASE 0x08000000
#endif


#define IS_ALIGNED( addr ) \
                ( ( ( (uint32_t)addr ) & ( FLASH_BLOCK_SIZE ) ) == 0 )


/****************************************************************************
 *
 * TYPE DECLARATION
 *
 ****************************************************************************/


typedef struct _data_in_t
{
    // memory buffer
    Byte                *buf;
    size_t              idx;
    size_t              len;
    size_t              size;

    // flash address
    uint32_t            addr;
} data_in_t;


typedef struct _data_out_t
{
    // memory buffer
    Byte                *buf;
    size_t              idx;
    size_t              len;
    size_t              size;

    // flash address
    uint32_t            addr;
} data_out_t;


typedef struct _decode_t
{
    data_in_t           in;
    data_out_t          out;

    SizeT               unpacked_size;
} decode_t;


/****************************************************************************
 *
 * PRIVATE FUNCTIONS
 *
 ****************************************************************************/


/* decode from flash to flash */
static hal_flash_status_t Write2Flash(
    uint8_t         *des,
    uint8_t         *buf,
    uint32_t        length)
{
    uint32_t        current_block;
    uint8_t         *current_block_start_addr;
    uint8_t         *end_addr;
    uint32_t        end_block;
    uint32_t        block_idx;
    uint32_t        erase_address;
    hal_flash_status_t flash_op_ret;

    LZMA_LOG("LZMA_write, des = %x\n", des);
    end_addr  = des + length - 1;
    end_block = (uint32_t)end_addr / FLASH_BLOCK_SIZE;

    current_block            = (uint32_t)des / FLASH_BLOCK_SIZE;
    current_block_start_addr = (uint8_t *)(current_block * 0x1000);

    if (current_block_start_addr != des)
    {
        current_block++;
    }

#if 1
    LZMA_LOG("LZMA_write: length = %d\n", length);
    LZMA_LOG("LZMA_write: current_block = %d\n", current_block);
    LZMA_LOG("LZMA_write: end_block = %d\n", end_block);
    LZMA_LOG("LZMA_write: current_block_start_addr = %x\n", current_block_start_addr);
#endif

    for ( block_idx = current_block; block_idx <= end_block; block_idx++)
    {
        LZMA_LOG("LZMA_erase start\n");
        erase_address = block_idx * 0x1000;
        LZMA_LOG("LZMA_erase, addr = %x\n", erase_address - RAM_ADDRESS_BASE);
        flash_op_ret = hal_flash_erase(erase_address - RAM_ADDRESS_BASE, HAL_FLASH_BLOCK_4K);
        //flash_op_ret = 0;
        if (flash_op_ret < 0)
        {
            LZMA_LOG("LZMA_erase fail\n");
            return HAL_FLASH_STATUS_ERROR_ERASE_FAIL;
        }
    }

    LZMA_LOG("LZMA_write, %d bytes to addr 0x%x\n", length, (uint32_t)des - RAM_ADDRESS_BASE);
    flash_op_ret = hal_flash_write((uint32_t)des - RAM_ADDRESS_BASE, buf, length);
    flash_op_ret = 0;
    if (flash_op_ret < 0)
    {
        LZMA_LOG("LZMA_write fail\n");
        return HAL_FLASH_STATUS_ERROR;
    }
    return HAL_FLASH_STATUS_OK;
}


static SRes Decode2Ram(
    CLzmaDec            *state,
    uint8_t             *des,
    uint8_t             *src,
    uint32_t            unpacked_size,
    ISzAlloc            *alloc )
{
    decode_t            ctx;
    hal_flash_status_t  flash_status;
    SRes                lz_status;
    ELzmaStatus         lz_status_ext;
    ELzmaFinishMode     lz_finish_mode;

    ctx.in.idx  = 0;
    ctx.in.len  = 0;
    ctx.in.size = IN_BUF_SIZE;
    ctx.in.addr = ( uint32_t )src - RAM_ADDRESS_BASE;
    ctx.in.buf  = ( Byte * )alloc->Alloc( alloc, ctx.in.size );
    if ( ! ctx.in.buf )
    {
        LZMA_LOG( "in.buf malloc failed!\n" );
        return LZMA_ERROR_MEM;
    }

    ctx.out.addr = ( uint32_t )des & ~( 0x1000 - 1 );
    ctx.out.idx  = 0;
    ctx.out.len  = ( size_t )des & ( 0x1000 - 1 ); // non-aligned
    ctx.out.size = OUT_BUF_SIZE;
    ctx.out.buf  = ( Byte * )alloc->Alloc( alloc, OUT_BUF_SIZE );
    if ( ! ctx.out.buf )
    {
        LZMA_LOG( "out.buf malloc failed!\n" );
        return LZMA_ERROR_MEM;
    }

    LzmaDec_Init( state );

    flash_status = hal_flash_init();
    if ( flash_status < HAL_FLASH_STATUS_OK )
    {
        LZMA_LOG( "lzma: flash fail\n" );
        return LZMA_ERROR_MEM;
    }

    for (;;)
    {
        SizeT consume;
        SizeT produce;

        // pull-in input buffer
        if ( ctx.in.idx != 0 )
        {
            int i;
            LZMA_LOG( "lzma (read): pull-in %d bytes to %d bytes ahead\n",
                      ctx.in.len, ctx.in.idx );
            for ( i = 0; i < (int)ctx.in.len; i++ )
                ctx.in.buf[ i ] = ctx.in.buf[ ctx.in.idx + i ];
            ctx.in.idx = 0;
        }

        // fill input buffer
        if ( ctx.in.len < ctx.in.size )
        {
            LZMA_LOG( "lzma (read): %d bytes from %x\n",
                      ctx.in.size - ctx.in.len, ctx.in.addr );

            flash_status = hal_flash_read( ctx.in.addr,
                                          &ctx.in.buf[ ctx.in.len ],
                                           ctx.in.size - ctx.in.len );

            if ( flash_status != HAL_FLASH_STATUS_OK )
            {
                LZMA_LOG( "lzma (read): flash status %d\n", flash_status );
                lz_status = LZMA_ERROR_READ;
                break;
            }

            ctx.in.addr += ctx.in.size - ctx.in.len;
            ctx.in.len   = ctx.in.size;
        }

        // decode
        if ( ctx.in.len > 0 && ctx.out.size - ctx.out.len > 0 )
        {
            consume = ctx.in.len;
            produce = ctx.out.size - ctx.out.len;

            LZMA_LOG( "lzma (decode): input %d bytes, buffer %d bytes, remain %d\n",
                      consume, produce, unpacked_size );

            if ( produce >= unpacked_size )
            {
                produce = unpacked_size;
                lz_finish_mode = LZMA_FINISH_END;
            }
            else
            {
                lz_finish_mode = LZMA_FINISH_ANY;
            }

            lz_status = LzmaDec_DecodeToBuf( state,
                                             ctx.out.buf + ctx.out.len,
                                             &produce,
                                             ctx.in.buf,
                                             &consume,
                                             lz_finish_mode, &lz_status_ext );

            LZMA_LOG( "lzma (decode): consumed %d bytes, produced %d bytes\n",
                      consume, produce );

            if ( lz_status != SZ_OK )
            {
                LZMA_LOG( "lzma (decode): decode error %d %d\n",
                          lz_status, lz_status_ext );
                break;
            }

            ctx.in.idx    += consume;
            ctx.in.len    -= consume;
            ctx.out.len   += produce;
            unpacked_size -= produce;
        }

        // write output buffer
        if ( ctx.out.len > 0 )
        {
            LZMA_LOG( "lzma (write): %d bytes to 0x%x\n",
                      ctx.out.len, ctx.out.addr );
                      
            flash_status = Write2Flash( (uint8_t *)ctx.out.addr,
                                                   ctx.out.buf,
                                                   ctx.out.len );

            if ( flash_status != HAL_FLASH_STATUS_OK )
            {
                LZMA_LOG( "LZMA_write_end: flash error %d\n", flash_status );
                lz_status = LZMA_ERROR_WRITE;
                break;
            }

            ctx.out.addr += ctx.out.len;
            ctx.out.len   = 0;
            ctx.out.idx   = 0;
        }

        if ( consume == 0 && produce == 0 )
        {
            if ( lz_status_ext != LZMA_STATUS_FINISHED_WITH_MARK )
                lz_status = SZ_ERROR_DATA;
            break;
        }

        if ( unpacked_size == 0 )
        {
            lz_status = LZMA_OK;
            break;
        }

#if defined(BL_FOTA_ENABLE) || (MOD_CFG_FOTA_BL_RESERVED)
        hal_wdt_feed(HAL_WDT_FEED_MAGIC);
#endif
    }

    alloc->Free( alloc, ctx.in.buf  );
    alloc->Free( alloc, ctx.out.buf );

    return lz_status;
}


/****************************************************************************
 *
 * PUBLIC FUNCTIONS
 *
 ****************************************************************************/


LZMA_ret lzma_decode(
    uint8_t         *destination,
    uint32_t        *destination_length,
    const uint8_t   *source,
    uint32_t        *source_length,
    const uint8_t   *compression_property,
    uint32_t        compression_property_size,
    lzma_alloc_t    *lzma_alloc )
{
    SRes            sres_ret                 = SZ_OK;
    LZMA_ret        lzma_ret                 = LZMA_OK;
    SizeT           destination_length_sizeT = (SizeT)*destination_length;
    SizeT           source_length_sizeT      = (SizeT)*source_length;
    ISzAlloc        *g_alloc;
    ELzmaStatus     status;

    g_alloc = (ISzAlloc *)lzma_alloc;

    sres_ret = LzmaDecode(destination,
                          &destination_length_sizeT,
                          source,
                          &source_length_sizeT,
                          compression_property,
                          (unsigned)compression_property_size,
                          LZMA_FINISH_ANY,
                          &status,
                          g_alloc);

    *destination_length = (unsigned int)destination_length_sizeT;
    *source_length      = (unsigned int)source_length_sizeT;
    lzma_ret            = (LZMA_ret)sres_ret;

    return lzma_ret;
}


LZMA_ret lzma_decode2flash(
	uint8_t             *destination,
	uint32_t            reserved_size,
	const uint8_t       *source,
	lzma_alloc_t        *lzma_alloc )
{
    uint64_t            unpacked_size;
    int                 i;
    SRes                sres;
    ISzAlloc            *g_alloc;
    CLzmaDec            state;
    unsigned char       lzma_header[ LZMA_PROPS_SIZE + 8 ];
    hal_flash_status_t  status;

#if PRODUCT_VERSION != 7933
    if ( ! IS_ALIGNED(destination) )
    {
        LZMA_LOG("destination error: must at begin address of one block");
        return LZMA_ERROR_MEM;
    }
#endif

    g_alloc = (ISzAlloc *)lzma_alloc;

    status = hal_flash_read( (uint32_t)source - RAM_ADDRESS_BASE,
                             lzma_header, sizeof( lzma_header ) );
    if ( status != HAL_FLASH_STATUS_OK )
    {
        LZMA_LOG( "read header fail (%d)\n", status );
        return LZMA_ERROR_READ;
    }

    // read decompressed size
    unpacked_size = 0;
    for ( i = 0; i < 8; i++ ) {
        uint8_t b = lzma_header[ LZMA_PROPS_SIZE + i ];
        unpacked_size += ( uint64_t )b << ( i * 8 );
    }

    if ( unpacked_size > reserved_size )
    {
        LZMA_LOG("ERROR: decompressed size over reserverd size!\n");
        return LZMA_ERROR_MEM;
    }

    LzmaDec_Construct( &state );
    LzmaDec_Allocate( &state, lzma_header, LZMA_PROPS_SIZE, g_alloc );

    LZMA_LOG( "LZMA decode begin: \n" );
    LZMA_LOG( "destination = %x\n", destination );
    LZMA_LOG( "source = %x\n", source + sizeof( lzma_header ) );
    LZMA_LOG( "unpacked_size = %d\n", ( uint32_t )unpacked_size );

    sres = Decode2Ram( &state, destination,
                       ( uint8_t * )source + sizeof( lzma_header ),
                       ( uint32_t )unpacked_size, g_alloc );

    LzmaDec_Free( &state, g_alloc );

    return sres;
}

