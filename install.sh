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
echo "Installing zap-cli"
echo ""
# Run the zap_download.py and extract the path of installed binary
# eg output before cut: "export ZAP_INSTALL_PATH=zap/zap-v2023.03.06-nightly"
# output after cut: zap/zap-v2023.03.06-nightly
# TODO: Remove the zap-version after https://github.com/project-chip/connectedhomeip/pull/25727 merged
zap_path=`python3 ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/scripts/tools/zap/zap_download.py \
    --sdk-root ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip --zap RELEASE --zap-version v2023.03.06-nightly \
    --extract-root .zap 2>/dev/null | cut -d= -f2`
# Check whether the download is successful.
if [ -z $zap_path ]; then
    echo "Failed to install zap-cli"
    deactivate
    exit 1
fi

# Move files to one directory up, so that binaries will be in $ESP_MATTER_PATH/.zap/ directory and export.sh can leverage the fixed path
if [ -d "${ESP_MATTER_PATH}/.zap" ]; then
    rm -r ${ESP_MATTER_PATH}/.zap
fi
mkdir ${ESP_MATTER_PATH}/.zap
mv $zap_path/* ${ESP_MATTER_PATH}/.zap/
rm -r $zap_path
chmod +x ${ESP_MATTER_PATH}/.zap/zap-cli

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

echo "All done! You can now run:"
echo ""
echo "  . ${basedir}/export.sh"
echo ""
