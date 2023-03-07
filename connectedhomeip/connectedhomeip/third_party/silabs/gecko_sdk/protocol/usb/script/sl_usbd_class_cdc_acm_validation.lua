
-- retrieve instance lists
local mod_config_insts  = slc.component("usb_device_configuration").instances
local mod_cdc_acm_insts = slc.component("usb_device_class_cdc_acm").instances

-- retrieve configurations that we need to check
local cfg_config_qty   = slc.config("SL_USBD_CDC_CONFIGURATION_QUANTITY")
local cfg_class_qty    = slc.config("SL_USBD_CDC_CLASS_INSTANCE_QUANTITY")
local cfg_subclass_qty = slc.config("SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY")
local cfg_dci_qty      = slc.config("SL_USBD_CDC_DATA_INTERFACE_QUANTITY")

-- check cfg_config_qty configuration
if (tonumber(cfg_config_qty.value) < #mod_config_insts) then
  validation.error("Value is smaller than the number of configuration instances",
                   validation.target_for_defines({cfg_config_qty.id}),
                   "The value of this config should be greater than or equal number of instances.",
                   nil)
end

-- check cfg_class_qty configuration
if (tonumber(cfg_class_qty.value) < #mod_cdc_acm_insts) then
  validation.error("Value is smaller than the number of CDC ACM instances",
                   validation.target_for_defines({cfg_class_qty.id}),
                   "The value of this config should be greater than or equal number of instances.",
                   nil)
end

-- check cfg_subclass_qty configuration
if (tonumber(cfg_subclass_qty.value) < #mod_cdc_acm_insts) then
  validation.error("Value is smaller than the number of CDC ACM instances",
                   validation.target_for_defines({cfg_subclass_qty.id}),
                   "The value of this config should be greater than or equal number of instances.",
                   nil)
end

-- check cfg_dci_qty configuration
if (tonumber(cfg_dci_qty.value) < #mod_cdc_acm_insts) then
  validation.error("Value is smaller than the number of CDC ACM instances",
                   validation.target_for_defines({cfg_dci_qty.id}),
                   "The value of this config should be greater than or equal number of instances.",
                   nil)
end
