#!/usr/bin/env bash
# Run cppcheck on esp-matter components
#
# Usage:
#   ./tools/ci/run_cppcheck.sh [--include-generated]
#
# Prerequisites:
#   Install cppcheck before running this script

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
REPORT_FILE="$REPO_ROOT/cppcheck_report.txt"
INCLUDE_GENERATED=false

parse_args() {
    for arg in "$@"; do
        case "$arg" in
            --include-generated) INCLUDE_GENERATED=true ;;
            -h|--help)
                echo "Usage: $0 [--include-generated]"
                exit 0
                ;;
            *)
                echo "Unknown argument: $arg"
                exit 1
                ;;
        esac
    done
}

collect_sources() {
    local find_excludes=(
        -not -path '*/zap_common/*'
        -not -path '*/managed_components/*'
        -not -path '*/connectedhomeip/*'
        -not -path '*/test/*'
    )
    if [[ "$INCLUDE_GENERATED" == false ]]; then
        find_excludes+=(-not -path '*/data_model/generated/*')
    fi

    find "$REPO_ROOT/components" \
        \( -name '*.c' -o -name '*.cpp' \) \
        "${find_excludes[@]}" 2>/dev/null | sort
}

run_cppcheck() {
    local src_files=("$@")

    cppcheck \
        --enable=warning,style,performance,portability \
        --std=c++17 \
        --language=c++ \
        --suppressions-list="$SCRIPT_DIR/cppcheck_suppressions.txt" \
        --inline-suppr \
        --error-exitcode=1 \
        --template='{file}:{line}: {severity}: {message} [{id}]' \
        --quiet \
        -I "$REPO_ROOT/components/esp_matter" \
        -I "$REPO_ROOT/components/esp_matter/utils" \
        -I "$REPO_ROOT/components/esp_matter/data_model" \
        -I "$REPO_ROOT/components/esp_matter_bridge" \
        -I "$REPO_ROOT/components/esp_matter_console" \
        -I "$REPO_ROOT/components/esp_matter_controller" \
        -I "$REPO_ROOT/components/esp_matter_ota_provider" \
        -I "$REPO_ROOT/components/esp_matter_rainmaker" \
        "${src_files[@]}" 2>"$REPORT_FILE"
}

report_results() {
    if [[ -s "$REPORT_FILE" ]]; then
        echo "cppcheck: $(wc -l < "$REPORT_FILE" | tr -d ' ') finding(s)"
        cat "$REPORT_FILE"
        exit 1
    fi
    echo "cppcheck: clean"
    rm -f "$REPORT_FILE"
}

main() {
    parse_args "$@"

    command -v cppcheck &>/dev/null || { echo "cppcheck not found"; exit 1; }
    echo "cppcheck version: $(cppcheck --version)"

    mapfile -t src_files < <(collect_sources)
    if [[ ${#src_files[@]} -eq 0 ]]; then
        echo "No source files found"
        exit 1
    fi
    echo "Checking ${#src_files[@]} files..."

    run_cppcheck "${src_files[@]}"
    report_results
}

main "$@"
