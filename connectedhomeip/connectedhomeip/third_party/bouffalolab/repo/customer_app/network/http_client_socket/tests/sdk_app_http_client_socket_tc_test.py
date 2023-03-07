from __future__ import print_function
from __future__ import unicode_literals
import time
import re
import os

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='sdk_app_http_client_socket_tc')
def sdk_app_http_client_socket_tc(env, extra_data):
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
        
        time.sleep(1)

        dut.write('stack_wifi')
        time.sleep(0.5)
        bssid = os.getenv('TEST_ROUTER_SSID')
        pwd = os.getenv('TEST_ROUTER_PASSWORD')
        cmd = ("wifi_sta_connect", bssid, pwd)
        cmd_wifi_connect = ' '.join(cmd)
        dut.write(cmd_wifi_connect)        
        #dut.write('wifi_sta_connect bl_test_013 tester12345678')

        dut.expect("Entering wifiConnected_IPOK state", timeout=20)

        dut.write('http')
        dut.expect("Server ip Address", timeout=2)
        dut.expect("Download comlete", timeout=200)
        
        dut.halt()
    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 http test failed')
        raise


if __name__ == '__main__':
    sdk_app_http_client_socket_tc()
