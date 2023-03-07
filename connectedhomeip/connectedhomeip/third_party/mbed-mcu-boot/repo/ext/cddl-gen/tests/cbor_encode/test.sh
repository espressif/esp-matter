#!/bin/bash
#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#


do_test() {
        test_dir=$1
        test_platform=$2
        canonical_val=$3

        pushd "$test_dir"

        if [ -d "build" ]; then rm -r build; fi
        mkdir build && cd build

        cmake -GNinja -DBOARD=$test_platform $canonical_val .. 2>&1 1> test.log
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


for dir in 'test1_suit/' 'test2_simple/' 'test3_strange/' ;
do
        for board in 'native_posix' 'native_posix_64' ;
        do
                for canonical in '-DCANONICAL=CANONICAL' '' ;
                do
                        if [[ $canonical == '-DCANONICAL=CANONICAL' ]] || [[ $dir != 'test1_suit/' ]] ; then
                                echo ""
                                echo "do_test $dir $board $canonical"
                                do_test $dir $board $canonical
                        fi
                done
        done
done
