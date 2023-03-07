/***************************************************************************//**
 * @file
 * @brief AoA MQTT topics.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef AOA_TOPICS_H
#define AOA_TOPICS_H

#define AOA_TOPIC_IQ_REPORT_PRINT    "silabs/aoa/iq_report/%s/%s"
#define AOA_TOPIC_IQ_REPORT_SCAN     "silabs/aoa/iq_report/%64[^/]/%64[^/]"
#define AOA_TOPIC_ANGLE_PRINT        "silabs/aoa/angle/%s/%s"
#define AOA_TOPIC_ANGLE_SCAN         "silabs/aoa/angle/%64[^/]/%64[^/]"
#define AOA_TOPIC_POSITION_PRINT     "silabs/aoa/position/%s/%s"
#define AOA_TOPIC_POSITION_SCAN      "silabs/aoa/position/%64[^/]/%64[^/]"
#define AOA_TOPIC_CORRECTION_PRINT   "silabs/aoa/correction/%s/%s"
#define AOA_TOPIC_CORRECTION_SCAN    "silabs/aoa/correction/%64[^/]/%64[^/]"
#define AOA_TOPIC_CONFIG_PRINT       "silabs/aoa/config/%s"
#define AOA_TOPIC_CONFIG_SCAN        "silabs/aoa/config/%64[^/]"
#define AOA_TOPIC_CONFIG_BROADCAST   "silabs/aoa/config"

#endif // AOA_TOPICS_H
