#!/bin/bash
# Copyright 2022 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

# If we're not in a bootstrapped shell exit immediately. We won't know where
# the config file is.
if [ -z "$PW_PROJECT_ROOT" ]; then
  exit 0
fi

set -o errexit
set -o nounset
set -o pipefail

echo -n "Updating CIPD packages..."

"$PW_ROOT/pw_env_setup/py/pw_env_setup/env_setup.py" \
  --project-root "$PW_PROJECT_ROOT" \
  --pw-root "$PW_ROOT" \
  --config-file "$_PW_ENVIRONMENT_CONFIG_FILE" \
  --shell-file "$_PW_ACTUAL_ENVIRONMENT_ROOT/unused.sh" \
  --install-dir "$_PW_ACTUAL_ENVIRONMENT_ROOT" \
  --quiet \
  --trust-cipd-hash \
  --skip-submodule-check \
  --cipd-only

echo "done."
