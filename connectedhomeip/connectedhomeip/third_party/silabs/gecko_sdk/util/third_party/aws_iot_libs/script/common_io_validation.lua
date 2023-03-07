local lib = {}

function lib.validate_instance_numbers(iot_module)

  local module = slc.component(iot_module)
  local configs = {}
  local previous_config = nil

  for k, v in pairs(module.instances) do 
    local instance = string.upper(k)
    local config_prefix = string.upper(iot_module)
    config_prefix = config_prefix .. "_CFG_"
    config_prefix = config_prefix .. instance 
    local config_inst_num = config_prefix .. "_INST_NUM"
    local config = slc.config(config_inst_num)
    configs[config_inst_num] = config  
  end

  table.sort(configs, function(a,b) return a < b end)

  for config, val in pairs(configs) do 
    local config_value = tonumber(val.value)
    if (config_value < 0) then
        local msg = string.upper(iot_module) .. " : Instance numbers cannot be negative"
        validation.error(msg,
                        validation.target_for_defines({config}),
                        "Instance numbers should be sequential, like 0, 1, 2, ...",
                        nil)
    end

    if previous_config ~= nil then
      local next_expected_value = previous_config + 1
      if previous_config == config_value then
        local msg = string.upper(iot_module) .. " : There are at least 2 instance configured with the same instance number"
        validation.error(msg,
                        validation.target_for_defines({config}),
                        "Instance numbers should be sequential, like 0, 1, 2, ...",
                        nil)
      elseif next_expected_value ~= config_value then
        local msg = string.upper(iot_module) .. " : Module has instances with numbers "
        msg = msg .. previous_config
        msg = msg .. " and "
        msg = msg .. config_value
        msg = msg .. ", yet there are no instances between them"
        validation.error(msg,
                        validation.target_for_defines({config}),
                        "Instance numbers should be sequential, like 0, 1, 2, ...",
                        nil)
      end
    elseif config_value ~= 0 then
        local msg = string.upper(iot_module) .. " : There is no instance that is configured with INST_NUM of 0"
        validation.error(msg,
                        validation.target_for_defines({config}),
                        "Instance numbers should be sequential, like 0, 1, 2, ...",
                        nil)
    end

    previous_config = config_value

  end
end

return lib