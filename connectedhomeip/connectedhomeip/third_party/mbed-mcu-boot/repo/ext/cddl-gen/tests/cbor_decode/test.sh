#!/bin/bash
#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#


do_test() {
        test_dir=$1
        test_platform=$2

        pushd "$test_dir"

        if [ -d "build" ]; then rm -r build; fi
        mkdir build &&cd build

        cmake -GNinja -DBOARD=$test_platform .. 2>&1 1> test.log
        if [[ $? -ne 0 ]]; then
            cat test.log
            exit 1
        fi

        ninja >> test.log
        if [[ $? -ne 0 ]]; then
            cat test.log
            exit 1
        fi

        ninja run
        [[ $? -ne 0 ]] && exit 1

        popd
}


for dir in 'test1_suit_old_formats/' 'test2_suit/' 'test3_simple/' 'test5_strange/' 'test7_suit9_simple' ;
do
        for board in 'native_posix' 'native_posix_64' ;
        do
                echo ""
                echo "do_test $dir $board"
                do_test $dir $board
        done
done
