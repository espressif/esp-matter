MBEDTLS module usage guide

Brief:          This module is an implementation of websocket client.
Usage:          GCC: In your GCC project Makefile, please add the following:
                     include $(SOURCE_DIR)/middleware/third_party/websocket/module.mk
                     To support secure websocket(TLS), please add mbedtls module with config-mtk-websocket.h as the configuration file,
                     and in your GCC project mk file, such as feature.mk, please add "MTK_WEBSOCKET_SSL_ENABLE = y".
                KEIL: Drag the middleware/third_party/websocket/src folder to your project. Add the following to include paths:
                      middleware/third_party/websocket
                      middleware/third_party/websocket/src
                      To support secure websocket(TLS), please add mbedtls module with config-mtk-websocket.h as the configuration file,
                      and define MTK_WEBSOCKET_SSL_ENABLE macro.
                IAR: Drag the middleware/third_party/websocket/src folder to your project. Add the following to include paths:
                     middleware/third_party/websocket
                     middleware/third_party/websocket/src
                     To support secure websocket(TLS), please add mbedtls module with config-mtk-websocket.h as the configuration file,
                     and define MTK_WEBSOCKET_SSL_ENABLE macro.
Dependency:     Please also include LWIP since this module uses it.
                To support secure websocket(TLS), please include mbedtls module as mentioned above.
Notice:         None.
Relative doc:   Please refer to the open source user guide under the doc folder for more detail.
Example project:Please find websocket_client project under project folder.



