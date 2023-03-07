
-- retrieve instance lists
local mod_config_insts  = slc.component("usb_device_configuration").instances
local mod_msc_scsi_insts = slc.component("usb_device_class_msc_scsi").instances

-- retrieve configurations that we need to check
local cfg_config_qty = slc.config("SL_USBD_MSC_CONFIGURATION_QUANTITY")
local cfg_class_qty  = slc.config("SL_USBD_MSC_CLASS_INSTANCE_QUANTITY")
local cfg_lun_qty    = slc.config("SL_USBD_MSC_LUN_QUANTITY")

-- check cfg_config_qty configuration
if (tonumber(cfg_config_qty.value) < #mod_config_insts) then
  validation.error("Value is smaller than the number of configuration instances",
                   validation.target_for_defines({cfg_config_qty.id}),
                   "The value of this config should be greater than or equal number of instances.",
                   nil)
end

-- check cfg_class_qty configuration
if (tonumber(cfg_class_qty.value) < #mod_msc_scsi_insts) then
  validation.error("Value is smaller than the number of MSC SCSI instances",
                   validation.target_for_defines({cfg_class_qty.id}),
                   "The value of this config should be greater than or equal number of instances.",
                   nil)
end

-- check cfg_subclass_qty configuration
if (tonumber(cfg_lun_qty.value) < #mod_msc_scsi_insts) then
  validation.error("Value is smaller than the number of MSC SCSI instances",
                   validation.target_for_defines({cfg_lun_qty.id}),
                   "The value of this config should be greater than or equal number of instances.",
                   nil)
end
