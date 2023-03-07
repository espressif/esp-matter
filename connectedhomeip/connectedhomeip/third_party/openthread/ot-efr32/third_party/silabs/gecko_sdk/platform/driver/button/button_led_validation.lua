--[[
    This script validates the conflict for port:pin when using the simple_led
    and simple_button component in a zigbee application
    This script iterates through all the instances of simple_led and
    simple_button, compares the port/pin for the instances.
--]]

if slc.is_selected("simple_led") == true and slc.is_selected("simple_button") == true then
    local leds = slc.component("simple_led")
    local buttons = slc.component("simple_button")
    local allow_button_conflicts = 0

    -- If there is a SL_SIMPLE_BUTTON_ALLOW_LED_CONFLICT item then parse it
    if slc.config("SL_SIMPLE_BUTTON_ALLOW_LED_CONFLICT") ~= nil then
        -- Remove any U/L characters to handle 0U or 0UL type defines
        value = slc.config("SL_SIMPLE_BUTTON_ALLOW_LED_CONFLICT").value
        value = string.gsub(value, "U", "")
        value = string.gsub(value, "L", "")
        -- Convert to a number so that we can handle hex values
        allow_button_conflicts = tonumber(value)
    end
    if allow_button_conflicts == 0 then
        for led, v in pairs(leds.instances) do
            local led_port = slc.config("SL_SIMPLE_LED_"..string.upper(led).."_PORT")
            local led_pin = slc.config("SL_SIMPLE_LED_"..string.upper(led).."_PIN")
            if led_port ~= nil and led_pin ~= nil then
                for button, val in pairs(buttons.instances) do
                    local button_port = slc.config("SL_SIMPLE_BUTTON_"..string.upper(button).."_PORT")
                    local button_pin = slc.config("SL_SIMPLE_BUTTON_"..string.upper(button).."_PIN")
                    if button_port ~= nil and button_pin ~= nil then
                        if led_port.value == button_port.value and led_pin.value == button_pin.value then
                            validation.error(
                                "Button "..button.." and LED "..led.." port and pin conflict",
                                validation.target_for_defines({"SL_SIMPLE_BUTTON_"..string.upper(button).."_PORT",
                                                            "SL_SIMPLE_BUTTON_"..string.upper(button).."_PIN",
                                                            "SL_SIMPLE_LED_"..string.upper(led).."_PORT",
                                                            "SL_SIMPLE_LED_"..string.upper(led).."_PIN"}),
                                "The Button and LED drivers are not designed to handle pin conflicts. Either choose separate pins for each button and LED or set SL_SIMPLE_BUTTON_ALLOW_LED_CONFLICT to 1 to indicate that the application will handle this.",
                                nil
                            )
                        end
                    end
                end
            end
        end
    end
end