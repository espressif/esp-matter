#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

if [ -d "build-libfuzzer" ]; then rm -r build-libfuzzer; fi
mkdir build-libfuzzer
pushd build-libfuzzer
cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_C_FLAGS="-fsanitize=fuzzer,address -DLIBFUZZER" -DCMAKE_C_COMPILER_WORKS=On
[[ $? -ne 0 ]] && exit 1
make fuzz_pet
[[ $? -ne 0 ]] && exit 1
popd
./build-libfuzzer/fuzz_pet -max_total_time=$1
