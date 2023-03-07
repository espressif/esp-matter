/*
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <inttypes.h>
#include <stdio.h>

#include "driver_smpu.h"
#include "pc_config.h"
#include "region_defs.h"
#include "RTE_Device.h"
#include "smpu_config.h"
#include "log/tfm_log.h"

#include "cy_prot.h"

/* Affect all 8 subregions */
#define ALL_ENABLED 0

struct smpu_resources {
    PROT_SMPU_SMPU_STRUCT_Type *smpu;
    cy_stc_smpu_cfg_t slave_config;
    cy_stc_smpu_cfg_t master_config;
};

static const char * smpu_name(const SMPU_Resources *smpu_dev)
{
    switch ((int)smpu_dev->smpu) {
    case (int)PROT_SMPU_SMPU_STRUCT0:
        return "SMPU 0";
    case (int)PROT_SMPU_SMPU_STRUCT1:
        return "SMPU 1";
    case (int)PROT_SMPU_SMPU_STRUCT2:
        return "SMPU 2";
    case (int)PROT_SMPU_SMPU_STRUCT3:
        return "SMPU 3";
    case (int)PROT_SMPU_SMPU_STRUCT4:
        return "SMPU 4";
    case (int)PROT_SMPU_SMPU_STRUCT5:
        return "SMPU 5";
    case (int)PROT_SMPU_SMPU_STRUCT6:
        return "SMPU 6";
    case (int)PROT_SMPU_SMPU_STRUCT7:
        return "SMPU 7";
    case (int)PROT_SMPU_SMPU_STRUCT8:
        return "SMPU 8";
    case (int)PROT_SMPU_SMPU_STRUCT9:
        return "SMPU 9";
    case (int)PROT_SMPU_SMPU_STRUCT10:
        return "SMPU 10";
    case (int)PROT_SMPU_SMPU_STRUCT11:
        return "SMPU 11";
    case (int)PROT_SMPU_SMPU_STRUCT12:
        return "SMPU 12";
    case (int)PROT_SMPU_SMPU_STRUCT13:
        return "SMPU 13";
    case (int)PROT_SMPU_SMPU_STRUCT14:
        return "SMPU 14";
    case (int)PROT_SMPU_SMPU_STRUCT15:
        return "SMPU 15";
    default:
        return "Unrecognised SMPU";
    }
}

/* API functions */

void SMPU_Print_Config(const SMPU_Resources *smpu_dev)
{
    printf("%s - address = %p, size = %#"PRIx32" bytes, %s subregions enabled\n",
           smpu_name(smpu_dev),
           smpu_dev->slave_config.address,
           (uint32_t)REGIONSIZE_TO_BYTES(smpu_dev->slave_config.regionSize),
           smpu_dev->slave_config.subregions == ALL_ENABLED ? "all" : "some");
    if (smpu_dev->slave_config.subregions != ALL_ENABLED) {
        printf("\tsubregion size = %#"PRIx32" bytes\n",
            (uint32_t)REGIONSIZE_TO_BYTES(smpu_dev->slave_config.regionSize)/8);
        for (int i=0; i<8; i++) {
            printf("\tsubregion %d %s\n",
                   i,
                   smpu_dev->slave_config.subregions & (1<<i) ? "disabled" : "enabled");
        }
    }
}

cy_en_prot_status_t SMPU_Configure(const SMPU_Resources *smpu_dev)
{
    cy_en_prot_status_t ret;

    ret = Cy_Prot_ConfigSmpuSlaveStruct(smpu_dev->smpu,
                                        &smpu_dev->slave_config);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }
    ret = Cy_Prot_ConfigSmpuMasterStruct(smpu_dev->smpu,
                                         &smpu_dev->master_config);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }
    ret = Cy_Prot_EnableSmpuSlaveStruct(smpu_dev->smpu);
    if (ret != CY_PROT_SUCCESS) {
        return ret;
    }
    ret = Cy_Prot_EnableSmpuMasterStruct(smpu_dev->smpu);
    return ret;
}

/* Only allow privileged secure PC=1 bus masters to change unconfigured SMPUs */
cy_en_prot_status_t protect_unconfigured_smpus(void)
{
    const cy_stc_smpu_cfg_t smpu_config = COMMON_SMPU_MASTER_CONFIG;
    cy_en_prot_status_t ret = CY_PROT_SUCCESS;
    int i;
    uint32_t att0, att1;

    for (i = 0; i < CPUSS_PROT_SMPU_STRUCT_NR; i++) {
        att0 = PROT->SMPU.SMPU_STRUCT[i].ATT0;
        att1 = PROT->SMPU.SMPU_STRUCT[i].ATT1;

        if ((_FLD2VAL(PROT_SMPU_SMPU_STRUCT_ATT0_ENABLED, att0) == 0)
            && (_FLD2VAL(PROT_SMPU_SMPU_STRUCT_ATT1_ENABLED, att1) == 0)) {

            ret = Cy_Prot_ConfigSmpuMasterStruct(&PROT->SMPU.SMPU_STRUCT[i],
                                                 &smpu_config);
            if (ret != CY_PROT_SUCCESS) {
                break;
            }
            ret = Cy_Prot_EnableSmpuMasterStruct(&PROT->SMPU.SMPU_STRUCT[i]);
            if (ret != CY_PROT_SUCCESS) {
                break;
            }
        }
    }

    return ret;
}

/* Exported per-SMPU macros */
#define DEFINE_SMPU(N) const SMPU_Resources SMPU##N##_Resources = { \
    .smpu = PROT_SMPU_SMPU_STRUCT##N, \
    .slave_config = SMPU##N##_SLAVE_CONFIG, \
    .master_config = SMPU##N##_MASTER_CONFIG, \
}; \

#if (RTE_SMPU0)
DEFINE_SMPU(0)
#endif /* RTE_SMPU0 */

#if (RTE_SMPU1)
DEFINE_SMPU(1)
#endif /* RTE_SMPU1 */

#if (RTE_SMPU2)
DEFINE_SMPU(2)
#endif /* RTE_SMPU2 */

#if (RTE_SMPU3)
DEFINE_SMPU(3)
#endif /* RTE_SMPU3 */

#if (RTE_SMPU4)
DEFINE_SMPU(4)
#endif /* RTE_SMPU4 */

#if (RTE_SMPU5)
DEFINE_SMPU(5)
#endif /* RTE_SMPU5 */

#if (RTE_SMPU6)
DEFINE_SMPU(6)
#endif /* RTE_SMPU6 */

#if (RTE_SMPU7)
DEFINE_SMPU(7)
#endif /* RTE_SMPU7 */

#if (RTE_SMPU8)
DEFINE_SMPU(8)
#endif /* RTE_SMPU8 */

#if (RTE_SMPU9)
DEFINE_SMPU(9)
#endif /* RTE_SMPU9 */

#if (RTE_SMPU10)
DEFINE_SMPU(10)
#endif /* RTE_SMPU10 */

#if (RTE_SMPU11)
DEFINE_SMPU(11)
#endif /* RTE_SMPU11 */

#if (RTE_SMPU12)
DEFINE_SMPU(12)
#endif /* RTE_SMPU12 */

#if (RTE_SMPU13)
DEFINE_SMPU(13)
#endif /* RTE_SMPU13 */

/* Note that SMPUs 14 and 15 are fixed by romboot */
