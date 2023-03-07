/***************************************************************************//**
 * @file
 * @brief CPC configuration file.
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_CPC_SECURITY_CONFIG_H
#define SL_CPC_SECURITY_CONFIG_H

// <h>CPC Security Configuration

// <q SL_CPC_SECURITY_ENABLED> Enable encryption using the security endpoint
// <i> Default: 0
#define SL_CPC_SECURITY_ENABLED    1

// <o SL_CPC_SECURITY_BINDING_KEY_METHOD> Binding key method
// <i>
// <SL_CPC_SECURITY_BINDING_KEY_CUSTOMER_SPECIFIC=> The customer provides the key.
// <SL_CPC_SECURITY_BINDING_KEY_ECDH=> The key is exchanged using the Elliptic-curve Diffie-Hellman algorithm
// <SL_CPC_SECURITY_BINDING_KEY_PLAINTEXT_SHARE=> The key is plaintext key share. The host sends its encryption key to the secondary
// <SL_CPC_SECURITY_BINDING_KEY_NONE=> No binding key is provided
// <i> Default: SL_CPC_SECURITY_BINDING_KEY_ECDH
#define SL_CPC_SECURITY_BINDING_KEY_METHOD SL_CPC_SECURITY_BINDING_KEY_ECDH

// </h>

// <<< end of configuration section >>>

#endif /* SL_CPC_CONFIG_H */
