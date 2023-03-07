#!/usr/bin/env bash
# Copyright 2020 Espressif Systems (Shanghai) Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Used for Internal Purpose
# This is a helper script which helps to create cryptoauthlib component for esp-idf
# which is a subset of original cryptoauthlib component located at
# https://github.com/MicrochipTech/cryptoauthlib
# The cryptoauthlib component is created mainly to reduce the size of the submodule by removing some unnecessary files from the original component and also integrate build support to make it compatible with esp-idf
# currently esp-cryptoauthlib points to tag 20191122 of https://github.com/MicrochipTech/cryptoauthlib 
# Prerequisute = In this script the github repo of esp-cryptoauthlib is cloned at the same level of tarball containing specific version of cryptoauthlib(Microchip)

link="https://github.com/MicrochipTech/cryptoauthlib/archive/refs/tags/v3.3.1.tar.gz"
curl -LO $link
tar -xzf v3.3.1.tar.gz
git clone https://github.com/espressif/esp-cryptoauthlib
# Delete earlier content of esp-cryptoauthlib/cryptoauthlib
rm -rf esp-cryptoauthlib/cryptoauthlib
mkdir -p esp-cryptoauthlib/cryptoauthlib
cd cryptoauthlib-3.3.1
cp -r app ../esp-cryptoauthlib/cryptoauthlib/
cp -r lib ../esp-cryptoauthlib/cryptoauthlib/
cp -r third_party ../esp-cryptoauthlib/cryptoauthlib/
cp README.md ../esp-cryptoauthlib/cryptoauthlib/
cp license.txt ../esp-cryptoauthlib/cryptoauthlib/LICENSE
echo "cryptoauthlib-v3.3.1" >> ../esp-cryptoauthlib/cryptoauthlib/version.txt
cd .
rm -rf cryptoauthlib-3.3.1
rm -rf v3.3.1.tar.gz

# create TAG with commit name
# git add esp-cryptoauthlib/cryptoauthlib
# git commit $commit
# git tag -a $commit -m "Sync cryptoauthlib component to TAG $commit of https://github.com/MicrochipTech/cryptoauthlib"
