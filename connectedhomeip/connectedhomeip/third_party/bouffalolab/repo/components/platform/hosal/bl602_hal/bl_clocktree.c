/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <bl602_glb.h>
#include <blog.h>

struct clk{
    const char *name;
    int parents_max;
    struct clk **parents;

    //return parent idx
    int (*parent)(struct clk *node);

    //Gating Control
    int (*enable)(struct clk *node);
    int (*disable)(struct clk *node);

    //Div Control
    int (*div_get)(struct clk *node);
    int (*div_set)(struct clk *node, int div);

    //CG status
    int (*cg)(struct clk *node);
};

/*Nodes of clock*/
static struct clk clock_RC32M;
static struct clk clock_XTAL;
static struct clk clock_pll_192m;
static struct clk clock_pll_160m;
static struct clk clock_pll_120m;
static struct clk clock_pll_96m;
static struct clk clock_pll_80m;
static struct clk clock_pll_48m;
static struct clk clock_pll_sel;
static struct clk clock_root_clk_sel_0;
static struct clk clock_root_clk_sel_1;

static int clock_root_clk_sel_0_parent_idx(struct clk *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    //will use idx 0
    return ((tmpVal >> 0) & 0x1) ? 1 : 0;
}

static struct clk clock_root_clk_sel_0 = {
    .name           = "root_clk_sel_0",
    .parents_max    = 2,
    .parents        = (struct clk *[]) {
        &clock_RC32M,
        &clock_XTAL,
        NULL,
    },
    .parent = clock_root_clk_sel_0_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_clk_pll_sel_idx(struct clk *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_PLL_SEL);

    return tmpVal;
}

static struct clk clock_pll_sel = {
    .name           = "pll_sel",
    .parents_max    = 4,
    .parents        = (struct clk *[]) {
        &clock_pll_48m,
        &clock_pll_120m,
        &clock_pll_160m,
        &clock_pll_192m,
        NULL,
    },
    .parent         = clock_root_clk_pll_sel_idx,
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_clk_sel_1_parent_idx(struct clk *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    //will use idx 1
    return ((tmpVal >> 1) & 0x1) ? 1 : 0;
}

static int clock_root_clk_sel_1_div_get(struct clk *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV);

    return tmpVal + 1;
}

static struct clk clock_root_clk_sel_1 = {
    .name           = "root_clk_sel_1",
    .parents_max    = 2,
    .parents        = (struct clk *[]) {
        &clock_root_clk_sel_0,
        &clock_pll_sel,
        NULL,
    },
    .parent         = clock_root_clk_sel_1_parent_idx,
    .div_get        = clock_root_clk_sel_1_div_get,
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_fclk = {
    .name           = "fclk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_root_clk_sel_1,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_hclk = {
    .name           = "hclk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_root_clk_sel_1,
        NULL,
    },
    //XXX Clock Enable
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_bclk_div_get(struct clk *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV);

    return tmpVal + 1;
}

static struct clk clock_bclk = {
    .name           = "bclk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_root_clk_sel_1,
        NULL,
    },
    .div_get        = clock_root_bclk_div_get,
    //XXX Clock Enable
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_RC32M = {
    .name           = "RC 32M",
    .parents_max    = 0,
    .parents        = (struct clk *[]) {
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_XTAL = {
    .name           = "XTAL 40M",
    .parents_max    = 0,
    .parents        = (struct clk *[]) {
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_clk_pll_root_parent_idx(struct clk *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CLKPLL_TOP_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_CLKPLL_XTAL_RC32M_SEL);

    return tmpVal;
}

static struct clk clock_PLL = {
    .name           = "PLL Root",
    .parents_max    = 2,
    .parents        = (struct clk *[]) {
        &clock_XTAL,
        &clock_RC32M,
        NULL,
    },
    .parent         = clock_root_clk_pll_root_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_pll_192m = {
    .name           = "PLL_192M",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_pll_160m = {
    .name           = "PLL_160M",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_pll_120m = {
    .name           = "PLL_120M",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_pll_96m = {
    .name           = "PLL_96m",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_pll_80m = {
    .name           = "PLL_80m",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk clock_pll_48m = {
    .name           = "PLL_48m",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_uart_parent_idx(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL);

    return tmpVal;
}

static int clock_uart_gating_status(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_uart_div_get(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_DIV);

    return tmpVal + 1;
}

/* nodes of clock consumer*/
static struct clk clock_uart = {
    .name           = "uart clk",
    .parents_max    = 2,
    .parents        = (struct clk *[]) {
        &clock_root_clk_sel_1,        //idx 0
        &clock_pll_160m ,             //idx 1
        NULL,
    },
    .parent         = clock_uart_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_uart_gating_status,
    .div_get        = clock_uart_div_get
};

static struct clk leaf_mcu_fclk = {
    .name           = "MCU fclk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_fclk,        //idx 0
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk leaf_mcu_hclk = {
    .name           = "MCU hclk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_hclk,        //idx 0
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk leaf_mcu_bclk = {
    .name           = "MCU bclk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_bclk,        //idx 0
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_flash_div_get(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);

    return tmpVal + 1;
}

static int clock_flash_gating_status(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_flash_parent_idx(struct clk *node)
{
    uint32_t tmpVal = 0, tmpVal2 = 0, val;

    //Flash Clock Select (0:sf_clk_sel2, 1:80M, 2:bclk, 3:96M)
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL);
    //For sf_clk_sel=0  0: 120MHz 1: XTAL 3: 48MHz
    tmpVal2 = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal2, GLB_SF_CLK_SEL2);

    if (0 == tmpVal) {
        /*sf_clk_sel == 0*/
        val = tmpVal2;
    } else {
        val = tmpVal - 1 + 3;
    }

    return val;
}

static struct clk flash_clock = {
    .name           = "flash clk",
    .parents_max    = 6,
    .parents        = (struct clk *[]) {
        &clock_pll_120m,
        &clock_XTAL,
        &clock_pll_48m,
        &clock_pll_80m,
        &clock_bclk,
        &clock_pll_96m,
        NULL,
    },
    .parent         = clock_flash_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_flash_gating_status,
    .div_get        = clock_flash_div_get
};

static int clock_i2c_gating_status(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_i2c_div_get(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV);

    return tmpVal + 1;
}

static int clock_spi_gating_status(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_spi_div_get(struct clk *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV);

    return tmpVal + 1;
}

static struct clk i2c_clock = {
    .name           = "i2c clk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_bclk,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_i2c_gating_status,
    .div_get        = clock_i2c_div_get
};

static struct clk spi_clock = {
    .name           = "spi clk",
    .parents_max    = 1,
    .parents        = (struct clk *[]) {
        &clock_bclk,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_spi_gating_status,
    .div_get        = clock_spi_div_get
};

static struct clk *clock_leaves[] = {
    &flash_clock,
    &i2c_clock,
    &spi_clock,
    &clock_uart,
    &leaf_mcu_fclk,
    &leaf_mcu_hclk,
    &leaf_mcu_bclk,
};

static void _dump_node(struct clk *node)
{
    int i;

    blog_info("node name %p %s\r\n", node, node->name);
    for (i = 0; i < node->parents_max; i++) {
        blog_info("parents[%i] %p\r\n", i, node->parents[i]);
    }
}

static void dump_clock_node(struct clk *node)
{
    struct clk *parent;
    int idx;

    puts(node->name);
    blog_info(" (Div %u)", node->div_get ? node->div_get(node) : 1);
    puts(node->cg && (1 == node->cg(node)) ? "(Gating)" : "");

    if (0 == node->parents_max) {
        return;
    }

    puts(" <<<");
    if (node->parent) {
        idx = node->parent(node);
        if (idx >= node->parents_max || idx < 0) {
            blog_info("Got illegal idx %d\r\n", idx);
            _dump_node(node);
            while (1) {
                ///XXX assert
            }
        }
    } else if (1 == node->parents_max) {
        idx = 0; //use first parent when no parent function is provided
    } else {
        /*XXX illegal clock tree topology, ASSERT Here*/
        idx = -1;
        puts("Clock Tree, ASSERT Here\r\n");
        while (1) {
        }
    }
    blog_info("(%d)-- ", idx);
    parent = node->parents[idx];

    if (NULL == parent) {
        _dump_node(node);
    }
    dump_clock_node(parent); //XXX Stack OverFlow Caution
}

void clock_tree_dump(void)
{
    int i;
    struct clk *leaf;

    for (i = 0; i < sizeof(clock_leaves)/sizeof(clock_leaves[0]); i++) {
        leaf = clock_leaves[i];
        dump_clock_node(leaf);
        puts("\r\n====================\r\n");
    }
}
