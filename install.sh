#!/usr/bin/env bash

set -e

basedir=$(dirname "$0")
ESP_MATTER_PATH=$(cd "${basedir}"; pwd)
MATTER_PATH=${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip
export ZAP_INSTALL_PATH=${MATTER_PATH}/.environment/cipd/packages/zap

echo ""
echo "Running Matter Setup"
echo ""
cd ${MATTER_PATH}
source ${MATTER_PATH}/scripts/bootstrap.sh
cd ${ESP_MATTER_PATH}

echo ""
echo "Building host tools"
echo ""
gn --root="${MATTER_PATH}" gen ${MATTER_PATH}/out/host --args='chip_inet_config_enable_ipv4=false'
ninja -C ${MATTER_PATH}/out/host chip-cert chip-tool
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
