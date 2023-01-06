# This script should be sourced, not executed.

realpath_int() {
    wdir="$PWD"; [ "$PWD" = "/" ] && wdir=""
    arg=$1
    case "$arg" in
        /*) scriptdir="${arg}";;
        *) scriptdir="$wdir/${arg#./}";;
    esac
    scriptdir="${scriptdir%/*}"
    echo "$scriptdir"
}


esp_matter_export_main() {
    if [ -z "${ESP_MATTER_PATH}" ]
    then
        # ESP_MATTER_PATH not set in the environment.
        # If using bash or zsh, try to guess ESP_MATTER_PATH from script location.
        self_path=""

        # shellcheck disable=SC2128  # ignore array expansion warning
        if [ -n "${BASH_SOURCE-}" ]
        then
            self_path="${BASH_SOURCE}"
        elif [ -n "${ZSH_VERSION-}" ]
        then
            self_path="${(%):-%x}"
        else
            echo "Could not detect ESP_MATTER_PATH. Please set it before sourcing this script:"
            echo "  export ESP_MATTER_PATH=(add path here)"
            return 1
        fi

        # shellcheck disable=SC2169,SC2169,SC2039  # unreachable with 'dash'
        if [[ "$OSTYPE" == "darwin"* ]]; then
            # convert possibly relative path to absolute
            script_dir="$(realpath_int "${self_path}")"
            # resolve any ../ references to make the path shorter
            script_dir="$(cd "${script_dir}" || exit 1; pwd)"
        else
            # convert to full path and get the directory name of that
            script_name="$(readlink -f "${self_path}")"
            script_dir="$(dirname "${script_name}")"
        fi
        export ESP_MATTER_PATH="${script_dir}"
        echo "Setting ESP_MATTER_PATH to '${ESP_MATTER_PATH}'"
    fi

    # PATH for gn
    export PATH=${PATH}:${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/.environment/cipd/packages/pigweed/

    # PATH for host tools
    export PATH=${PATH}:${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host
}

esp_matter_export_main
