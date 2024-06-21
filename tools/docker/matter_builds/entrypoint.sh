#!/usr/bin/env bash
set -e

source $IDF_PATH/export.sh
source $ESP_MATTER_PATH/export.sh

exec "$@"
