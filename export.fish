# This script should be sourced, not executed.

function esp_matter_export_main 
    if [ -z "$ESP_MATTER_PATH" ]
        # ESP_MATTER_PATH not set in the environment.
        # If using bash or zsh, try to guess ESP_MATTER_PATH from script location.
        set self_path (status --current-filename)
        status --current-filename

        set script_name (readlink -f "$self_path")
        set script_dir (dirname "$script_name")

        set -gx ESP_MATTER_PATH "$script_dir"
        echo " * Setting ESP_MATTER_PATH to '$ESP_MATTER_PATH'"
    else
        echo " * ESP_MATTER_PATH already set to '$ESP_MATTER_PATH'"
    end

    # PATH for gn
    echo " * Adding to path: '$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/pigweed/'"
    fish_add_path -g "$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/pigweed/"

    # PATH for host tools
    echo " * Adding to path: '$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/out/host'"
    fish_add_path -g "$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/out/host"

    # export zap-cli path
    echo " * Setting ZAP_INSTALL_PATH to '$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/zap'"
    set -gx ZAP_INSTALL_PATH "$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/zap"

    echo
    echo "Done! You can now compile ESP_MATTER projects."
end

echo "Setting Up ESP-MATTER"

esp_matter_export_main
