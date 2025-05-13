#!/usr/bin/env bash
set -e

cd $IDF_PATH
. ./export.sh
cd $ESP_MATTER_PATH
. ./export.sh

exec "$@"
