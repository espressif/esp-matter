/*
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_NAME        8
#define MAX_FL_COMP     16
#define CRC_TABLE_SIZE  256
#define MAX_BUFFER_SIZE 512
#define MAX_FILE_PATH   256
#define MAX_COMP_SIZE   32
#define MAX_SECT_SIZE   1024

#define STRINGIFY_1(x)  #x
#define S(x)        STRINGIFY_1(x)

#define WLAN_FW_MAGIC           (('W' << 0)|('L' << 8)|('F' << 16)|('W' << 24))
#define FW_MAGIC_STR            (('M' << 0)|('R' << 8)|('V' << 16)|('L' << 24))
#define FW_MAGIC_SIG            ((0x7BUL << 0) | (0xF1UL << 8) | (0x9CUL << 16) | (0x2EUL << 24))
#define FW_BLK_LOADABLE_SEGMENT (2)       /* loads data to follow */
#define MCU_FW_OFFSET           (0x100U)  /* MCU firmware application offset from image header */

/** Partition Table */
struct partition_table {
#define PARTITION_TABLE_MAGIC (('W' << 0)|('M' << 8)|('P' << 16)|('T' << 24))
    /** The magic identifying the start of the partition table */
    uint32_t magic;
#define PARTITION_TABLE_VERSION 1
    /** The version number of this partition table */
    uint16_t version;
    /** The number of partition entries that follow this */
    uint16_t partition_entries_no;
    /** Generation level */
    uint32_t gen_level;
    /** The CRC of all the above components */
    uint32_t crc;
};

/** Partition Entry */
struct partition_entry {
    /** The type of the flash component */
    uint8_t type;
    /** The device id, internal flash is always id 0 */
    uint8_t device;
    /** A descriptive component name */
    char name[MAX_NAME];
    /** Start address on the given device */
    uint32_t start;
    /** Size on the given device */
    uint32_t size;
    /** Generation level */
    uint32_t gen_level;
};

/** The various components in a flash layout */
enum flash_comp {
    /** The secondary stage boot loader to assist firmware bootup */
    FC_COMP_BOOT2 = 0,
    /** The firmware image. There can be a maximum of two firmware
     * components available in a flash layout. These will be used in an
     * active-passive mode if rfget module is enabled.
     */
    FC_COMP_FW,
    /** The wlan firmware image. There can be one wlan firmware image in the
     * system. The contents of this location would be downloaded to the WLAN
     * chip.
     */
    FC_COMP_WLAN_FW,
    /** The FTFS image. */
    FC_COMP_FTFS,
    /** The PSM data */
    FC_COMP_PSM,
    /** Application Specific component */
    FC_COMP_USER_APP,
    /** The BT firmware image if external BT/BLE chip is used */
    FC_COMP_BT_FW,
};

/*
 * Firmware magic signature
 *
 * First word is the string "MRVL" and is endian invariant.
 * Second word = magic value 0x2e9cf17.
 * Third word = time stamp (seconds since 00:00:00 UTC, January 1, 1970).
 */
struct img_hdr {
    uint32_t magic_str;
    uint32_t magic_sig;
    uint32_t time;
    uint32_t seg_cnt;
    uint32_t entry;
};

/* Maximum number of segments */
#define SEG_CNT 9

struct seg_hdr {
    uint32_t type;
    uint32_t offset;
    uint32_t len;
    uint32_t laddr;
    uint32_t crc;
};

struct wlan_fw_header {
    uint32_t        magic;
    uint32_t        length;
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static uint32_t crc_table[CRC_TABLE_SIZE];
static const int rcrc32 = 0xEDB88320;
static char gbuffer[MAX_BUFFER_SIZE];
static struct partition_table g_flash_table;
static struct partition_entry g_flash_parts[MAX_FL_COMP];
static uint8_t gsection[MAX_SECT_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/
static void die_usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s <option> <input_file> <output_img> [<load_addr>]\n", argv0);
    fprintf(stderr, "  option:\n");
    fprintf(stderr, "    layout: Convert layout config file to layout flash image.\n");
    fprintf(stderr, "    mcufw:  Convert MCU image to MCU flash image. <load_addr> needed.\n");
    fprintf(stderr, "    wififw: Convert WiFi image to WiFi flash image\n");
    exit(1);
}

static void die_perror(const char *msg)
{
    perror(msg);
    exit(1);
}

static int flash_get_comp(const char *comp)
{
    if (!strcmp(comp, "FC_COMP_FW"))
        return FC_COMP_FW;
    else if (!strcmp(comp, "FC_COMP_WLAN_FW"))
        return FC_COMP_WLAN_FW;
    else if (!strcmp(comp, "FC_COMP_BT_FW"))
        return FC_COMP_BT_FW;
    else if (!strcmp(comp, "FC_COMP_FTFS"))
        return FC_COMP_FTFS;
    else if (!strcmp(comp, "FC_COMP_BOOT2"))
        return FC_COMP_BOOT2;
    else if (!strcmp(comp, "FC_COMP_PSM"))
        return FC_COMP_PSM;
    else if (!strcmp(comp, "FC_COMP_USER_APP"))
        return FC_COMP_USER_APP;
    else {
        printf("Error: Invalid flash component %s\n", comp);
        return -1;
    }
}

static void soft_crc32_init(void)
{
    unsigned int crc = 0;
    unsigned char i;
    unsigned int j;

    for (j = 0; j < CRC_TABLE_SIZE; j++) {
        crc = 0;
        for (i = 0x01; i != 0x00; i <<= 1) {
            if ((crc & 0x00000001) != 0) {
                crc >>= 1;
                crc ^= rcrc32;
            } else {
                crc >>= 1;
            }
            if ((j & i) != 0)
                crc ^= rcrc32;
        }
        crc_table[j] = crc;
    }
}

static uint32_t soft_crc32(const void *__data, int data_size, uint32_t crc)
{
    const uint8_t *data = __data;
    unsigned int result = crc;
    unsigned char crc_H8;

    while (data_size--) {
        crc_H8 = (unsigned char)(result & 0x000000FF);
        result >>= 8;
        result ^= crc_table[crc_H8 ^ (*data)];
        data++;
    }

    return result;
}

static void h_to_le32(uint32_t v, uint32_t *x)
{
    uint8_t *b = (uint8_t *)x;
    b[0] = v;
    b[1] = (v >> 8);
    b[2] = (v >> 16);
    b[3] = (v >> 24);
}

static void convert_layout(FILE *in, FILE *out)
{
    int i = 0, parts_no = 0, ret;
    char comp[MAX_COMP_SIZE];
    struct partition_entry *fl = &g_flash_parts[0];
    uint32_t crc;

    memset(gbuffer, 0, sizeof(gbuffer));
    memset(&g_flash_table, 0, sizeof(g_flash_table));
    memset(&g_flash_parts[0], 0, sizeof(g_flash_parts));

    while (fgets(gbuffer, sizeof(gbuffer), in) != NULL) {
        if (i >= MAX_FL_COMP) {
            printf("Only %d partition entries are supported, "
                "Truncating...\n", MAX_FL_COMP);
            break;
        }
        /* Skip and continue if the line begins with # or is empty */
        if (gbuffer[0] == '#' || gbuffer[0] == '\n')
            /*
             * There is autogeneration header in layout file and
             * hence printing here some log message will increase
             * overall time.
             */
            continue;
        sscanf(gbuffer,
            "%"S(MAX_COMP_SIZE)"s""\t%x\t%x\t%d\t""%"S(MAX_NAME)"s"
            , comp,
            (unsigned int *)&fl[i].start,
            (unsigned int *)&fl[i].size,
            (unsigned int *)&fl[i].device,
            fl[i].name);
        if ((ret = flash_get_comp(comp)) != -1)
            fl[i].type = ret;
        else {
            die_perror("get component type failed");
        }

        /* Default generation level for all partitions */
        fl[i].gen_level = 1;

        parts_no++;
        i++;
        memset(gbuffer, 0, sizeof(gbuffer));
    }

    g_flash_table.magic = PARTITION_TABLE_MAGIC;
    g_flash_table.version = PARTITION_TABLE_VERSION;
    g_flash_table.partition_entries_no = parts_no;
    g_flash_table.gen_level = 0;
    g_flash_table.crc = soft_crc32(&g_flash_table, sizeof(g_flash_table) - 4, 0);
    if (fwrite(&g_flash_table, sizeof(g_flash_table), 1, out) != 1)
        die_perror("write output file error");

    crc = soft_crc32(&g_flash_parts[0], sizeof(g_flash_parts[0]) * parts_no, 0);
    if (fwrite(&g_flash_parts[0], sizeof(g_flash_parts[0]) * parts_no, 1, out) != 1)
        die_perror("write output file error");
    
    if (fwrite(&crc, sizeof(crc), 1, out) != 1)
        die_perror("write output file error");
}

static void convert_mcufw(FILE *in, FILE *out, uint32_t laddr)
{
    struct img_hdr ih;
    struct seg_hdr *sh;
    uint32_t len, entry, pad, crc;
    uint8_t *data;

    fseek(in, 0, SEEK_END);
    len = ftell(in);
    fseek(in, 4, SEEK_SET);
    if (fread(&entry, 4, 1, in) != 1)
    {
        die_perror("read fw entry error");
    }
    fseek(in, 0, SEEK_SET);

    h_to_le32(FW_MAGIC_STR, &ih.magic_str);
    h_to_le32(FW_MAGIC_SIG, &ih.magic_sig);
    h_to_le32(time(NULL), &ih.time);
    h_to_le32(1U, &ih.seg_cnt);
    h_to_le32(entry, &ih.entry);

    if (MCU_FW_OFFSET - sizeof(struct img_hdr) < SEG_CNT * sizeof(struct seg_hdr))
    {
        die_perror("MCU_FW_OFFSET too small");
    }

    sh = malloc(MCU_FW_OFFSET - sizeof(struct img_hdr));
    if (!sh)
        die_perror("out of memory");
    memset(sh, 0xff, MCU_FW_OFFSET - sizeof(struct img_hdr));

    /* padding firmware to 4 bytes aligned */
    pad = ((len + 3U) & ~3UL) - len;
    data = malloc(len + pad);
    if (!data)
        die_perror("out of memory");
    if (fread(data, len, 1, in) != 1)
        die_perror("cannot read MCU firmware");
    memset(data + len, 0xff, pad);
    len += pad;

    crc = soft_crc32(data, len, 0);

    h_to_le32(FW_BLK_LOADABLE_SEGMENT, &sh[0].type);
    h_to_le32(MCU_FW_OFFSET, &sh[0].offset);
    h_to_le32(len, &sh[0].len);
    h_to_le32(laddr, &sh[0].laddr);
    h_to_le32(crc, &sh[0].crc);

    if (fwrite(&ih, sizeof(ih), 1, out) != 1)
        die_perror("write image header error");
    if (fwrite(sh, MCU_FW_OFFSET - sizeof(struct img_hdr), 1, out) != 1)
        die_perror("write segment header error");
    if (fwrite(data, len, 1, out) != 1)
        die_perror("write MCU firmware error");
}

static void convert_wififw(FILE *in, FILE *out)
{
    struct wlan_fw_header wf_header;
    uint32_t len;
    uint32_t sz;

    fseek(in, 0, SEEK_END);
    len = ftell(in);
    fseek(in, 0, SEEK_SET);

    wf_header.magic = WLAN_FW_MAGIC;
    wf_header.length = len;

    if (fwrite(&wf_header, sizeof(wf_header), 1, out) != 1)
        die_perror("write wifi header error");
    
    while (len > 0)
    {
        sz = sizeof(gsection) > len ? len : sizeof(gsection);
        if (fread(gsection, sz, 1, in) != 1)
            die_perror("read input file error");
        if (fwrite(gsection, sz, 1, out) != 1)
            die_perror("write output file error");
        len -= sz;
    }
}

/*!
 * @brief Main function
 */
int main(int argc, char *argv[])
{
    FILE *in, *out;
    uint32_t laddr;

    if (argc != 4 && argc != 5)
        die_usage(argv[0]);

    in = fopen(argv[2], "rb");
    if (!in)
        die_perror(argv[1]);
    out = fopen(argv[3], "wb");
    if (!out)
        die_perror(argv[2]);

    soft_crc32_init();

    if (!strncmp(argv[1], "layout", 6))
    {
        convert_layout(in, out);
    }
    else if (!strncmp(argv[1], "mcufw", 5))
    {
        if (argc != 5)
        {
            die_usage(argv[0]);
        }
        laddr = strtoul(argv[4], NULL, 0);
        printf("Convert MCU firmware with load address 0x%x\n", laddr);
        convert_mcufw(in, out, laddr);
    }
    else if (!strncmp(argv[1], "wififw", 6))
    {
        convert_wififw(in, out);
    }

    fclose(in);
    if (fclose(out) != 0)
        die_perror(argv[3]);

    return 0;
}

