/***************************************************************************//**
 * @file
 * @brief Backbone of CLI framework
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_CLI_H
#define SL_CLI_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#include "sl_cli_types.h"
#include "sl_cli_command.h"
#include "sl_cli_arguments.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Obtain the number of command arguments.
 *
 * @details
 *  Users obtain the number of command arguments with this macro.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @return
 *   The number of command arguments.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the number of arguments (after the command strings).
 *     int arg_count = sl_cli_get_argument_count(args); // arg_count = 4
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_count(a)      ((int)((a)->argc - (a)->arg_ofs))

/***************************************************************************//**
 * @brief
 *  Obtain the type of argument.
 *
 * @details
 *  Users obtain the type of the argument with this macro.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   enum of the argument type.
 *
 * @details
 *   For auto-generated commands, it could be useful in the handler to know
 *   the type of the argument.
 *
 ******************************************************************************/
#define sl_cli_get_argument_type(a, n)      ((sl_cli_arg_t)((a)->arg_type_list[(n)]))

/***************************************************************************//**
 * @brief
 *  Obtain int8 arguments.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   The argument value.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the arguments (after the command strings).
 *     char *text_ptr = sl_cli_get_argument_string(args, 0);  // char ptr to
 *     // "hello"
 *     uint8_t num1_u8 = sl_cli_get_argument_uint8(args, 1);  // num1_u8 = 255
 *     int16_t num2_i16 = sl_cli_get_argument_int16(args, 2); // num2_i16 = -100
 *     size_t hex_array_size;
 *     uint8_t *hex_array_ptr = sl_cli_get_argument_hex(args, 3, &hex_array_size);
 *     // uint8_t ptr to {0xAA, 0xBB, 0xCC}, hex_array_size = 3
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_int8(a, n)    (*(int8_t *)((a)->argv[(a)->arg_ofs + (n)]))

/***************************************************************************//**
 * @brief
 *  Obtain int16 arguments.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   The argument value.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the arguments (after the command strings).
 *     char *text_ptr = sl_cli_get_argument_string(args, 0);  // char ptr to
 *     // "hello"
 *     uint8_t num1_u8 = sl_cli_get_argument_uint8(args, 1);  // num1_u8 = 255
 *     int16_t num2_i16 = sl_cli_get_argument_int16(args, 2); // num2_i16 = -100
 *     size_t hex_array_size;
 *     uint8_t *hex_array_ptr = sl_cli_get_argument_hex(args, 3, &hex_array_size);
 *     // uint8_t ptr to {0xAA, 0xBB, 0xCC}, hex_array_size = 3
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_int16(a, n)   (*(int16_t *)((a)->argv[(a)->arg_ofs + (n)]))

/***************************************************************************//**
 * @brief
 *  Obtain int32 arguments.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   The argument value.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the arguments (after the command strings).
 *     char *text_ptr = sl_cli_get_argument_string(args, 0);  // char ptr to
 *     // "hello"
 *     uint8_t num1_u8 = sl_cli_get_argument_uint8(args, 1);  // num1_u8 = 255
 *     int16_t num2_i16 = sl_cli_get_argument_int16(args, 2); // num2_i16 = -100
 *     size_t hex_array_size;
 *     uint8_t *hex_array_ptr = sl_cli_get_argument_hex(args, 3, &hex_array_size);
 *     // uint8_t ptr to {0xAA, 0xBB, 0xCC}, hex_array_size = 3
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_int32(a, n)   (*(int32_t *)((a)->argv[(a)->arg_ofs + (n)]))

/***************************************************************************//**
 * @brief
 *  Obtain uint8 arguments.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   The argument value.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the arguments (after the command strings).
 *     char *text_ptr = sl_cli_get_argument_string(args, 0);  // char ptr to
 *     // "hello"
 *     uint8_t num1_u8 = sl_cli_get_argument_uint8(args, 1);  // num1_u8 = 255
 *     int16_t num2_i16 = sl_cli_get_argument_int16(args, 2); // num2_i16 = -100
 *     size_t hex_array_size;
 *     uint8_t *hex_array_ptr = sl_cli_get_argument_hex(args, 3, &hex_array_size);
 *     // uint8_t ptr to {0xAA, 0xBB, 0xCC}, hex_array_size = 3
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_uint8(a, n)   (*(uint8_t *)((a)->argv[(a)->arg_ofs + (n)]))

/***************************************************************************//**
 * @brief
 *  Obtain uint16 arguments.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   The argument value.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the arguments (after the command strings).
 *     char *text_ptr = sl_cli_get_argument_string(args, 0);  // char ptr to
 *     // "hello"
 *     uint8_t num1_u8 = sl_cli_get_argument_uint8(args, 1);  // num1_u8 = 255
 *     int16_t num2_i16 = sl_cli_get_argument_int16(args, 2); // num2_i16 = -100
 *     size_t hex_array_size;
 *     uint8_t *hex_array_ptr = sl_cli_get_argument_hex(args, 3, &hex_array_size);
 *     // uint8_t ptr to {0xAA, 0xBB, 0xCC}, hex_array_size = 3
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_uint16(a, n)  (*(uint16_t *)((a)->argv[(a)->arg_ofs + (n)]))

/***************************************************************************//**
 * @brief
 *  Obtain uint32 arguments.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   The argument value.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the arguments (after the command strings).
 *     char *text_ptr = sl_cli_get_argument_string(args, 0);  // char ptr to
 *     // "hello"
 *     uint8_t num1_u8 = sl_cli_get_argument_uint8(args, 1);  // num1_u8 = 255
 *     int16_t num2_i16 = sl_cli_get_argument_int16(args, 2); // num2_i16 = -100
 *     size_t hex_array_size;
 *     uint8_t *hex_array_ptr = sl_cli_get_argument_hex(args, 3, &hex_array_size);
 *     // uint8_t ptr to {0xAA, 0xBB, 0xCC}, hex_array_size = 3
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_uint32(a, n)  (*(uint32_t *)((a)->argv[(a)->arg_ofs + (n)]))

/***************************************************************************//**
 * @brief
 *  Obtain string arguments.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is the number 0, the next 1, etc.
 *
 * @return
 *   The argument value.
 *   For hex arguments, the return value is a pointer to a buffer containing
 *   decoded values. For string arguments, the return value is a pointer to the
 *   string.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the arguments (after the command strings).
 *     char *text_ptr = sl_cli_get_argument_string(args, 0);  // char ptr to
 *     // "hello"
 *     uint8_t num1_u8 = sl_cli_get_argument_uint8(args, 1);  // num1_u8 = 255
 *     int16_t num2_i16 = sl_cli_get_argument_int16(args, 2); // num2_i16 = -100
 *     size_t hex_array_size;
 *     uint8_t *hex_array_ptr = sl_cli_get_argument_hex(args, 3, &hex_array_size);
 *     // uint8_t ptr to {0xAA, 0xBB, 0xCC}, hex_array_size = 3
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_argument_string(a, n)  ((char *)((a)->argv[(a)->arg_ofs + (n)]))

/***************************************************************************//**
 * @brief
 *  Obtain the number of command strings.
 *
 * @details
 *  Users obtain the number of command strings with this macro.
 *
 * @param[in] a
 *   The number of command strings.
 *
 * @return
 *   The number of command strings.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the number of command strings (before the arguments).
 *     int cmd_count = sl_cli_get_command_count(args); // cmd_count = 2
 *     // (first string being "example_sub_menu" and second "example_command")
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_command_count(a)       ((int)((a)->arg_ofs))

/***************************************************************************//**
 * @brief
 *  Obtain the command string(s).
 *
 * @details
 *  Users get the command string with this macro. If the command is
 *  used in a group, both the group string(s) and command string can be obtained.
 *  The total number of command strings is available in the command argument
 *  arg_ofs variable.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The command string number. The first string is number 0, the next 1, etc.
 *
 * @details
 *   An example could be the CLI input of
 *   `example_sub_menu example_command hello 255 -100 {AA BB CC}`
 *   with corresponding C code of:
 *   ```c
 *   void example_command(sl_cli_command_arg_t *args)
 *   {
 *     // Get the command strings (before the arguments).
 *     char *cmd1_ptr = sl_cli_get_command_string(args, 0); // char ptr to
 *     // "example_sub_menu"
 *     char *cmd2_ptr = sl_cli_get_command_string(args, 1); // char ptr to
 *     // "example_command"
 *   }
 *   ```
 ******************************************************************************/
#define sl_cli_get_command_string(a, n)   ((char *)((a)->argv[(n)]))

/***************************************************************************//**
 *  @brief The default CLI handle.
 ******************************************************************************/
extern sl_cli_handle_t sl_cli_default_handle;

/***************************************************************************//**
 *  @brief The default CLI command group.
 ******************************************************************************/
extern sl_cli_command_group_t *sl_cli_default_command_group;

/***************************************************************************//**
 * @brief
 *  Get the hex argument length and value.
 *
 * @param[in] a
 *   A pointer to the command arguments.
 *
 * @param[in] n
 *   The argument number. The first argument is number 0, the next 1, etc.
 *
 * @param[in] l
 *   A pointer to the variable that receives the argument value length.
 *   Note: The length is stored in a 2 byte variable. Valid lengths are
 *   in the range 0 .. 65535.
 *
 * @return
 *   A pointer to the value buffer.
 ******************************************************************************/
uint8_t *sl_cli_get_argument_hex(sl_cli_command_arg_t *a, size_t n, size_t *l);

/***************************************************************************//**
 * @brief
 *  Clear (reset) a CLI.
 *
 * @param[in] handle
 *   A handle to the CLI that will be cleared.
 ******************************************************************************/
void sl_cli_clear(sl_cli_handle_t handle);

/***************************************************************************//**
 * @brief
 *  Redirect user input.
 *  Normally, an input string is passed to the CLI command handler for execution,
 *  but with the redirect function the input string will be passed to the
 *  alternative function.
 *  If the alternative input function should not to be used, this function
 *  can be called with NULL in the command_function, prompt and aux.
 *
 * @param[in] handle
 *   A handle to the CLI.
 *
 * @param[in] command_function
 *   A pointer to the function that will receive user input when the re-direct
 *   is active. NULL to stop the redirect.
 *
 * @param[in] prompt
 *   A pointer to a string that will be used as command prompt in the redirect
 *   function. NULL to stop the redirect.
 *
 * @param[in] aux
 *   A pointer that will be added to the redirect function arguments.
 *   NULL to stop the redirect.
 ******************************************************************************/
void sl_cli_redirect_command(sl_cli_handle_t handle, sl_cli_command_function_t command_function, const char *prompt, void *aux);

/***************************************************************************//**
 * @brief
 *  Handle input. Execute a complete command line with command and arguments.
 *
 * @param[in] handle
 *   A handle to the CLI.
 *
 * @param[in] string
 *   A pointer to the string containing the command line that shall be executed.
 *
 * @return
 *   Execution status.
 *   Note: If the command is redirected, the function will always return
 *   SL_STATUS_OK.
 ******************************************************************************/
sl_status_t sl_cli_handle_input(sl_cli_handle_t handle, char *string);

/***************************************************************************//**
 * @brief
 *  Initialize a CLI instance.
 *
 * @param[in] handle
 *   A handle to the CLI.
 *
 * @param[in] parameters
 *   A pointer to the structure containing instance parameters.
 *
 * @return
 *   Initialization status.
 ******************************************************************************/
sl_status_t sl_cli_instance_init(sl_cli_handle_t handle,
                                 sl_cli_instance_parameters_t *parameters);

#if !defined(SL_CATALOG_KERNEL_PRESENT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *  Check if the CLI instance can allow sleep.
 *  This function is available in a bare metal configuration only.
 *
 * @param[in] handle
 *   A handle to the CLI instance.
 *
 * @return
 *   A boolean that is true if the CLI allows sleep, else false.
 ******************************************************************************/
bool sl_cli_is_ok_to_sleep(sl_cli_handle_t handle);

/***************************************************************************//**
 * @brief
 *  The bare metal tick function.
 *
 * @param[in] handle
 *   A handle to the CLI instance.
 ******************************************************************************/
void sl_cli_tick_instance(sl_cli_handle_t handle);
#endif

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup cli) */

/* *INDENT-OFF* */
/* THE REST OF THE FILE IS DOCUMENTATION ONLY! */
/***************************************************************************//**
 * @addtogroup cli Command Line Interface
 * @details The Command Line Interface (CLI) software component simplifies
 * creating command line user interfaces for programs. It handles tasks such as
 * receiving keyboard input, parsing the input strings for commands and
 * arguments, and finally calling the proper C function with decoded and
 * validated arguments. Functions can use standard output functions such as
 * printf to write information to the console. Many modules of the SDK have
 * built-in commands that are provided via CLI, users can even add the
 * custom commands if required.
 *
 * The Command Line Interface (CLI) depends on @ref iostream
 * for reading and writing bytes. You can set the I/O stream instance in the CLI
 * instance configuration.
 *
 ******************************************************************************/
#endif // SL_CLI_H
