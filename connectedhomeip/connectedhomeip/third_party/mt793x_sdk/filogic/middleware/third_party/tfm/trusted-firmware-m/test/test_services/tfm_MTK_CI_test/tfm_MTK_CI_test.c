/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "tfm_api.h"
#include "mt7933.h"
#include "hal_devapc.h"
#include "hal_asic_mpu.h"
#include "hal_asic_mpu_internal.h"
#include "pltfm_iso_type.h"

extern const DEVICE_INFO INFRA_Devices[];
extern const DEVICE_INFO AUD_Devices[];
extern const ASIC_MPU_INFO ASIC_MPU_Devices[][REGION_NUM];

psa_status_t tfm_ci_exception_handle_test(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec)
{
    __asm volatile("udf #255");

    return TFM_ERROR_GENERIC;
}

psa_status_t tfm_ci_platform_isolation_test(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec)
{
    int32_t ret = TFM_SUCCESS;
    uint32_t slv_mod = 0;
    hal_devapc_dom_t dom;
    hal_devapc_apc_t apc = 0, tab_apc;

    //Infra dapc test
    for(dom = HAL_DEVAPC_DOMAIN_0; dom < HAL_DEVAPC_DOMAIN_MAX; dom++, slv_mod = 0)
    {
        while(INFRA_Devices[slv_mod].addr)
        {
            if(hal_devapc_get_module_apc(HAL_DEVAPC_TYPE_INFRA, slv_mod, dom, &apc))
            {
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Get Infra APC fail\n");
            }

            tab_apc = INFRA_Devices[slv_mod].perm[dom];
            if(apc != tab_apc)
            {
                printf("apc = 0x%x, tab_apc = 0x%x\n", apc, tab_apc);
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Infra APC configurations compare fail\n");
                ret = TFM_ERROR_GENERIC;
            }

            //lock test
            tab_apc = (tab_apc + 1) % 4;
            if(hal_devapc_set_module_apc(HAL_DEVAPC_TYPE_INFRA, slv_mod, dom, tab_apc))
            {
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Set Infra APC fail\n");
            }
            if(hal_devapc_get_module_apc(HAL_DEVAPC_TYPE_INFRA, slv_mod, dom, &apc))
            {
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Get Infra APC fail\n");
            }
            if(apc == tab_apc)
            {
                printf("apc = 0x%x, tab_apc = 0x%x\n", apc, tab_apc);
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Infra APC lock test fail\n");
                ret = TFM_ERROR_GENERIC;
            }

            slv_mod++;
        }
    }

#ifdef MTK_AUDIO_SUPPORT
    //Audio DAPC APC test
    for(hal_devapc_dom_t dom = HAL_DEVAPC_DOMAIN_0; dom < HAL_DEVAPC_DOMAIN_MAX; dom++, slv_mod = 0)
    {
        while(AUD_Devices[slv_mod].addr)
        {
            if(hal_devapc_get_module_apc(HAL_DEVAPC_TYPE_AUD, slv_mod, dom, &apc))
            {
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Get Audio APC fail\n");
            }

            tab_apc = AUD_Devices[slv_mod].perm[dom];
            if(apc != tab_apc)
            {
                printf("apc = 0x%x, tab_apc = 0x%x\n", apc, tab_apc);
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Audio APC configurations compare fail\n");
                ret = TFM_ERROR_GENERIC;
            }

            //lock test
            tab_apc = (tab_apc + 1) % 4;
            if(hal_devapc_set_module_apc(HAL_DEVAPC_TYPE_AUD, slv_mod, dom, tab_apc))
            {
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Set Audio APC fail\n");
            }
            if(hal_devapc_get_module_apc(HAL_DEVAPC_TYPE_AUD, slv_mod, dom, &apc))
            {
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Get Infra APC fail\n");
            }
            if(apc == tab_apc)
            {
                printf("apc = 0x%x, tab_apc = 0x%x\n", apc, tab_apc);
                printf("dom = %u, slv_mod = %u\n", dom, slv_mod);
                printf("Audio APC lock test fail\n");
                ret = TFM_ERROR_GENERIC;
            }

            slv_mod++;
        }
    }

#endif /* MTK_AUDIO_SUPPORT */

#undef ASIC_MPU_BASE
#define ASIC_MPU_BASE    (0xC0000000)

    unsigned int tb_cfg_val = 0, val = 0;
    hal_asic_mpu_type_t mem_typ;
    unsigned int region;

    //ASIC_MPU region configurations test
    for(mem_typ = HAL_ASIC_MPU_TYPE_FLASH; mem_typ < MPU_NUM; mem_typ++)
    {
        for(region = 0; region < REGION_NUM; region++)
        {
            if(hal_asic_mpu_get_region_cfg(mem_typ, region, &val))
            {
                printf("mem_typ = %u, region = %u\n", mem_typ, region);
                printf("Get ASIC_MPU region configuration fail\n");
            }
            tb_cfg_val = (ASIC_MPU_Devices[mem_typ][region].addr >> 12);
            if(ASIC_MPU_Devices[mem_typ][region].enable != DISABLE)
            {
                if(val != tb_cfg_val)
                {
                    printf("mem_typ = %u, region = %u, val = 0x%x, tb_cfg_val = 0x%x\n", mem_typ, region, val, tb_cfg_val);
                    printf("ASIC_MPU_Devices[mem_typ][region].addr = 0x%x\n", ASIC_MPU_Devices[mem_typ][region].addr);
                    printf("platform isolation ASIC_MPU region configurations compare fail\n");
                    ret = TFM_ERROR_GENERIC;
                }
            }
            else if((tb_cfg_val == 0 && val != 0) || (tb_cfg_val != 0 && val == 0))
            {
                printf("mem_typ = %u, region = %u, val = 0x%x, tb_cfg_val = 0x%x\n", mem_typ, region, val, tb_cfg_val);
                printf("ASIC_MPU_Devices[mem_typ][region].addr = 0x%x\n", ASIC_MPU_Devices[mem_typ][region].addr);
                printf("platform isolation ASIC_MPU region configurations not match\n");
                ret = TFM_ERROR_GENERIC;
            }
            else
            {
                break;
            }

            //region lock test
            #define ASIC_MPU_TEST_ADDR 0x12345678
            if(!hal_asic_mpu_set_region_cfg(mem_typ, region, 1, ASIC_MPU_TEST_ADDR))
            {
                if(hal_asic_mpu_get_region_cfg(mem_typ, region, &val))
                {
                    printf("mem_typ = %u, region = %u\n", mem_typ, region);
                    printf("Get ASIC_MPU region configuration fail\n");
                }

                tb_cfg_val = (ASIC_MPU_TEST_ADDR >> 12);
                if(val == tb_cfg_val)
                {
                    printf("mem_typ = %u, region = %u, val = 0x%x, tb_cfg_val = 0x%x\n", mem_typ, region, val, tb_cfg_val);
                    printf("platform isolation ASIC_MPU lock test fail\n");
                    ret = TFM_ERROR_GENERIC;
                }
            }
            else
            {
                printf("ASIC_MPU set region config fail\n");
            }
        }
    }

    //ASIC_MPU APC configurations test
    for(mem_typ = HAL_ASIC_MPU_TYPE_FLASH; mem_typ < MPU_NUM; mem_typ++)
    {
        for(region = 0; (region < REGION_NUM) && (ASIC_MPU_Devices[mem_typ][region].enable != DISABLE); region++)
        {
            for(dom = HAL_DEVAPC_DOMAIN_0; dom < HAL_DEVAPC_DOMAIN_MAX; dom++, tb_cfg_val = 0)
            {
                if(hal_asic_mpu_get_region_apc(mem_typ, region, dom, &val))
                {
                    printf("mem_typ = %u, region = %u, dom = %u\n", mem_typ, region, dom);
                    printf("Get ASIC_MPU APC configuration fail\n");
                }
                tb_cfg_val = ASIC_MPU_Devices[mem_typ][region].perm[dom];
                if(val != tb_cfg_val)
                {
                    printf("mem_typ = %u, region = %u\n, dom = %u", mem_typ, region, dom);
                    printf("val = 0x%x, tb_cfg_val = 0x%x\n", val, tb_cfg_val);
                    printf("platform isolation ASIC_MPU APC configurations compare fail\n");
                    ret = TFM_ERROR_GENERIC;
                }

                val = (val + 1) % HAL_ASIC_MPU_APC_MAX;
                if(hal_asic_mpu_set_region_apc(mem_typ, region, dom, val))
                {
                    printf("mem_typ = %u, region = %u, dom = %u\n", mem_typ, region, dom);
                    printf("Set ASIC_MPU APC configuration fail\n");
                }

                if(hal_asic_mpu_get_region_apc(mem_typ, region, dom, &tb_cfg_val))
                {
                    printf("mem_typ = %u, region = %u, dom = %u\n", mem_typ, region, dom);
                    printf("Get ASIC_MPU APC configuration fail\n");
                }

                if(val == tb_cfg_val)
                {
                    printf("mem_typ = %u, region = %u, val = 0x%x, tb_cfg_val = 0x%x\n", mem_typ, region, val, tb_cfg_val);
                    printf("ASIC_MPU APC lock test fail\n");
                    ret = TFM_ERROR_GENERIC;
                }
            }
        }
    }

    return ret;
}

int32_t tfm_CI_test_init(psa_invec *in_vec, uint32_t num_invec, psa_outvec *out_vec, uint32_t num_outvec)
{
    return TFM_SUCCESS;
}
