/**
 * @file bl_tjpgd.c
 * @author jsun@bouffalolab.com
 * @brief 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright bouffalolab(c) 2021
 * 
 */
#include <FreeRTOS.h>
#include <bl_mtd.h>
#include <stdio.h>
#include <string.h>
#include <utils_log.h>
#include <vfs.h>

#include "bl_timer.h"
#include "tjpgd.h"

/* User defined device identifier */
typedef struct
{
    FILE *fp;       /* File pointer for input function */
    uint8_t *fbuf;  /* Pointer to the frame buffer for output function */
    uint16_t wfbuf; /* Width of the frame buffer [pix] */
} IODEV;

/**
 * @brief User defined input funciton
 * 
 * @param jd 
 * @param buff 
 * @param nbyte 
 * @return uint16_t 
 */
unsigned int in_func(JDEC *jd, uint8_t *buff, unsigned int nbyte) {
    IODEV *dev = (IODEV *)jd->device; /* Device identifier for the session (5th argument of jd_prepare function) */

    if (buff) {
        /* Read bytes from input stream */
        return (unsigned int)aos_read((int)dev->fp, buff, nbyte);
    } else {
        /* Remove bytes from input stream */
        return (unsigned int)aos_lseek((int)dev->fp, nbyte, SEEK_CUR) ? nbyte : 0;
    }
}

/**
 * @brief User defined output funciton
 * 
 * @param jd 
 * @param bitmap 
 * @param rect 
 * @return uint16_t 
 */
int out_func(JDEC *jd, void *bitmap, JRECT *rect) {
    IODEV *dev = (IODEV *)jd->device;
    uint8_t *src, *dst;
    uint16_t y, bws, bwd;

    /* Put progress indicator */
    if (rect->left == 0) {
        printf("\r%lu%%", (rect->top << jd->scale) * 100UL / jd->height);
    }

    /* Copy the decompressed RGB rectanglar to the frame buffer (assuming RGB888 cfg) */
    src = (uint8_t *)bitmap;
    dst = dev->fbuf + 3 * (rect->top * dev->wfbuf + rect->left); /* Left-top of destination rectangular */
    bws = 3 * (rect->right - rect->left + 1);                    /* Width of source rectangular [byte] */
    bwd = 3 * dev->wfbuf;                                        /* Width of frame buffer [byte] */
    for (y = rect->top; y <= rect->bottom; y++) {
        memcpy(dst, src, bws); /* Copy a line */
        src += bws;
        dst += bwd; /* Next line */
    }

    return 1; /* Continue to decompress */
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
void Jpeg_Dec(char *buf, int len, int argc, char **argv) {
    void *work;  /* Pointer to the decompressor work area */
    JDEC jdec;   /* Decompression object */
    JRESULT res; /* Result code of TJpgDec API */
    IODEV devid; /* User defined device identifier */

#if (JD_API_TEST == 3)
    int pre_us, dec_us = 0;
#endif

    /* Initialize input stream */
    if (argc < 2)
        return;
    devid.fp = (FILE *)aos_open(argv[1], 0);

    if (!devid.fp)
        return;

    /* Allocate a work area for TJpgDec */
    work = pvPortMalloc(3100);
    if (work) {
        memset(work, 0, 3100);
    } else
        return;

#if (JD_API_TEST == 3)
    pre_us = bl_timer_now_us();
#endif

    /* Prepare to decompress */
    res = jd_prepare(&jdec, in_func, work, 3100, (void *)&devid);

#if (JD_API_TEST == 3)
    pre_us = bl_timer_now_us() - pre_us;
#endif

    if (res == JDR_OK) {
        /* Ready to dcompress. Image info is available here. */
        printf("Image dimensions: %u by %u. %u bytes used.\r\n", jdec.width, jdec.height, 3100 - jdec.sz_pool);
#if !JD_API_TEST
        devid.fbuf = pvPortMalloc(3 * jdec.width * jdec.height); /* Frame buffer for output image (assuming RGB888 cfg) */
#else
        devid.fbuf = NULL;
#endif
        devid.wfbuf = jdec.width;

#if (JD_API_TEST == 3)
        dec_us = bl_timer_now_us();
#endif

        res = jd_decomp(&jdec, out_func, 0); /* Start to decompress with 1/1 scaling */

#if (JD_API_TEST == 3)
        dec_us = bl_timer_now_us() - dec_us;
#endif

        if (res == JDR_OK) {
            /* Decompression succeeded. You have the decompressed image in the frame buffer here. */
            printf("\r\nOK  \r\n");
#if (JD_API_TEST == 3)
            printf("Prepare us: %d us.\r\nDec us: %d us\r\n", pre_us, dec_us);
#endif
        } else {
            printf("Failed to decompress: rc=%d\r\n", res);
        }

        vPortFree(devid.fbuf); /* Discard frame buffer */
    } else {
        printf("Failed to prepare: rc=%d\r\n", res);
    }

    vPortFree(work); /* Discard work area */

    aos_close((int)devid.fp); /* Close the JPEG file */
}
