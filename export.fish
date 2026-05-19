# This script should be sourced, not executed.

function realpath_int --description "Simplified realpath helper" --argument-names arg
    if test -z "$arg"
        return 1
    end

    set -l wdir $PWD
    if test "$PWD" = "/"
        set wdir ""
    end

    if string match -rq '^~' -- $arg
        set arg (string replace -r '^~' $HOME -- $arg)
    end

    set -l scriptdir ""
    switch $arg
        case '/*'
            set scriptdir $arg
        case '*'
            set -l stripped (string replace -r '^\./' '' -- $arg)
            set scriptdir "$wdir/$stripped"
    end

    set scriptdir (string replace -r '/[^/]*$' '' -- $scriptdir)
    if test -z "$scriptdir"
        set scriptdir "/"
    end
    echo $scriptdir
end

function esp_matter_export_main --description "Initialize ESP Matter environment variables for fish"
    if not set -q ESP_MATTER_PATH
        set -l self_path (status --current-filename)

        if test -z "$self_path"
            echo "Could not detect ESP_MATTER_PATH. Please set it before sourcing this script:" 1>&2
            echo "  set -gx ESP_MATTER_PATH <add path here>" 1>&2
            return 1
        end

        set -l uname_out (uname -s)
        set -l script_dir ""
        if string match -q 'Darwin*' -- $uname_out
            set -l script_dir_guess (realpath_int "$self_path")
            set script_dir (begin; builtin cd "$script_dir_guess"; pwd; end)
        else
            set -l script_name (command readlink -f "$self_path" 2>/dev/null)
            if test $status -eq 0; and test -n "$script_name"
                set script_dir (command dirname "$script_name")
            end
            if test -z "$script_dir"
                set script_dir (realpath_int "$self_path")
                set script_dir (begin; builtin cd "$script_dir"; pwd; end)
            end
        end

        set -gx ESP_MATTER_PATH "$script_dir"
        echo "Setting ESP_MATTER_PATH to '$ESP_MATTER_PATH'"
    end

    set -l gn_path "$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/pigweed/"
    if not contains -- $gn_path $PATH
        set -gx PATH $PATH $gn_path
    end

    set -l host_path "$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/out/host"
    if not contains -- $host_path $PATH
        set -gx PATH $PATH $host_path
    end

    set -gx ZAP_INSTALL_PATH "$ESP_MATTER_PATH/connectedhomeip/connectedhomeip/.environment/cipd/packages/zap"
end

esp_matter_export_main
