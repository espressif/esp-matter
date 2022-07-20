#!/usr/bin/env bash

set -e

basedir=$(dirname "$0")
ESP_MATTER_PATH=$(cd "${basedir}"; pwd)
export ESP_MATTER_PATH

echo ""
echo "Running Matter Setup"
echo ""
source ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/scripts/bootstrap.sh

echo ""
echo "Building chip-tool"
echo ""
${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/scripts/examples/gn_build_example.sh ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/examples/chip-tool ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/examples/chip-tool/out/
echo ""
echo "Created chip-tool executable at: ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/examples/chip-tool/out/"
echo ""

echo "All done! You can now run:"
echo ""
echo "  . ${basedir}/export.sh"
echo ""
