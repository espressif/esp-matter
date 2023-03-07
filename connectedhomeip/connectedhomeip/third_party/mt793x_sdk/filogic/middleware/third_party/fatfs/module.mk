
###################################################
# Sources
FATFS_SRC = middleware/third_party/fatfs

C_FILES  += $(FATFS_SRC)/source/ff.c
C_FILES  += $(FATFS_SRC)/source/ff_cli.c
C_FILES  += $(FATFS_SRC)/source/ffunicode.c
C_FILES  += $(FATFS_SRC)/source/ffsystem.c
CFLAGS += -fwide-exec-charset=UTF-16LE


ifeq ($(PRODUCT_VERSION),7933)
C_FILES += $(FATFS_SRC)/source/portable/mt7933/src/diskio.c
C_FILES += $(FATFS_SRC)/source/portable/mt7933/src/diskio_sd.c
C_FILES += $(FATFS_SRC)/source/portable/mt7933/src/diskio_usb.c
endif



#################################################################################
# include path

CFLAGS 	+= -I$(SOURCE_DIR)/middleware/util/include
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/$(FATFS_SRC)/source

ifeq ($(PRODUCT_VERSION),7933)
CFLAGS  += -I$(SOURCE_DIR)/$(FATFS_SRC)/source/portable/mt7933/inc
endif

