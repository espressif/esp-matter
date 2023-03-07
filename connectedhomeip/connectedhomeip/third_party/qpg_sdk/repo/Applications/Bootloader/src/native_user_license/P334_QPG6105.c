const userlicense_t FLASH_PROGMEM user_license LINKER_SECTION(".native_user_license") =
{
    .vpp = (UInt32)&_native_user_license_vpp,

    .programLoadedMagicWord = USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD,

#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
#if defined(GP_DIVERSITY_SECURE_BOOTLOADER_PRODUCTION)
    #error BULK ERASE AND DEBUG LOCK ENABLED - Remove this error if you are sure that you want to lock the device
    // Block programming via a JTAG/SWD debugger (in programmer mode)
    // Note: This does not block debugging via "attach to running target"
    .debugLockMagicWord = USER_LICENSE_DEBUG_LOCK_MAGIC_WORD,
    // Block bulk erase (in programmer mode)
    .bulkEraseMagicWord = USER_LICENSE_BULK_ERASE_LOCK_MAGIC_WORD,
#endif //GP_DIVERSITY_SECURE_BOOTLOADER_PRODUCTION

    // write locks in programmer mode:
    // - Make bootloader unaccessible
    // - Keep jumptables + application overwritable
    .sectorWriteLockBitmapFine_0 =
        GPHAL_FLASH_WRITE_LOCK_REGION_0_2K |            // 0x04000000 - 0x040007FF: Lock native user license
        /* GPHAL_FLASH_WRITE_LOCK_REGION_2_4K   | */    // 0x04000800 - 0x04000FFF: Keep jumptable 4K area (2K-6K) unlocked
        /* GPHAL_FLASH_WRITE_LOCK_REGION_4_6K   | */    // 0x04001000 - 0x040017FF: Keep jumptable 4K area (2K-6K) unlocked
        GPHAL_FLASH_WRITE_LOCK_REGION_6_8K |            // 0x04001800 - 0x04001FFF:
        GPHAL_FLASH_WRITE_LOCK_REGION_8_10K |           // 0x04002000 - 0x040027FF:
        GPHAL_FLASH_WRITE_LOCK_REGION_10_12K |          // 0x04002800 - 0x04002FFF:
        GPHAL_FLASH_WRITE_LOCK_REGION_12_14K |          // 0x04003000 - 0x040037FF:
        GPHAL_FLASH_WRITE_LOCK_REGION_14_16K |          // 0x04003800 - 0x04003FFF:
        GPHAL_FLASH_WRITE_LOCK_REGION_16_18K |          // 0x04004000 - 0x040047FF:
        GPHAL_FLASH_WRITE_LOCK_REGION_18_20K |          // 0x04004800 - 0x04004FFF:
        GPHAL_FLASH_WRITE_LOCK_REGION_20_22K,           // 0x04005000 - 0x040057FF:
        /* GPHAL_FLASH_WRITE_LOCK_REGION_22_24K | */    // 0x04005800 - 0x04005FFF: Keep RMA token area unlocked
        // GPHAL_FLASH_WRITE_LOCK_REGION_24_26K |       // 0x04006000 - 0x040067FF
        // GPHAL_FLASH_WRITE_LOCK_REGION_26_28K |       // 0x04006800 - 0x04006FFF
        // GPHAL_FLASH_WRITE_LOCK_REGION_28_30K |       // 0x04007000 - 0x040077FF
        // GPHAL_FLASH_WRITE_LOCK_REGION_30_32K |       // 0x04007800 - 0x04007FFF
        // GPHAL_FLASH_WRITE_LOCK_REGION_32_64K |       // 0x04008000 - 0x0400FFFF
        // GPHAL_FLASH_WRITE_LOCK_REGION_64_128K |      // 0x04010000 - 0x0401FFFF
        // GPHAL_FLASH_WRITE_LOCK_REGION_128_256K |     // 0x04020000 - 0x0403FFFF

#endif // GP_APP_DIVERSITY_SECURE_BOOTLOADER
#if defined(GP_COMP_VERSION)
    .swVersion = {{GP_VERSIONINFO_GLOBAL_VERSION}, 0x00, 0, 0, ((0x00FFFFFFUL & GP_CHANGELIST) << 8)},
#endif

};

