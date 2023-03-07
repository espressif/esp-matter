from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_event_tc')
def bl602_demo_event_tc(env, extra_data):
    # first, flash dut
    # then, test
    dut = env.get_dut("port0", "fake app path")
    print('Flashing app')
    dut.flash_app(env.log_path, env.get_variable('flash'))
    print('Starting app')
    dut.start_app()

    try:
        dut.expect("Booting BL602 Chip...", timeout=0.5)
        print('BL602 booted')
        dut.expect('Init CLI with event Driven', timeout=0.5)
        print('BL602 CLI init done')
        time.sleep(0.1)

        print('To reboot BL602')
        dut.write('reboot')
        dut.expect("Booting BL602 Chip...", timeout=0.5)
        print('BL602 rebooted')
        time.sleep(1)
        dut.write('stack_wifi')
        time.sleep(1)
        dut.write('wifi_ap_start')
        ap_ssid = dut.expect(re.compile(r"\[WF\]\[SM\] start AP with ssid (.+);"), timeout=2)
        print('Started AP with SSID: {}'.format(ap_ssid[0]))
        dut.halt()
    except DUT.ExpectTimeout:
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 example test failed')
        raise


if __name__ == '__main__':
    bl602_demo_event_tc()
