/***************************************************************************//**
 * @file
 * @brief Configuration file for TensorFlow Lite Micro.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_TFLITE_MICRO_CONFIG_H
#define SL_TFLITE_MICRO_CONFIG_H

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/


// <e SL_TFLITE_MICRO_INTERPRETER_INIT_ENABLE> Automatically initialize model
// <i> If this is enabled, TensorFlow Lite for Microcontrollers will be 
// <i> initalized using metadata from the flatbuffer model in the 
// <i> configuration folder.
// <i> This includes instantiating an interpreter based on the model 
// <i> operations and allocating memory required for the tensors. Modify
// <i> the memory allocation size below.
// <i> Default: 1
#define SL_TFLITE_MICRO_INTERPRETER_INIT_ENABLE    (1)

// <o SL_TFLITE_MICRO_ARENA_SIZE> Tensor Arena Size
// <i> TensorFlow Lite for Microcontrollers requires a certain amount of 
// <i> preallocated working memory for input, output and intermediate arrays.
// <i> The memory requirement varies between models, and is best found by trial 
// <i> and error.
// <i> Note: Models provided by the MLTK (Silicon Labs Machine Learning Toolkit)
// <i> have a pre-configured arena size associated with them. Changing this
// <i> configuration to a non-zero value will overwrite any pre-configured value
// <i> in the model's metadata.
// <i> Default: 0
#define SL_TFLITE_MICRO_ARENA_SIZE                 (0)
// </e>

#endif // SL_TFLITE_MICRO_CONFIG_H

// <<< end of configuration section >>>
