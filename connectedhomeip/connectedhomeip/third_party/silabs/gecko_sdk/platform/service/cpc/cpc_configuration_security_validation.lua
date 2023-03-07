local config_security = slc.config("SL_CPC_SECURITY_ENABLED")
if config_security.value == "1" then
  local config_binding_key = slc.config("SL_CPC_SECURITY_BINDING_KEY_METHOD")
  if config_binding_key.value == "SL_CPC_SECURITY_BINDING_KEY_NONE" then
    validation.error(
    "CPC encryption is enabled but binding method is selected as none",
    validation.target_for_defines({"SL_CPC_SECURITY_BINDING_KEY_METHOD"}),
    nil,
    nil)
  end
end