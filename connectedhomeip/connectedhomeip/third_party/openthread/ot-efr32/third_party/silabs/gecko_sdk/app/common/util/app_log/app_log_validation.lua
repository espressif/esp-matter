component_table = {
    SL_IOSTREAM_TYPE_SWO = 'iostream_swo',
    SL_IOSTREAM_TYPE_RTT = 'iostream_rtt',
    SL_IOSTREAM_TYPE_UART = 'iostream_usart',
    SL_IOSTREAM_TYPE_VUART = 'iostream_vuart',
}

-- check if the choosen key is part of a set or not (no "in" operator in lua)
function table_contains(set, key)
    if (type(set) == "table") then 
        return set[key] ~= nil
    end
    return false
end

-- converts a table content (keys or values) to a string
function table_to_string(t, use_vals)
    local result_str = ""
    local cnt = 1
    for key,value in pairs(t) do
        cnt = cnt + 1
        local snippet
        if use_vals == true then
            snippet = value
        else
            snippet = key
        end
        if cnt > 2 then
            result_str = result_str .. ", " .. snippet
        else
            result_str = result_str .. snippet
        end
    end
    return result_str
end


-- checks a stream-type if its a known type and the selected components type or not
function check_type(project, stream_type)
    if table_contains(component_table, stream_type) == true then
        local component_name = component_table[stream_type]
        if component_name ~= nil then
            return project.is_selected(component_name)
        end
    end
    return false
end

-- gets the selected component based on the stream-type
function get_selected_component(project, stream_type)
    if check_type(project, stream_type) == true then
        return project.component(component_table[stream_type])
    end
    return nil
end

-- get selected component's stream_types in a table
function get_types(project)
    local result_types = {}
    for stream_type, component_name in pairs(component_table) do
        if component_name ~= nil then
            local c = project.is_selected(component_name)
            if c == true then
                table.insert(result_types, stream_type)
            end
        end
    end
    return result_types
end

-- check stream_typed selected component if its instantiable
function check_type_instance(project, stream_type)
    if check_type(project, stream_type) == true then
        local c = get_selected_component(project, stream_type)
        return c.is_instantiable
    end
    return false
end

-- get selected component's instance names
function get_names(project, stream_type)
    local instances
    if check_type(project, stream_type) == true then
        local c = get_selected_component(project, stream_type)
        instances = c.instances
    end
    return instances
end

-- checks wether the selected components instance name is valid
function check_name(project, stream_type, stream_name)
    local names_table = get_names(project, stream_type)
    if names_table ~= nil then
        if table_contains(names_table, stream_name) then
            return true
        end
    end
    return false
end

-- app_log validation script for checking IO stream validity.
local override_enabled = autonumber_common.autonumber(slc.config('APP_LOG_OVERRIDE_DEFAULT_STREAM').value)
if override_enabled ~= nil and override_enabled == 1 then
    local stream_type = slc.config('APP_LOG_STREAM_TYPE')
    if stream_type ~= nil then
        local selected_type = check_type(slc, stream_type.value)
        local type_has_instance = check_type_instance(slc, stream_type.value)
        local selected_name = false
        stream_name = slc.config('APP_LOG_STREAM_INSTANCE')
        local stream_name_formatted
        if stream_name ~= nil then
            stream_name_formatted = stream_name.value:gsub("\"","")
            selected_name = check_name(slc, stream_type.value, stream_name_formatted)
        end
        local component_label = slc.component(component_table[stream_type.value]).label
        if selected_type == false then
            local types = get_types(slc)
            validation.error("IO Stream is not found by type!",
                            validation.target_for_defines({'APP_LOG_STREAM_TYPE'}),
                            "Selected type " .. stream_type.value:gsub("SL_IOSTREAM_TYPE_", "") ..  " is not present. Add the \'" .. component_label .. "\' (" .. component_table[stream_type.value] .. ")" .. 
                            " component to the project or choose a type from installed ones: [" .. table_to_string(types,true):gsub("SL_IOSTREAM_TYPE_", "") ..  "]",
                            validation.quickfix(types))
        elseif type_has_instance == true and selected_name == false then
            local names = get_names(slc, stream_type.value)
            validation.error("IO Stream is not found by instance name!",
                            validation.target_for_defines({'APP_LOG_STREAM_INSTANCE'}),
                            "Selected instance with type \'" .. stream_type.value:gsub("SL_IOSTREAM_TYPE_", "") ..  "\' and name \'"
                            .. stream_name_formatted ..  "\' is not present. Add \'" .. stream_name_formatted .. 
                            "\' instance of \'" .. component_label .. "\' component to the project or choose from the available instances: [" .. table_to_string(names, false) .. "]",
                            validation.quickfix(names))
        end
    end
end