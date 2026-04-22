#!/usr/bin/env bash
# Run cppcheck on esp-matter components
#
# Usage:
#   ./tools/ci/run_cppcheck.sh [--include-generated]
#
# Options:
#   --include-generated    Include data_model/generated/ files in analysis
#
# Prerequisites:
#   cppcheck: brew install cppcheck (or apt-get install cppcheck)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Parse arguments
INCLUDE_GENERATED=false

for arg in "$@"; do
    case "$arg" in
        --include-generated)
            INCLUDE_GENERATED=true
            ;;
        -h|--help)
            echo "Usage: $0 [--include-generated]"
            echo ""
            echo "Options:"
            echo "  --include-generated    Include data_model/generated/ files in analysis"
            exit 0
            ;;
        *)
            echo "Unknown argument: $arg"
            echo "Usage: $0 [--include-generated]"
            exit 1
            ;;
    esac
done

GENERATED_EXCLUDE="data_model/generated"
if [[ "$INCLUDE_GENERATED" == true ]]; then
    GENERATED_EXCLUDE=""
    echo -e "${YELLOW}Including generated code in analysis${NC}"
fi

echo -e "${YELLOW}=== Running cppcheck ===${NC}"

if ! command -v cppcheck &> /dev/null; then
    echo -e "${RED}cppcheck not found. Install: brew install cppcheck (or apt-get install cppcheck)${NC}"
    exit 1
fi

echo "cppcheck version: $(cppcheck --version)"

# Build find exclusions
find_excludes=(
    -not -path '*/zap_common/*'
    -not -path '*/managed_components/*'
    -not -path '*/connectedhomeip/*'
    -not -path '*/test/*'
)
if [[ -n "$GENERATED_EXCLUDE" ]]; then
    find_excludes+=(-not -path '*/data_model/generated/*')
fi

# Collect source files from components/
src_files=()
while IFS= read -r f; do
    src_files+=("$f")
done < <(find "$REPO_ROOT/components" \
    \( -name '*.c' -o -name '*.cpp' \) \
    "${find_excludes[@]}" \
    2>/dev/null | sort)

if [[ ${#src_files[@]} -eq 0 ]]; then
    echo -e "${RED}No source files found${NC}"
    exit 1
fi

echo "Checking ${#src_files[@]} files..."

report_file="$REPO_ROOT/cppcheck_report.txt"

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
    "${src_files[@]}" 2>"$report_file"

exit_code=$?

if [[ -s "$report_file" ]]; then
    count=$(wc -l < "$report_file" | tr -d ' ')
    echo -e "${RED}cppcheck: $count finding(s)${NC}"
    cat "$report_file"
    exit 1
fi

echo -e "${GREEN}cppcheck: clean${NC}"
rm -f "$report_file"
exit $exit_code
