-- Validation script for bootloader storage slots
	
local slot0_enable = slc.config('SLOT0_ENABLE')
local slot1_enable = slc.config('SLOT1_ENABLE')
local slot2_enable = slc.config('SLOT2_ENABLE')
	
local slot0_size = slc.config('SLOT0_SIZE')
local slot1_size = slc.config('SLOT1_SIZE')
local slot2_size = slc.config('SLOT2_SIZE')
	
local slot0_start_addr = slc.config('SLOT0_START')
local slot1_start_addr = slc.config('SLOT1_START')
local slot2_start_addr = slc.config('SLOT2_START')

-- Default flash page size is set to 2Kb. This variable is updated later based on the device family
local flash_page_size = 2048

slotStartAddress = {slot0_start_addr.value,slot1_start_addr.value,slot2_start_addr.value}
slotSizes = {slot0_size.value,slot1_size.value,slot2_size.value}
isSlotEnabled = {slot0_enable.value,slot1_enable.value,slot2_enable.value}

-- Set the correct flash page size based on the device family
-- Default flash page size is set to 2Kb which is the flash page size for device_series_1
if slc.is_provided("device_sdid_100") then
    flash_page_size = 4096
elseif slc.is_provided("device_series_2") then
    flash_page_size = 8192
end

if (not slc.is_selected("bootloader_spiflash_storage")) and (not slc.is_selected("bootloader_spiflash_storage_sfdp")) then
    -- Check is slots are aligned to flash page size
    for slotID = 1,3,1
    do
        -- Check alignment for all enabled slots
	 	if isSlotEnabled[slotID] == "1" then
	 	    if tonumber(slotStartAddress[slotID],10) % flash_page_size ~= 0 then
	 	        validation.warning('Slot '.. tostring(tonumber(slotID-1)) ..
	 	                " is not page aligned. Try aligning the slot start address to a " .. tostring(flash_page_size) .. "bytes boundary",
	 					validation.target_for_project())
	 		end
	 		if tonumber(slotSizes[slotID],10) % flash_page_size ~= 0 then
	 	        validation.warning('Slot size for slot '.. tostring(tonumber(slotID-1)) ..
	 	                " is not page aligned. Try aligning the slot size to a " .. tostring(flash_page_size) .. "bytes boundary",
	 					validation.target_for_project())
	 		end
	 	end
	end
end

-- Check for storage slot overlaps
-- No need to check for slot0 as there needs to be atleast 2 slots enabled to check for overlap

if (slot0_enable.value == "1") and (slot1_enable.value == "1") then
    -- slot 0 and slot 1 are enabled
	-- Conditions to detect potential overlaps b/w slot0 and slot1
	if tonumber(slot1_start_addr.value,10) == tonumber(slot0_start_addr.value,10) then
		validation.error('Overlap detected between slot0 and slot1', validation.target_for_project())
	end

	if (tonumber(slot1_start_addr.value,10) > (tonumber(slot0_start_addr.value,10))) and ((tonumber(slot0_start_addr.value,10) + tonumber(slot0_size.value,10)) > tonumber(slot1_start_addr.value,10)) then
		validation.error('Overlap detected between storage slot 0 and slot 1',validation.target_for_project())
	end

	if ((tonumber(slot0_start_addr.value,10)) > (tonumber(slot1_start_addr.value,10))) and ((tonumber(slot1_start_addr.value,10) + tonumber(slot1_size.value,10)) > tonumber(slot0_start_addr.value,10)) then
		validation.error('Overlap detected between storage slot 0 and slot 1',validation.target_for_project())
	end
end

if slot0_enable.value == "1" and slot1_enable.value == "1" and slot2_enable.value == "1" then
	-- slot 0,slot 1 and slot 2 are enabled
	-- Conditions to detect potential overlaps b/w slot1 and slot2
	if tonumber(slot1_start_addr.value,10) == tonumber(slot2_start_addr.value,10) then
		validation.error('Overlap detected between storage slot 1 and slot 2',validation.target_for_project())
    end

	if ((tonumber(slot2_start_addr.value,10)) > (tonumber(slot1_start_addr.value,10))) and ((tonumber(slot1_start_addr.value,10) + tonumber(slot1_size.value,10)) > tonumber(slot2_start_addr.value,10)) then
		validation.error('Overlap detected between storage slot 1 and slot 2',validation.target_for_project())
    end

	if ((tonumber(slot1_start_addr.value,10)) > (tonumber(slot2_start_addr.value,10))) and ((tonumber(slot2_start_addr.value,10) + tonumber(slot2_size.value,10)) > tonumber(slot1_start_addr.value,10)) then
		validation.error('Overlap detected between storage slot 1 and slot 2',validation.target_for_project())
    end

	-- Conditions to detect potential overlaps b/w slot0 and slot2
	if tonumber(slot0_start_addr.value,10) == tonumber(slot2_start_addr.value,10) then
		validation.error('Overlap detected between storage slot 0 and slot 2',validation.target_for_project())
    end

	if ((tonumber(slot2_start_addr.value,10)) > (tonumber(slot0_start_addr.value,10))) and ((tonumber(slot0_start_addr.value,10) + tonumber(slot0_size.value,10)) > tonumber(slot2_start_addr.value,10)) then
		validation.error('Overlap detected between storage slot 0 and slot 2',validation.target_for_project())
    end

	if ((tonumber(slot0_start_addr.value,10)) > (tonumber(slot2_start_addr.value,10))) and ((tonumber(slot2_start_addr.value,10) + tonumber(slot2_size.value,10)) > tonumber(slot0_start_addr.value,10)) then
		validation.error('Overlap detected between storage slot 0 and slot 2',validation.target_for_project())
    end
end