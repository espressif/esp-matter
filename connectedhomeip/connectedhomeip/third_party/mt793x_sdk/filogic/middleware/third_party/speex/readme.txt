SPEEX module usage guide

Brief:          This module is the file system implementation speex codec operation.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/third_party/speex/module.mk" in your GCC project Makefile, add "MTK_AUDIO_SPEEX_ENABLED = y",
                "MTK_I2S_ENABLED = y" and "MTK_FATFS_ON_SPI_SD = y" in your GCC project feature.mk if SD is implemented based on SPI.
                KEIL/IAR: Drag the C files on middleware/third_party/speex/src and middleware/third_party/speex/libspeex folder to your to your project. According to current 
                configuration to your project and add middleware/third_party/speex, middleware/third_party/speex/src, middleware/third_party/speex/libspeex, middleware/third_party/speex/include
                and middleware/third_party/speex/include/ogg to include paths, add "MTK_AUDIO_SPEEX_ENABLED = y",
                "MTK_I2S_ENABLED = y" and "MTK_FATFS_ON_SPI_SD = y" in your GCC project feature.mk if SD is implemented based on SPI.
Dependency:     Please define HAL_SD_MODULE_ENABLED and HAL_I2S_MODULE_ENABLED in hal_feature_config.h under the inc folder of your project.
Notice:         
Relative doc:   Please refer to the open source user guide under the doc folder for more detail.
Example project:Please find the audio_ref_design project under project folder with speex_ prefix
