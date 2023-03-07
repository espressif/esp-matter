/**
 * @file cmd_handlers.h
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef CMD_HANDLER_H_
#define CMD_HANDLER_H_

#include <stdint.h>
#include <comm_interface.h>

#ifdef ZW_CONTROLLER
#include <ZW_controller_api.h>
#endif

typedef void (*cmd_handler_t)(const comm_interface_frame_ptr);

typedef struct
{
  uint8_t cmd;
  cmd_handler_t pHandler;
}
cmd_handler_map_t;

#define CMD_HANDLER_SECTION "_cmd_handlers"

#define ZW_ADD_CMD(cmd) \
  static void cmd_handler_fcn_##cmd(const comm_interface_frame_ptr frame); /* Prototype */ \
  static const cmd_handler_map_t cmd_handler_##cmd __attribute__((__used__, __section__( CMD_HANDLER_SECTION ))) = {cmd,cmd_handler_fcn_##cmd}; \
  static void cmd_handler_fcn_##cmd(const comm_interface_frame_ptr frame)

/**
 * Invoke command handler.
 * 
 * @param[in] frame Frame
 * @return true if handler for given @p frame was invoked, false if no handler was found
 */
bool invoke_cmd_handler(const comm_interface_frame_ptr frame);

typedef void * cmd_context_t;

typedef bool (*cmd_foreach_callback_t)(cmd_handler_map_t const * const p_cmd_entry, cmd_context_t context);

/**
 * Invokes callback for each registered command.
 *
 * Will stop if the callback returns true.
 *
 * @param callback Callback function to invoke.
 * @param context Context to pass on to the callback function.
 */
void cmd_foreach(cmd_foreach_callback_t callback, cmd_context_t context);

#ifdef ZW_CONTROLLER
void ZCB_ComplHandler_ZW_NodeManagement(LEARN_INFO_T *statusInfo);
#endif

#endif /* CMD_HANDLER_H_ */
