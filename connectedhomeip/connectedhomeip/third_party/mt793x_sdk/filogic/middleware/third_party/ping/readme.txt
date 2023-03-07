Ping module usage guide

Brief:          This module is the implementation of ping protocol.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/third_party/ping/module.mk" in your GCC project Makefile and set MTK_PING_OUT_ENABLE to "y".
                KEIL:Drag the middleware/third_party/ping folder to your project and define the MTK_PING_OUT_ENABLE macro. Add middleware/third_party/ping/inc to include paths.
                IAR: Drag the middleware/third_party/ping folder to your project and define the MTK_PING_OUT_ENABLE macro. Add middleware/third_party/ping/inc to include paths.
Dependency:     MTK_MINICLI_ENABLE must be defined in feature.mk and include lwip module in your project.
Notice:         N/A.
Relative doc:   N/A.
Example project:Please find the iot_sdk_demo project under project/mt7687_hdk/apps folder. To use Ping, please submit "ping < ip_address>" in cli command line to use ping.
                The relative sources of ping cli implementation are in middleware/third_party/ping/src/ping_cli.c.