-- Validation script for Bootloader when both SPI Flash Storage and SPI Flash Storage SFDP are selected
	
local spiflash = slc.is_selected("bootloader_spiflash_storage")
local spiflash_sfdp = slc.is_selected("bootloader_spiflash_storage_sfdp")

if spiflash and spiflash_sfdp then
	validation.error('Cannot install SPI Flash Storage and SPI Flash Storage SFDP components simultaneously',
	validation.target_for_project(),
	'Please un-install either one of these components to fix this error')
end