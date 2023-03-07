-- Proxy validation script

local proxy_server_present = slc.is_selected('btmesh_feature_proxy_client')
local proxy_client_present = slc.is_selected('btmesh_feature_proxy_server')
local proxy_present = slc.is_selected('btmesh_feature_proxy')

if proxy_present and not ( proxy_client_present or proxy_server_present) then
    validation.warning(
        'If you add proxy component then either proxy server or proxy client will also be needed',
        validation.target_for_project(),
        'Add either proxy_server or proxy_client component)'
    )
end
