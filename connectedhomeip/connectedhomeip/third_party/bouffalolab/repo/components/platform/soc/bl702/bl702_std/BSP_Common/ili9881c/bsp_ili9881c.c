/**
 * @file bsp_il9341.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bsp_ili9881c.h"
#include "hal_dsi.h"

/** @addtogroup  BSP_COMMON
 *  @{
 */

/** @addtogroup  ILI9881C
 *  @{
 */

/** @defgroup  ILI9881C_Private_Macros
 *  @{
 */

/*@} end of group ILI9881C_Private_Macros */

/** @defgroup  ILI9881C_Private_Types
 *  @{
 */

#define u8  uint8_t
#define u16 uint16_t

enum ili9881c_op {
    ILI9881C_SWITCH_PAGE,
    ILI9881C_COMMAND,
};

struct ili9881c_instr {
    enum ili9881c_op op;

    union arg {
        struct cmd {
            u8 cmd;
            u8 data;
        } cmd;
        u8 page;
    } arg;
};

struct ili9881c_desc {
    const struct ili9881c_instr *init;
    const size_t init_length;
};

struct ili9881c_dev {
    const mipi_dsi_device_t *dsi;
    const struct ili9881c_desc *desc;
};

#define ILI9881C_SWITCH_PAGE_INSTR(_page) \
    {                                     \
        .op = ILI9881C_SWITCH_PAGE,       \
        .arg = {                          \
            .page = (_page),              \
        },                                \
    }

#define ILI9881C_COMMAND_INSTR(_cmd, _data) \
    {                                       \
        .op = ILI9881C_COMMAND,             \
        .arg = {                            \
            .cmd = {                        \
                .cmd = (_cmd),              \
                .data = (_data),            \
            },                              \
        },                                  \
    }

/*@} end of group ILI9881C_Private_Types */

/** @defgroup  ILI9881C_Private_Variables
 *  @{
 */
static const struct ili9881c_instr lhr050h41_init[] = {
    ILI9881C_SWITCH_PAGE_INSTR(3),
    ILI9881C_COMMAND_INSTR(0x01, 0x00),
    ILI9881C_COMMAND_INSTR(0x02, 0x00),
    ILI9881C_COMMAND_INSTR(0x03, 0x72),
    ILI9881C_COMMAND_INSTR(0x04, 0x00),
    ILI9881C_COMMAND_INSTR(0x05, 0x00),
    ILI9881C_COMMAND_INSTR(0x06, 0x09),
    ILI9881C_COMMAND_INSTR(0x07, 0x00),
    ILI9881C_COMMAND_INSTR(0x08, 0x00),
    ILI9881C_COMMAND_INSTR(0x09, 0x01),
    ILI9881C_COMMAND_INSTR(0x0a, 0x00),
    ILI9881C_COMMAND_INSTR(0x0b, 0x00),
    ILI9881C_COMMAND_INSTR(0x0c, 0x01),
    ILI9881C_COMMAND_INSTR(0x0d, 0x00),
    ILI9881C_COMMAND_INSTR(0x0e, 0x00),
    ILI9881C_COMMAND_INSTR(0x0f, 0x00),
    ILI9881C_COMMAND_INSTR(0x10, 0x00),
    ILI9881C_COMMAND_INSTR(0x11, 0x00),
    ILI9881C_COMMAND_INSTR(0x12, 0x00),
    ILI9881C_COMMAND_INSTR(0x13, 0x00),
    ILI9881C_COMMAND_INSTR(0x14, 0x00),
    ILI9881C_COMMAND_INSTR(0x15, 0x00),
    ILI9881C_COMMAND_INSTR(0x16, 0x00),
    ILI9881C_COMMAND_INSTR(0x17, 0x00),
    ILI9881C_COMMAND_INSTR(0x18, 0x00),
    ILI9881C_COMMAND_INSTR(0x19, 0x00),
    ILI9881C_COMMAND_INSTR(0x1a, 0x00),
    ILI9881C_COMMAND_INSTR(0x1b, 0x00),
    ILI9881C_COMMAND_INSTR(0x1c, 0x00),
    ILI9881C_COMMAND_INSTR(0x1d, 0x00),
    ILI9881C_COMMAND_INSTR(0x1e, 0x40),
    ILI9881C_COMMAND_INSTR(0x1f, 0x80),
    ILI9881C_COMMAND_INSTR(0x20, 0x05),
    ILI9881C_COMMAND_INSTR(0x21, 0x02),
    ILI9881C_COMMAND_INSTR(0x22, 0x00),
    ILI9881C_COMMAND_INSTR(0x23, 0x00),
    ILI9881C_COMMAND_INSTR(0x24, 0x00),
    ILI9881C_COMMAND_INSTR(0x25, 0x00),
    ILI9881C_COMMAND_INSTR(0x26, 0x00),
    ILI9881C_COMMAND_INSTR(0x27, 0x00),
    ILI9881C_COMMAND_INSTR(0x28, 0x33),
    ILI9881C_COMMAND_INSTR(0x29, 0x02),
    ILI9881C_COMMAND_INSTR(0x2a, 0x00),
    ILI9881C_COMMAND_INSTR(0x2b, 0x00),
    ILI9881C_COMMAND_INSTR(0x2c, 0x00),
    ILI9881C_COMMAND_INSTR(0x2d, 0x00),
    ILI9881C_COMMAND_INSTR(0x2e, 0x00),
    ILI9881C_COMMAND_INSTR(0x2f, 0x00),
    ILI9881C_COMMAND_INSTR(0x30, 0x00),
    ILI9881C_COMMAND_INSTR(0x31, 0x00),
    ILI9881C_COMMAND_INSTR(0x32, 0x00),
    ILI9881C_COMMAND_INSTR(0x33, 0x00),
    ILI9881C_COMMAND_INSTR(0x34, 0x04),
    ILI9881C_COMMAND_INSTR(0x35, 0x00),
    ILI9881C_COMMAND_INSTR(0x36, 0x00),
    ILI9881C_COMMAND_INSTR(0x37, 0x00),
    ILI9881C_COMMAND_INSTR(0x38, 0x3C),
    ILI9881C_COMMAND_INSTR(0x39, 0x00),
    ILI9881C_COMMAND_INSTR(0x3a, 0x40),
    ILI9881C_COMMAND_INSTR(0x3b, 0x40),
    ILI9881C_COMMAND_INSTR(0x3c, 0x00),
    ILI9881C_COMMAND_INSTR(0x3d, 0x00),
    ILI9881C_COMMAND_INSTR(0x3e, 0x00),
    ILI9881C_COMMAND_INSTR(0x3f, 0x00),
    ILI9881C_COMMAND_INSTR(0x40, 0x00),
    ILI9881C_COMMAND_INSTR(0x41, 0x00),
    ILI9881C_COMMAND_INSTR(0x42, 0x00),
    ILI9881C_COMMAND_INSTR(0x43, 0x00),
    ILI9881C_COMMAND_INSTR(0x44, 0x00),
    ILI9881C_COMMAND_INSTR(0x50, 0x01),
    ILI9881C_COMMAND_INSTR(0x51, 0x23),
    ILI9881C_COMMAND_INSTR(0x52, 0x45),
    ILI9881C_COMMAND_INSTR(0x53, 0x67),
    ILI9881C_COMMAND_INSTR(0x54, 0x89),
    ILI9881C_COMMAND_INSTR(0x55, 0xab),
    ILI9881C_COMMAND_INSTR(0x56, 0x01),
    ILI9881C_COMMAND_INSTR(0x57, 0x23),
    ILI9881C_COMMAND_INSTR(0x58, 0x45),
    ILI9881C_COMMAND_INSTR(0x59, 0x67),
    ILI9881C_COMMAND_INSTR(0x5a, 0x89),
    ILI9881C_COMMAND_INSTR(0x5b, 0xab),
    ILI9881C_COMMAND_INSTR(0x5c, 0xcd),
    ILI9881C_COMMAND_INSTR(0x5d, 0xef),
    ILI9881C_COMMAND_INSTR(0x5e, 0x11),
    ILI9881C_COMMAND_INSTR(0x5f, 0x01),
    ILI9881C_COMMAND_INSTR(0x60, 0x00),
    ILI9881C_COMMAND_INSTR(0x61, 0x15),
    ILI9881C_COMMAND_INSTR(0x62, 0x14),
    ILI9881C_COMMAND_INSTR(0x63, 0x0e),
    ILI9881C_COMMAND_INSTR(0x64, 0x0f),
    ILI9881C_COMMAND_INSTR(0x65, 0x0c),
    ILI9881C_COMMAND_INSTR(0x66, 0x0d),
    ILI9881C_COMMAND_INSTR(0x67, 0x06),
    ILI9881C_COMMAND_INSTR(0x68, 0x02),
    ILI9881C_COMMAND_INSTR(0x69, 0x07),
    ILI9881C_COMMAND_INSTR(0x6a, 0x02),
    ILI9881C_COMMAND_INSTR(0x6b, 0x02),
    ILI9881C_COMMAND_INSTR(0x6c, 0x02),
    ILI9881C_COMMAND_INSTR(0x6d, 0x02),
    ILI9881C_COMMAND_INSTR(0x6e, 0x02),
    ILI9881C_COMMAND_INSTR(0x6f, 0x02),
    ILI9881C_COMMAND_INSTR(0x70, 0x02),
    ILI9881C_COMMAND_INSTR(0x71, 0x02),
    ILI9881C_COMMAND_INSTR(0x72, 0x02),
    ILI9881C_COMMAND_INSTR(0x73, 0x02),
    ILI9881C_COMMAND_INSTR(0x74, 0x02),
    ILI9881C_COMMAND_INSTR(0x75, 0x01),
    ILI9881C_COMMAND_INSTR(0x76, 0x00),
    ILI9881C_COMMAND_INSTR(0x77, 0x14),
    ILI9881C_COMMAND_INSTR(0x78, 0x15),
    ILI9881C_COMMAND_INSTR(0x79, 0x0e),
    ILI9881C_COMMAND_INSTR(0x7a, 0x0f),
    ILI9881C_COMMAND_INSTR(0x7b, 0x0c),
    ILI9881C_COMMAND_INSTR(0x7c, 0x0d),
    ILI9881C_COMMAND_INSTR(0x7d, 0x06),
    ILI9881C_COMMAND_INSTR(0x7e, 0x02),
    ILI9881C_COMMAND_INSTR(0x7f, 0x07),
    ILI9881C_COMMAND_INSTR(0x80, 0x02),
    ILI9881C_COMMAND_INSTR(0x81, 0x02),
    //ILI9881C_COMMAND_INSTR(0x82, 0x0F),
    ILI9881C_COMMAND_INSTR(0x83, 0x02),
    ILI9881C_COMMAND_INSTR(0x84, 0x02),
    ILI9881C_COMMAND_INSTR(0x85, 0x02),
    ILI9881C_COMMAND_INSTR(0x86, 0x02),
    ILI9881C_COMMAND_INSTR(0x87, 0x02),
    ILI9881C_COMMAND_INSTR(0x88, 0x02),
    ILI9881C_COMMAND_INSTR(0x89, 0x02),
    ILI9881C_COMMAND_INSTR(0x8A, 0x02),
    ILI9881C_SWITCH_PAGE_INSTR(4),
    ILI9881C_COMMAND_INSTR(0x6C, 0x15),
    ILI9881C_COMMAND_INSTR(0x6E, 0x2a),
    ILI9881C_COMMAND_INSTR(0x6F, 0x33),
    ILI9881C_COMMAND_INSTR(0x3A, 0x94),
    ILI9881C_COMMAND_INSTR(0x8D, 0x15),
    ILI9881C_COMMAND_INSTR(0x87, 0xBA),
    ILI9881C_COMMAND_INSTR(0x26, 0x76),
    ILI9881C_COMMAND_INSTR(0x2d, 0x03),
    ILI9881C_COMMAND_INSTR(0x2f, 0x00),
    ILI9881C_COMMAND_INSTR(0xB2, 0xD1),
    ILI9881C_COMMAND_INSTR(0xB5, 0x06),
    ILI9881C_SWITCH_PAGE_INSTR(1),
    ILI9881C_COMMAND_INSTR(0x22, 0x0a),
    ILI9881C_COMMAND_INSTR(0x25, 0x0a),
    ILI9881C_COMMAND_INSTR(0x26, 0x08),
    ILI9881C_COMMAND_INSTR(0x27, 0x25),
    ILI9881C_COMMAND_INSTR(0x31, 0x00),
    ILI9881C_COMMAND_INSTR(0x53, 0xa5),
    ILI9881C_COMMAND_INSTR(0x55, 0xA2),
    ILI9881C_COMMAND_INSTR(0x50, 0xb7),
    ILI9881C_COMMAND_INSTR(0x51, 0xb7),
    ILI9881C_COMMAND_INSTR(0x60, 0x22),
    ILI9881C_COMMAND_INSTR(0x61, 0x00),
    ILI9881C_COMMAND_INSTR(0x62, 0x19),
    ILI9881C_COMMAND_INSTR(0x63, 0x10),
    ILI9881C_COMMAND_INSTR(0xA0, 0x08),
    ILI9881C_COMMAND_INSTR(0xA1, 0x17),
    ILI9881C_COMMAND_INSTR(0xA2, 0x1e),
    ILI9881C_COMMAND_INSTR(0xA3, 0x0e),
    ILI9881C_COMMAND_INSTR(0xA4, 0x13),
    ILI9881C_COMMAND_INSTR(0xA5, 0x24),
    ILI9881C_COMMAND_INSTR(0xA6, 0x1b),
    ILI9881C_COMMAND_INSTR(0xA7, 0x1b),
    ILI9881C_COMMAND_INSTR(0xA8, 0x53),
    ILI9881C_COMMAND_INSTR(0xA9, 0x1b),
    ILI9881C_COMMAND_INSTR(0xAA, 0x28),
    ILI9881C_COMMAND_INSTR(0xAB, 0x45),
    ILI9881C_COMMAND_INSTR(0xAC, 0x1a),
    ILI9881C_COMMAND_INSTR(0xAD, 0x1a),
    ILI9881C_COMMAND_INSTR(0xAE, 0x50),
    ILI9881C_COMMAND_INSTR(0xAF, 0x21),
    ILI9881C_COMMAND_INSTR(0xB0, 0x2c),
    ILI9881C_COMMAND_INSTR(0xB1, 0x3b),
    ILI9881C_COMMAND_INSTR(0xB2, 0x63),
    ILI9881C_COMMAND_INSTR(0xB3, 0x39),
    ILI9881C_COMMAND_INSTR(0xC0, 0x08),
    ILI9881C_COMMAND_INSTR(0xC1, 0x0c),
    ILI9881C_COMMAND_INSTR(0xC2, 0x17),
    ILI9881C_COMMAND_INSTR(0xC3, 0x0f),
    ILI9881C_COMMAND_INSTR(0xC4, 0x0b),
    ILI9881C_COMMAND_INSTR(0xC5, 0x1c),
    ILI9881C_COMMAND_INSTR(0xC6, 0x10),
    ILI9881C_COMMAND_INSTR(0xC7, 0x16),
    ILI9881C_COMMAND_INSTR(0xC8, 0x5b),
    ILI9881C_COMMAND_INSTR(0xC9, 0x1a),
    ILI9881C_COMMAND_INSTR(0xCA, 0x26),
    ILI9881C_COMMAND_INSTR(0xCB, 0x55),
    ILI9881C_COMMAND_INSTR(0xCC, 0x1d),
    ILI9881C_COMMAND_INSTR(0xCD, 0x1e),
    ILI9881C_COMMAND_INSTR(0xCE, 0x52),
    ILI9881C_COMMAND_INSTR(0xCF, 0x26),
    ILI9881C_COMMAND_INSTR(0xD0, 0x29),
    ILI9881C_COMMAND_INSTR(0xD1, 0x45),
    ILI9881C_COMMAND_INSTR(0xD2, 0x63),
    ILI9881C_COMMAND_INSTR(0xD3, 0x39),
};

static const struct ili9881c_desc lhr050h41_desc = {
    .init = lhr050h41_init,
    .init_length = sizeof(lhr050h41_init) / sizeof(lhr050h41_init[0]),
};

#if 0
/* embedded device not use this */
const struct mipi_dsi_host_ops ops =
{
    .attach = NULL,
    .detach = NULL,
    .transfer = NULL,
};

static struct mipi_dsi_host host =
{
    //.dev=NULL,
    .ops = &ops,
};
#endif

static const mipi_dsi_device_t dsi_device = {
    //.dev=NULL,
    //.host=&host,
    .name = { "Ili9881c" },
    .channel = 0,
    .lanes = 4,
    .format = MIPI_DSI_DATA_FMT_RGB565,
    .mode_flags = (MIPI_DSI_MODE_LPM),
    .hs_rate = 500,
    .lp_rate = 8,
};
static struct ili9881c_dev ili9881c = {
    .dsi = &dsi_device,
    .desc = &lhr050h41_desc,
};

/*@} end of group ILI9881C_Private_Variables */

/** @defgroup  ILI9881C_Global_Variables
 *  @{
 */

/*@} end of group ILI9881C_Global_Variables */

/** @defgroup  ILI9881C_Private_Fun_Declaration
 *  @{
 */

/*@} end of group ILI9881C_Private_Fun_Declaration */

/** @defgroup  ILI9881C_Private_Functions
 *  @{
 */

/*@} end of group ILI9881C_Private_Functions */

/** @defgroup  ILI9881C_Public_Functions
 *  @{
 */
/*
 * The panel seems to accept some private DCS commands that map
 * directly to registers.
 *
 * It is organised by page, with each page having its own set of
 * registers, and the first page looks like it's holding the standard
 * DCS commands.
 *
 * So before any attempt at sending a command or data, we have to be
 * sure if we're in the right page or not.
 */
static int ili9881c_switch_page(struct ili9881c_dev *dev, u8 page)
{
    u8 buf[4] = { 0xff, 0x98, 0x81, page };
    int ret;

    ret = mipi_dsi_dcs_write_buffer(dev->dsi, buf, sizeof(buf));

    if (ret < 0) {
        return ret;
    }

    return 0;
}

static int ili9881c_send_cmd_data(struct ili9881c_dev *dev, u8 cmd, u8 data)
{
    u8 buf[2] = { cmd, data };
    int ret;

    ret = mipi_dsi_dcs_write_buffer(dev->dsi, buf, sizeof(buf));

    if (ret < 0) {
        return ret;
    }

    return 0;
}

static int ili9881c_prepare(struct ili9881c_dev *dev)
{
    unsigned int i;
    int ret;

    mipi_dsi_controller_init(dev->dsi);

    ili9881c_switch_page(dev, 0);
    mipi_dsi_dcs_enter_sleep_mode(&dsi_device);
    bflb_platform_delay_ms(100);
    mipi_dsi_dcs_soft_reset(&dsi_device);
    bflb_platform_delay_ms(200);

    for (i = 0; i < dev->desc->init_length; i++) {
        const struct ili9881c_instr *instr = &dev->desc->init[i];

        if (instr->op == ILI9881C_SWITCH_PAGE) {
            ret = ili9881c_switch_page(dev, instr->arg.page);
        } else if (instr->op == ILI9881C_COMMAND)
            ret = ili9881c_send_cmd_data(dev, instr->arg.cmd.cmd,
                                         instr->arg.cmd.data);

        if (ret) {
            return ret;
        }
    }

    ret = ili9881c_switch_page(dev, 0);

    if (ret) {
        return ret;
    }

    ret = mipi_dsi_dcs_set_tear_on(dev->dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);

    if (ret) {
        return ret;
    }

    ret = mipi_dsi_dcs_exit_sleep_mode(dev->dsi);

    if (ret) {
        return ret;
    }

    return 0;
}

static int ili9881c_enable(struct ili9881c_dev *dev)
{
    mipi_dsi_dcs_set_display_on(dev->dsi);

    return 0;
}

static int ili9881c_disable(struct ili9881c_dev *dev)
{
    return mipi_dsi_dcs_set_display_off(dev->dsi);
}

static int ili9881c_unprepare(struct ili9881c_dev *dev)
{
    mipi_dsi_dcs_enter_sleep_mode(dev->dsi);
    return 0;
}

int display_prepare(void)
{
    return ili9881c_prepare(&ili9881c);
}

int display_enable(void)
{
    return ili9881c_enable(&ili9881c);
}

int display_disable(void)
{
    return ili9881c_disable(&ili9881c);
}

int display_unprepare(void)
{
    return ili9881c_unprepare(&ili9881c);
}

/*@} end of group ILI9881C_Public_Functions */

/*@} end of group ILI9881C */

/*@} end of group BSP_COMMON */
