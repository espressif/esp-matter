#include <stdio.h>
#include "bl602.h"
#include "bl602_glb.h"
#include "hbn_reg.h"
#include "glb_reg.h"
#include "pwm_reg.h"

struct clk_tree{
    const char *name;
    int parents_max;
    struct clk_tree **parents;

    //return parent idx
    int (*parent)(struct clk_tree *node);

    //Gating Control
    int (*enable)(struct clk_tree *node);
    int (*disable)(struct clk_tree *node);

    //Div Control
    int (*div_get)(struct clk_tree *node);
    int (*div_set)(struct clk_tree *node, int div);

    //CG status
    int (*cg)(struct clk_tree *node);
};

/*Nodes of clock*/
static struct clk_tree clock_RC32M;
static struct clk_tree clock_RC32K;
static struct clk_tree clock_XTAL;
static struct clk_tree clock_XTAL32K;
static struct clk_tree clock_pll_192m;
static struct clk_tree clock_pll_160m;
static struct clk_tree clock_pll_120m;
static struct clk_tree clock_pll_96m;
static struct clk_tree clock_pll_80m;
static struct clk_tree clock_pll_48m;
static struct clk_tree clock_pll_32m;
static struct clk_tree clock_f32k;
static struct clk_tree clock_pll_sel;
static struct clk_tree clock_f32k_sel;
static struct clk_tree clock_root_clk_sel_0;
static struct clk_tree clock_root_clk_sel_1;

static int clock_root_clk_sel_0_parent_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    //will use idx 0
    return ((tmpVal >> 0) & 0x1) ? 1 : 0;
}

static struct clk_tree clock_root_clk_sel_0 = {
    .name           = "root_clk_sel_0",
    .parents_max    = 2,
    .parents        = (struct clk_tree *[]) {
        &clock_RC32M,
        &clock_XTAL,
        NULL,
    },
    .parent = clock_root_clk_sel_0_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_clk_pll_sel_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_PLL_SEL);

    return tmpVal;
}

static int clock_root_clk_f32k_sel_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_F32K_SEL);

    return tmpVal;
}

static struct clk_tree clock_pll_sel = {
    .name           = "pll_sel",
    .parents_max    = 4,
    .parents        = (struct clk_tree *[]) {
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

static struct clk_tree clock_f32k_sel = {
    .name           = "f32k_sel",
    .parents_max    = 3,
    .parents        = (struct clk_tree *[]) {
        &clock_XTAL,
        &clock_XTAL32K,
        &clock_RC32K,
        NULL,
    },
    .parent         = clock_root_clk_f32k_sel_idx,
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_f32k = {
    .name           = "f32k_clock",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_f32k_sel,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_clk_sel_1_parent_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_HBN_ROOT_CLK_SEL);

    //will use idx 1
    return ((tmpVal >> 1) & 0x1) ? 1 : 0;
}

static int clock_root_clk_sel_1_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV);

    return tmpVal + 1;
}

static struct clk_tree clock_root_clk_sel_1 = {
    .name           = "root_clk_sel_1",
    .parents_max    = 2,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_0,
        &clock_pll_sel,
        NULL,
    },
    .parent         = clock_root_clk_sel_1_parent_idx,
    .div_get        = clock_root_clk_sel_1_div_get,
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_fclk = {
    .name           = "fclk",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_1,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_hclk = {
    .name           = "hclk",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_1,
        NULL,
    },
    //XXX Clock Enable
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_bclk_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV);

    return tmpVal + 1;
}

static struct clk_tree clock_bclk = {
    .name           = "bclk",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_1,
        NULL,
    },
    .div_get        = clock_root_bclk_div_get,
    //XXX Clock Enable
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_RC32M = {
    .name           = "RC 32M",
    .parents_max    = 0,
    .parents        = (struct clk_tree *[]) {
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_RC32K = {
    .name           = "RC 32k",
    .parents_max    = 0,
    .parents        = (struct clk_tree *[]) {
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_xtal_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);

    return tmpVal + 1;
}

static int clock_xtal_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static struct clk_tree clock_XTAL = {
    .name           = "XTAL 40M",
    .parents_max    = 0,
    .parents        = (struct clk_tree *[]) {
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_XCLK = {
    .name           = "XCLK",
    .parents_max    = 2,
    .parents        = (struct clk_tree *[]) {
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_XTAL32K = {
    .name           = "XTAL 32K",
    .parents_max    = 0,
    .parents        = (struct clk_tree *[]) {
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_root_clk_pll_root_parent_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;
    
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CLKPLL_TOP_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_CLKPLL_XTAL_RC32M_SEL);

    return tmpVal;
}

static struct clk_tree clock_PLL = {
    .name           = "PLL Root",
    .parents_max    = 2,
    .parents        = (struct clk_tree *[]) {
        &clock_XTAL,
        &clock_RC32M,
        NULL,
    },
    .parent         = clock_root_clk_pll_root_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_pll_192m = {
    .name           = "PLL_192M",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_pll_160m = {
    .name           = "PLL_160M",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_pll_120m = {
    .name           = "PLL_120M",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_pll_96m = {
    .name           = "PLL_96m",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_pll_80m = {
    .name           = "PLL_80m",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_pll_48m = {
    .name           = "PLL_48m",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree clock_pll_32m = {
    .name           = "PLL_32m",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_PLL,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_uart_parent_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL);

    return tmpVal;
}

static int clock_uart_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_uart_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_DIV);

    return tmpVal + 1;
}

static int clock_pwm_parent_idx(struct clk_tree *node)
{ 
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE + 0x20, PWM_CONFIG);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);

    return tmpVal;
}

static int clock_pwm1_parent_idx(struct clk_tree *node)
{ 
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE + 0x40, PWM_CONFIG);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);

    return tmpVal;
}

static int clock_pwm2_parent_idx(struct clk_tree *node)
{ 
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE + 0x60, PWM_CONFIG);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);

    return tmpVal;
}

static int clock_pwm3_parent_idx(struct clk_tree *node)
{ 
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE+0x80, PWM_CONFIG);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);

    return tmpVal;
}

static int clock_pwm4_parent_idx(struct clk_tree *node)
{ 
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE+0xA0, PWM_CONFIG);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);

    return tmpVal;
}

static int clock_adc_parent_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_SEL);

    return tmpVal;
}

static int clock_dac_parent_idx(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_CLK_SRC_SEL);

    return tmpVal;
}

/* nodes of clock consumer*/
static struct clk_tree clock_uart = {
    .name           = "uart clk_tree",
    .parents_max    = 2,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_1,        //idx 0
        &clock_pll_160m ,             //idx 1
        NULL,
    },
    .parent         = clock_uart_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_uart_gating_status,
    .div_get        = clock_uart_div_get,
};

static struct clk_tree leaf_mcu_fclk = {
    .name           = "MCU fclk",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_fclk,        //idx 0
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree leaf_mcu_hclk = {
    .name           = "MCU hclk",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_hclk,        //idx 0
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static struct clk_tree leaf_mcu_bclk = {
    .name           = "MCU bclk",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_bclk,        //idx 0
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
};

static int clock_flash_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);

    return tmpVal + 1;
}

static int clock_flash_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_flash_parent_idx(struct clk_tree *node)
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

static struct clk_tree flash_clock = {
    .name           = "flash clk_tree",
    .parents_max    = 6,
    .parents        = (struct clk_tree *[]) {
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

static int clock_i2c_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_i2c_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV);

    return tmpVal + 1;
}

static int clock_pwm_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE + 0x20, PWM_CLKDIV);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_CLK_DIV);

    return tmpVal + 1;
}

static int clock_pwm1_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE + 0x40, PWM_CLKDIV);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_CLK_DIV);

    return tmpVal + 1;
}

static int clock_pwm2_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE + 0x60, PWM_CLKDIV);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_CLK_DIV);

    return tmpVal + 1;
}

static int clock_pwm3_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE + 0x80, PWM_CLKDIV);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_CLK_DIV);

    return tmpVal + 1;
}

static int clock_pwm4_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PWM_BASE+0xA0, PWM_CLKDIV);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_CLK_DIV);

    return tmpVal + 1;
}

static int clock_spi_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_spi_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV);

    return tmpVal + 1;
}

static int clock_ir_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_ir_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_DIV);

    return tmpVal + 1;
}

static int clock_adc_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_DIV_EN);

    return tmpVal == 1 ? 0 : 1;
}

static int clock_adc_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_DIV);

    return tmpVal + 1;
}

static int clock_dac_div_get(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_DIV);

    return tmpVal + 1;
}

static int clock_dac_gating_status(struct clk_tree *node)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_512K_DIV);

    return tmpVal == 1 ? 0 : 1;
}

static struct clk_tree i2c_clock = {
    .name           = "i2c clk_tree",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_bclk,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_i2c_gating_status,
    .div_get        = clock_i2c_div_get
};

static struct clk_tree spi_clock = {
    .name           = "spi clk_tree",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_bclk,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_spi_gating_status,
    .div_get        = clock_spi_div_get
};

static struct clk_tree ir_clock = {
    .name           = "IR-remote_clock clk_tree",
    .parents_max    = 1,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_0,
        NULL,
    },
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_ir_gating_status,
    .div_get        = clock_ir_div_get
};

static struct clk_tree adc_clock = {
    .name           = "adc_clock clk_tree",
    .parents_max    = 2,
    .parents        = (struct clk_tree *[]) {
        &clock_pll_96m,
        &clock_root_clk_sel_0,
        NULL,
    },
    .parent         = clock_adc_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_adc_gating_status,
    .div_get        = clock_adc_div_get,
};

static struct clk_tree pwm_clock = {
    .name           = "pwm0 clk_tree",
    .parents_max    = 3,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_0,
        &clock_bclk,
        &clock_f32k,
        NULL,
    },
    .parent         = clock_pwm_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = NULL,
    .div_get        = clock_pwm_div_get,
};

static struct clk_tree pwm1_clock = {
    .name           = "pwm1 clk_tree",
    .parents_max    = 3,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_0,
        &clock_bclk,
        &clock_f32k,
        NULL,
    },
    .parent         = clock_pwm1_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = NULL,
    .div_get        = clock_pwm1_div_get,
};

static struct clk_tree pwm2_clock = {
    .name           = "pwm2 clk_tree",
    .parents_max    = 3,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_0,
        &clock_bclk,
        &clock_f32k,
        NULL,
    },
    .parent         = clock_pwm2_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = NULL,
    .div_get        = clock_pwm2_div_get,
};

static struct clk_tree pwm3_clock = {
    .name           = "pwm3 clk_tree",
    .parents_max    = 3,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_0,
        &clock_bclk,
        &clock_f32k,
        NULL,
    },
    .parent         = clock_pwm3_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = NULL,
    .div_get        = clock_pwm3_div_get,
};

static struct clk_tree pwm4_clock = {
    .name           = "pwm4 clk_tree",
    .parents_max    = 3,
    .parents        = (struct clk_tree *[]) {
        &clock_root_clk_sel_0,
        &clock_bclk,
        &clock_f32k,
        NULL,
    },
    .parent         = clock_pwm4_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = NULL,
    .div_get        = clock_pwm4_div_get,
};

static struct clk_tree dac_clock = {
    .name           = "dac clk_tree",
    .parents_max    = 2,
    .parents        = (struct clk_tree *[]) {
        &clock_pll_32m,
        &clock_root_clk_sel_0,
        NULL,
    },
    .parent         = clock_dac_parent_idx,
    .enable         = NULL,
    .disable        = NULL,
    .cg             = clock_dac_gating_status,
    .div_get        = clock_dac_div_get,
};

static struct clk_tree *clock_leaves[] = {
    &flash_clock,
    &i2c_clock,
    &spi_clock,
    &clock_uart,
    &pwm_clock,
    &pwm1_clock,
    &pwm2_clock,
    &pwm3_clock,
    &pwm4_clock,
    &ir_clock,
    &adc_clock,
    &dac_clock,
    &leaf_mcu_fclk,
    &leaf_mcu_hclk,
    &leaf_mcu_bclk,
};

static void _dump_node(struct clk_tree *node)
{
    int i;

    printf("node name %p %s\r\n", node, node->name);
    for (i = 0; i < node->parents_max; i++) {
        printf("parents[%i] %p\r\n", i, node->parents[i]);
    }
}

static void dump_clock_node(struct clk_tree *node)
{
    struct clk_tree *parent;
    int idx;

    puts(node->name);
    printf(" (Div %u)", node->div_get ? node->div_get(node) : 1);
    puts(node->cg && (1 == node->cg(node)) ? "(Gating)" : "");

    if (0 == node->parents_max) {
        return;
    }

    puts(" <<<");
    if (node->parent) {
        idx = node->parent(node);
        if (idx >= node->parents_max || idx < 0) {
            printf("Got illegal idx %d\r\n", idx);
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
    printf("(%d)-- ", idx);
    parent = node->parents[idx];

    if (NULL == parent) {
        _dump_node(node);
    }
    dump_clock_node(parent); //XXX Stack OverFlow Caution
}

void bugkiller_clocktree_dump(void)
{
    int i;
    struct clk_tree *leaf;

    for (i = 0; i < sizeof(clock_leaves)/sizeof(clock_leaves[0]); i++) {
        leaf = clock_leaves[i];
        dump_clock_node(leaf);
        printf("\r\n==================\r\n");
    }
}

