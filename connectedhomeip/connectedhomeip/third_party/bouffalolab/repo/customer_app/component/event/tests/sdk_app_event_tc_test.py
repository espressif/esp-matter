from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='sdk_app_event_tc')
def sdk_app_event_tc(env, extra_data):
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

        dut.expect("######This is event test1", timeout=1)
        dut.expect("######This is event test2", timeout=1)
       
        dut.write('event')
        dut.expect("#####This is event poll_read_fd", timeout=6)
        dut.halt()
    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 event test failed')
        raise


if __name__ == '__main__':
    sdk_app_event_tc()
