/***************************************************************************//**
 * @brief Zigbee Fragmentation component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Fragmentation configuration

// <o EMBER_AF_PLUGIN_FRAGMENTATION_MAX_INCOMING_PACKETS> Max incoming fragmented packets <1-10>
// <i> Default: 1
// <i> Indicates the maximum number of simultaneous incoming fragmented packets that the node will be able to handle. Notice that each entry requires a buffer for storing the incoming fragmented packet
#define EMBER_AF_PLUGIN_FRAGMENTATION_MAX_INCOMING_PACKETS   1

// <o EMBER_AF_PLUGIN_FRAGMENTATION_MAX_OUTGOING_PACKETS> Max outgoing fragmented packets <1-10>
// <i> Default: 1
// <i> Indicates the maximum number of simultaneous outgoing fragmented packets that the node will be able to handle. Notice that each entry requires a buffer for storing the outgoing fragmented packet
#define EMBER_AF_PLUGIN_FRAGMENTATION_MAX_OUTGOING_PACKETS   1

// <o EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE> Max payload size prior to fragmentation <74-10000>
// <i> Default: 255
// <i> Indicates the maximum size in bytes of the payload of a packet that can be handled by the fragmentation plugin
#define EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE   255

// <q EMBER_AF_PLUGIN_FRAGMENTATION_FREE_OUTGOING_MESSAGE_PRIOR_TO_FINAL_ACK> Free outgoing message buffer prior to final acknowledgement
// <i> Default: FALSE
// <i> This allow the fragmentation plugin to free the outgoing message buffer prior to receiving the final acknowledgement.  This helps in situations where an application wants to immediately turn around and send a new fragmented transmission after completing the preivous one.  However, the message sent handler will NOT have access to the buffer with the transmitted message after success or failure.
#define EMBER_AF_PLUGIN_FRAGMENTATION_FREE_OUTGOING_MESSAGE_PRIOR_TO_FINAL_ACK   0

// </h>

// <<< end of configuration section >>>
