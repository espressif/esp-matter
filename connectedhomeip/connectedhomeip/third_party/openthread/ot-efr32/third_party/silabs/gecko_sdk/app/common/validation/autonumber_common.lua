-- automatic conversion of input parameters to number
local autonumber_common = {}

function autonumber_common.autonumber(input)
    logit("Autonumber function.")
    local base = 10
    local orig_input = input
    if (type(input) == "string") then
        input = input:gsub("[\(\)\"uUlL]", "")
        if string.find(input,"[bxhBXH]") ~= nil then
            if string.find(string.lower(input), "0b") == 1 then
                input = input:gsub("[bB]","")
                base = 2
            elseif string.find(string.lower(input), "0x") == 1 then
                input = input:gsub("[xXhH]","")
                base = 16
            end
        elseif string.find(input, "0") == 1 then
            base = 8
        end
    elseif (type(input) == "number") then
        return input
    else
        logit("autonumber() expects either a string or a number!")
        return nil
    end
    local result = tonumber(input, base)
    if result == nil then
        logit("Configured value is not valid: \"" .. tostring(orig_input) .. "\" - modify it to a numeric value!")
    end
    return result
end

return autonumber_common