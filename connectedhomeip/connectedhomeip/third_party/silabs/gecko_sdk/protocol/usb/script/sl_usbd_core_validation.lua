-- retrieve configurations that we need to check
local cfg_interface_qty = slc.config("SL_USBD_INTERFACE_QUANTITY")
local cfg_alt_interface_qty  = slc.config("SL_USBD_ALT_INTERFACE_QUANTITY")

-- check cfg_alt_interface_qty configuration
if (tonumber(cfg_alt_interface_qty.value) < tonumber(cfg_interface_qty.value)) then
  validation.error("Value is smaller than the number of interfaces",
                   validation.target_for_defines({"SL_USBD_ALT_INTERFACE_QUANTITY"}),
                   "The value of this config should be greater than or equal to SL_USBD_INTERFACE_QUANTITY.",
                   nil)
end

