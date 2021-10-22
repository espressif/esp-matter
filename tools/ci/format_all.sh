#!/bin/bash

pushd "$(pwd)"

cd "${ESP_MATTER_PATH}"
SRC_FILES="$(find components device_hal examples -regextype posix-egrep -regex  ".*\.(c|cpp|cxx|h|hpp|hxx)" |\
             grep -v -iE '/gen/|/build/|/zap-generated/')"
for SRC_FILE in ${SRC_FILES}; do
    echo "formatting ${SRC_FILE}"
    clang-format -i "${SRC_FILE}"
done

popd
