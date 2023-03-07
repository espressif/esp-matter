#!/bin/sh
# Copyright 2020 The Pigweed Authors
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

if [ -z "$PW_ENVIRONMENT_ROOT" ]; then
  PW_ENVIRONMENT_ROOT="$PW_ROOT/environment"
fi
PREFIX="$PW_ENVIRONMENT_ROOT/bootstrap"
mkdir -p "$PREFIX"

set -o errexit

# Update the mtimes on the most recent pw_env_setup executables.
for HASH in $(git --git-dir="$PW_ROOT/.git" --no-pager log --max-count=5 --format=format:%H); do
  if [ -f "$PREFIX/$HASH" ]; then
    touch "$PREFIX/$HASH" &> /dev/null
  fi
done

# Delete any files with an (apparent) age greater than 5 days. This will never
# include the 5 most recent pw_env_setup executables, but if there's been no
# bootstrap call in less than 5 days this could delete all versions of
# pw_env_setup. This is acceptable because it's very unlikely there have been
# no commits in a 5 day period, and if there really have been no commits this
# will just re-download that executable in a few lines.
find "$PREFIX" -mtime +5 -exec rm {} \;

OS=$(uname -s | tr '[:upper:]' '[:lower:]')
if [ "$OS" = "darwin" ]; then
  OS=mac
fi

ARCH=$(uname -m | tr '[:upper:]' '[:lower:]')
if [ "$ARCH" = "x86_64" ]; then
  ARCH="amd64"
fi

# Support `mac-arm64` through Rosetta until `mac-arm64` binaries are ready
if [[ "$OS" = "mac" ] && \
    [ "$ARCH" = "arm64" ] && \
    [ -n "$PW_BOOTSTRAP_USE_ROSETTA" ]]
then
  ARCH="amd64"
fi

for HASH in $(git --git-dir="$PW_ROOT/.git" --no-pager log --max-count=10 --format=format:%H); do
  URL="https://storage.googleapis.com/pigweed-envsetup/$OS-$ARCH"
  URL="$URL/$HASH/pw_env_setup"
  FILEPATH="$PREFIX/$HASH"

  # First try curl.
  if [ ! -f "$FILEPATH" ]; then
    curl -o "$FILEPATH" "$URL" &> /dev/null
  fi

  # If curl fails try wget.
  if [ ! -f "$FILEPATH" ]; then
    wget -O "$FILEPATH" "$URL" &> /dev/null
  fi

  # If either curl or wget succeeded mark the file executable, print it, and
  # exit. If the file appears to be a text file then it doesn't exist in GCS
  # and we'll try the next one.
  TEXT=$(file --mime "$FILEPATH" | grep text)
  if [ -n "$TEXT" ]; then
    rm "$FILEPATH"
    continue
  fi

  if [ -f "$FILEPATH" ]; then
    chmod a+x "$FILEPATH"
    echo "$FILEPATH"
    exit 0
  fi
done

exit -1
