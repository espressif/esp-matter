from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='sdk_app_easyflash_tc')
def sdk_app_easyflash_tc(env, extra_data):
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
        time.sleep(2)
        dut.write('psm_erase')
        time.sleep(1)
        dut.write('psm_test')
        dut.expect("kvbin set 1 byte bin -> read 1 byte -> memcmp success.", timeout=2)
        dut.expect("kvbin set 1024 byte bin -> read 1024 byte -> memcmp success.", timeout=2)
        dut.expect("kvbin set 2048 byte bin -> read 2048 byte -> memcmp success.", timeout=2)
        dut.expect("kvbin set 3978 byte bin -> read 3978 byte -> memcmp success.", timeout=2)
        dut.expect("kvbin set 3979 byte bin -> read 3979 byte -> memcmp success.", timeout=2)
        dut.expect("kvbin set/get 3980 byte error, res1 = 8, res2 = 3979.", timeout=2)
        dut.halt()
    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 sdk_app_easyflash test failed')
        raise


if __name__ == '__main__':
    sdk_app_easyflash_tc()
