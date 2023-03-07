--Validation script for Bootloader Image Parser with no encryption component

local has_parser_noenc = slc.is_selected("bootloader_image_parser_nonenc")
local has_bl_interface = slc.is_selected("bootloader_interface")

local btl_enforce_signed_upgrade = slc.config('BOOTLOADER_ENFORCE_SIGNED_UPGRADE')
local btl_enforce_encryption = slc.config('BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE')

if btl_enforce_encryption.value == "1" and has_parser_noenc then
    validation.error('Can not use parser without encryption support, since the bootloader is configured to enforce encrypted upgrade files',
	validation.target_for_defines({'BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE'}))
end

if has_bl_interface then
    validation.error('Cannot install Bootloader Application Interface component in a Gecko Bootloader project. Please un-install the Bootloader Application Interface component to avoid build errors',
	validation.target_for_project())
end
