lzma_decoder module usage guide

Brief:          This module is migrated from lzma 3rd party open source. We also provide some wrapper functions based on the original lzma decode interface for FOTA feature in bootloader.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/third_party/lzma_decoder/module.mk" in your GCC project Makefile, or include library file directly under lib folder.
                KEIL: Drag the middleware/third_party/lzma_decoder folder to your project. Add middleware/third_party/lzma_decoder/inc to INCLUDE_PATH.
                IAR: Drag the middleware/third_party/lzma_decoder folder to your project. Add middleware/third_party/lzma_decoder/inc to "additional include directories" in IAR options setting.
Dependency:     None. 
Notice:         The lzma wrappered functions are used to decode FOTA upgrade file in bootloader.
Relative doc:   Please refer to Internet and Open Source Software Guide under the doc folder for more detail.
Example project:Please find fota related sources under driver/board/mt25x3_hdk/bootloader/core/src/bl_fota.c for detail. 
