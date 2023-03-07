/*
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
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

#ifndef __PC_CONFIG_H__
#define __PC_CONFIG_H__

/* Which PC is used for what */
/* PC=0 : romboot (CM0+) */
/* PC=1 : BL2/SPM (CM0+) */
#define CY_PROT_SPM_DEFAULT     CY_PROT_PC1
/* PC=2 : unused (secure) */
/* PC=3 : unused (secure) */
/* PC=4 : unused (secure) */
/* PC=5 : unused (non-secure) */
/* PC=6 : non-secure code (CM4) */
#define CY_PROT_HOST_DEFAULT    CY_PROT_PC6
/* PC=7 : Test Controller */
#define CY_PROT_TC              CY_PROT_PC7

#define ONLY_BL2_SPM_MASK       CY_PROT_PCMASK1
#define SECURE_PCS_MASK         (CY_PROT_PCMASK1 | CY_PROT_PCMASK2 | CY_PROT_PCMASK3 | \
                                 CY_PROT_PCMASK4)
#define HOST_PCS_MASK           (CY_PROT_PCMASK5 | CY_PROT_PCMASK6)
#define TC_PC_MASK              CY_PROT_PCMASK7
#define ALL_PCS_EXCEPT_TC_MASK  (CY_PROT_PCMASK1 | CY_PROT_PCMASK2 | CY_PROT_PCMASK3 | \
                                 CY_PROT_PCMASK4 | CY_PROT_PCMASK5 | CY_PROT_PCMASK6)
#define ALL_PCS_MASK            (CY_PROT_PCMASK1 | CY_PROT_PCMASK2 | CY_PROT_PCMASK3 | \
                                 CY_PROT_PCMASK4 | CY_PROT_PCMASK5 | CY_PROT_PCMASK6 | CY_PROT_PCMASK7)
#endif /* __PC_CONFIG_H__ */

