#!/bin/bash
set -euo pipefail

if [ ! -x "$(command -v astyle_py)" ]; then
	echo "astyle_py not found, please install astyle_py:"
	echo "  pip install -U astyle_py==VERSION"
	echo "where VERSION is the same as in .pre-commit-config.yaml."
	exit 1
fi
pushd "$(pwd)"

cd "${ESP_MATTER_PATH}"

SRC_FILES="$(find components device_hal examples -regextype posix-egrep -regex  ".*\.(c|cpp|cxx|h|hpp|hxx)" |\
             grep -v -iE '/gen/|/build/|/zap-generated/|/managed_components/')"
for SRC_FILE in ${SRC_FILES}; do
    echo "formatting ${SRC_FILE}"
    astyle_py --astyle-version=3.4.7 --rules=tools/ci/astyle-rules.yml "${SRC_FILE}"
done

popd
