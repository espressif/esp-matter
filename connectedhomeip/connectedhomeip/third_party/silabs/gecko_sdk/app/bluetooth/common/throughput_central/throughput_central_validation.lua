-- throughput central validation script for checking MAC adress formats in allowlist
local slots = { 
    {
        name = "Slot 1",
        enable = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_1_ENABLE",
        slot = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_1"
    },
    {
        name = "Slot 2",
        enable = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_2_ENABLE",
        slot = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_2"
    },
    {
        name = "Slot 3",
        enable = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_3_ENABLE",
        slot = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_3"
    },
    {
        name = "Slot 4",
        enable = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_4_ENABLE",
        slot = "THROUGHPUT_CENTRAL_ALLOWLIST_SLOT_4"
    }
}

local mac_pattern = "(%x%x:%x%x:%x%x:%x%x:%x%x:%x%x)"

function check_mac(address)
    local maclist = string.match(address,mac_pattern)
    if maclist ~= nil then
        if maclist == address then
            return true
        end
    end
    return false
end

local wl_enabled = autonumber_common.autonumber(slc.config("THROUGHPUT_CENTRAL_ALLOWLIST_ENABLE").value)

if wl_enabled ~= nil and wl_enabled == 1 then
    for k,v in pairs(slots) do
        local slot_enabled = autonumber_common.autonumber(slc.config(v.enable).value)
        if slot_enabled ~= nil and slot_enabled == 1 then
                local slot = slc.config(v.slot)
                local slot_formatted = slot.value:gsub("\""," "):gsub("%s","")
            if slot ~= nil then
                local result = check_mac(slot_formatted)
                if not result then
                    validation.error("MAC address " .. slot_formatted .. " for " .. v.name .. " is not valid! ",
                    validation.target_for_defines({v.slot}),
                    "MAC address shall be in FF:FF:FF:FF:FF:FF hexadecimal format.",
                    nil)
                end
            end
        end
    end
end
