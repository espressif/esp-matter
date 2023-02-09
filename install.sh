#!/usr/bin/env bash

set -e

basedir=$(dirname "$0")
ESP_MATTER_PATH=$(cd "${basedir}"; pwd)
MATTER_PATH=${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip
export ESP_MATTER_PATH

echo ""
echo "Running Matter Setup"
echo ""
source ${MATTER_PATH}/scripts/bootstrap.sh

echo ""
echo "Building host tools"
echo ""
gn --root="${MATTER_PATH}" gen ${MATTER_PATH}/out/host
ninja -C ${MATTER_PATH}/out/host
echo ""
echo "Host tools built at: ${MATTER_PATH}/out/host"
echo ""

echo ""
echo "Exit Matter environment"
echo ""
deactivate

echo ""
echo "Installing python dependencies for mfg_tool"
echo ""
python3 -m pip install -r ${ESP_MATTER_PATH}/tools/mfg_tool/requirements.txt

echo ""
echo "Installing python dependencies for Matter"
echo ""
python3 -m pip install -r ${ESP_MATTER_PATH}/requirements.txt

echo ""
echo "Installing zap-cli"
echo ""
# Run the zap_download.py and extract the path of installed binary
# eg output before cut: "export ZAP_INSTALL_PATH=zap/zap-v2023.01.19-nightly"
# output after cut: zap/zap-v2023.01.19-nightly
zap_path=`python3 connectedhomeip/connectedhomeip/scripts/tools/zap/zap_download.py \
    --sdk-root connectedhomeip/connectedhomeip --zap RELEASE --extract-root zap \
    2>/dev/null | cut -d= -f2`
# Move files to one directory up, so that binaries will be in zap/ directory and export.sh can leverage the fixed path
mv $zap_path/* zap/
rm -r $zap_path
chmod +x zap/zap-cli

echo "All done! You can now run:"
echo ""
echo "  . ${basedir}/export.sh"
echo ""
