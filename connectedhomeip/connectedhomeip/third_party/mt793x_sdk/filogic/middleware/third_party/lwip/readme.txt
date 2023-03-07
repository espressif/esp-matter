LWIP module usage guide

Brief:          This module is the implementation of TCP/IP stack.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/third_party/lwip/module.mk" in your GCC project Makefile.
                KEIL:Drag the middleware/third_party/lwip folder to your project. Add middleware/third_party/lwip/src/include and middleware/third_party/lwip/ports/include to include paths.
                IAR: Drag the middleware/third_party/lwip folder to your project. Add middleware/third_party/lwip/src/include and middleware/third_party/lwip/ports/include to include paths.
Dependency:     For mt76x7, Please enable wifi module. For mt2523x, please enable modem module.
Relative doc:   Please refer to the Open source user guide under the doc folder for more detail.
Example project:Please find the project under project folder with lwip_ prefix.
Notice:         Configuration file needed, should be "lwipopts.h" in the project-wise include path,
                e.g., project/mt76x7_hdk/apps/< project>/inc/lwipopts.h.
                To enable debug log of LWIP module, please add "# define LWIP_DEBUG LWIP_DBG_ON" in
                project/mt76x7_hdk/apps/< project>/inc/lwipopts.h, after that, please redefine the
                log module in LWIP to LWIP_DBG_ON, e.g., "# define IP_DEBUG LWIP_DBG_ON".
                The default log type of LWIP is defined in lwip_log.c as ERROR type. You can modify
                the sources of lwip_log.c to other debug type, such as "log_create_module(lwip, PRINT_LEVEL_INFO)".
