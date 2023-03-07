/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/*
 * Copyright (c) 2019 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file psa/error.h
 *
 * \brief Supplementary error codes for the SPM and RoT Services
 *        as defined in PSA Firmware Framework v1.0, which are not present
 *        in crypto_values.h
 */

#ifndef __PSA_ERROR__
#define __PSA_ERROR__

#define PSA_ERROR_PROGRAMMER_ERROR      ((psa_status_t)-129)
#define PSA_ERROR_CONNECTION_REFUSED    ((psa_status_t)-130)
#define PSA_ERROR_CONNECTION_BUSY       ((psa_status_t)-131)
#define PSA_ERROR_SERVICE_FAILURE       ((psa_status_t)-144)

#endif // __PSA_ERROR__
