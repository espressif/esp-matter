-- This validation script checks following:
-- The Slot Manager plugin must be selected in order for slots to be used.
local slot_manager_enabled = slc.is_selected("slot_manager")
local gecko_bootloader_storage_support_val =
    slc.config("EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_GECKO_BOOTLOADER_STORAGE_SUPPORT")
if (gecko_bootloader_storage_support_val ~= nil and (gecko_bootloader_storage_support_val.value ~= "DO_NOT_USE_SLOTS") and (not slot_manager_enabled)) then
    validation.error(
        "Using Gecko Bootloader storage support requires the Slot Manager plugin",
        validation.target_for_defines({"EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_GECKO_BOOTLOADER_STORAGE_SUPPORT"}),
        nil,
        nil
    )
end
