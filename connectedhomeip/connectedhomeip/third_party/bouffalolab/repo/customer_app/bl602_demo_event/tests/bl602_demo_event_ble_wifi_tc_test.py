from __future__ import print_function
from __future__ import unicode_literals
import time
import re
import os

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_event_ble_wifi_tc')
def bl602_demo_event_ble_wifi_tc(env, extra_data):
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
        time.sleep(1)

        dut.write('stack_ble')
        time.sleep(1)
        dut.expect(re.compile("BD_ADDR:(.+)"), timeout=1)

        dut.write('stack_wifi')
        time.sleep(1)
        bssid = os.getenv('TEST_ROUTER_SSID')
        pwd = os.getenv('TEST_ROUTER_PASSWORD')
        cmd = ("wifi_sta_connect", bssid, pwd)
        cmd_wifi_connect = ' '.join(cmd)
        dut.write(cmd_wifi_connect)
        #dut.write('wifi_sta_connect bl_test_013 tester12345678')
        #dut.write('wifi_sta_connect grand_2.4G grand86338156')
        dut.expect("Entering wifiConnected_IPOK state", timeout=20)

        print('To reboot BL602')
        dut.write('reboot')
        dut.expect("Booting BL602 Chip...", timeout=0.5)
        print('BL602 rebooted')
        time.sleep(1)

        dut.write('stack_wifi')
        time.sleep(2)
        dut.write(cmd_wifi_connect)
        #dut.write('wifi_sta_connect bl_test_013 tester12345678')
        #dut.write('wifi_sta_connect grand_2.4G grand86338156')
        dut.expect("Entering wifiConnected_IPOK state", timeout=20)

        dut.write('stack_ble')
        time.sleep(1)
        dut.expect(re.compile("BD_ADDR:(.+)"), timeout=1)

        dut.halt()
    except DUT.ExpectTimeout:
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 ble_wifi test failed')
        raise


if __name__ == '__main__':
    bl602_demo_event_ble_wifi_tc()
