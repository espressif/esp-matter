from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='sdk_app_romfs_tc')
def sdk_app_romfs_tc(env, extra_data):
    # first, flash dut
    # then, test
    dut = env.get_dut("port0", "fake app path")
    print('Flashing app')
    dut.flash_app(env.log_path, env.get_variable('flash'))
    print('Starting app')
    dut.start_app()

    try:
        dut.expect("Booting BL602 Chip...", timeout=1)
        print('BL602 booted')
        dut.expect('Init CLI with event Driven', timeout=1)
        print('BL602 CLI init done')

        dut.write('romfs')
        dut.expect("case1:len = 1", timeout=1)
        dut.expect("49")
        dut.expect("case2:len = 1", timeout=1)
        dut.expect("33")
        dut.expect("case3:len = 50", timeout=1)
        dut.expect("03 00 00 00 00 00 3f 54 58 58 58 00 00 00 03 00 00 00 00 00 50 52 49 56 00 00 00 05 00 00 00 7b 04 00 00 54 53 53 45 00 00 00 0f 00 00 00 4c 61 76 66")
        dut.expect("case4:len = 50", timeout=1)
        dut.expect("49 44 33 03 00 00 00 00 00 3f 54 58 58 58 00 00 00 03 00 00 00 00 00 50 52 49 56 00 00 00 05 00 00 00 7b 04 00 00 54 53 53 45 00 00 00 0f 00 00 00 4c")
        dut.expect("case5:len = 0", timeout=1)
        dut.expect("00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00")
       
        dut.halt()
    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 romfs test failed')
        raise


if __name__ == '__main__':
    sdk_app_romfs_tc()
