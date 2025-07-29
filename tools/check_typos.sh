#!/bin/bash

# Usage: tools/check_typos.sh [source_file_or_directory] ...

CODESPELL_SKIP_LIST=()
ERROR_FOUND=0

parse_codespellrc_skips() {
    if [ -f .codespellrc ]; then
        local skip_files
        skip_files=$(grep -E "^skip\s*=" .codespellrc | cut -d '=' -f2 | tr -d ' ')
        IFS=',' read -ra CODESPELL_SKIP_LIST <<< "$skip_files"
    fi
}

should_skip_file() {
    local file_path="$1"
    for pattern in "${CODESPELL_SKIP_LIST[@]}"; do
        if [[ "$file_path" == $pattern* ]]; then
            return 0
        fi
    done
    return 1
}

process_file() {
    local SRC_FILE="$1"
    cat "$SRC_FILE" | \
    python3 -c "
import sys, re
ignore_directive = re.compile(r\"[^\w\s]\s*codespell:ignore\b\")
def split_line(line):
    if ignore_directive.search(line):
        return line.rstrip()
    tokens = re.findall(r\"\b[\w']+\b\", line)
    result = []
    for token in tokens:
        if \"'\" in token:
            result.append(token)
        else:
            result.extend(re.findall(r\"\d+[a-zA-Z]+|[A-Z]+(?=[A-Z][a-z])|[A-Z]?[a-z]+|[A-Z]+\", token))
    return \" \".join(result)
for line in sys.stdin:
    print(split_line(line))
" | codespell --check-hidden --stdin-single-line -q 32 - 2>&1 | sed "s|^|$SRC_FILE: |"
    if [ "${PIPESTATUS[2]}" -ne 0 ]; then
        ERROR_FOUND=1
    fi
}

is_valid_file() {
    local file="$1"

    case "$file" in
        *.c|*.h|*.cpp|*.py|*.txt|*.md|*.yml|*.ini|*.json|*.sh|*.cmake) ;;
        *) return 1 ;;
    esac

    if should_skip_file "$file"; then
        return 1
    fi

    return 0
}

process_directory() {
    local DIR="$1"
    local file

    find "$DIR" -type f | while read -r file; do
        if is_valid_file "$file"; then
            process_file "$file"
        fi
    done
}

main() {
    local TARGETS=("$@")

    parse_codespellrc_skips

    if [ "$#" -eq 0 ]; then
        TARGETS=(./components ./examples)
    fi
    
    for TARGET in "${TARGETS[@]}"; do
        if [ -f "$TARGET" ]; then
            if is_valid_file "$TARGET"; then
                process_file "$TARGET"
            fi
        elif [ -d "$TARGET" ]; then
            process_directory "$TARGET"
        else
            echo "Warning: '$TARGET' is not a valid file or directory, skipping"
        fi
    done

    if [ "$ERROR_FOUND" == "1" ]; then
        echo "Typos found"
        exit 1
    fi
}

main "$@"
