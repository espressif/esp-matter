from __future__ import print_function
from __future__ import unicode_literals
import time
import re
import os

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_wifi_sta_ap_tc')
def bl602_demo_wifi_sta_ap_tc(env, extra_data):
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

        for i in range(2):
            print('To reboot BL602')
            dut.write('reboot')
            dut.expect("Booting BL602 Chip...", timeout=0.5)
            print('BL602 rebooted')
            time.sleep(1)

            dut.write('stack_wifi')
            time.sleep(0.5)
            start_time = time.perf_counter()
            while True:
                dut.write('wifi_scan')
                list_all = dut.expect(re.compile(r"cached scan list([\s\S]*?)-----"), timeout=20)
                regexp_str = "channel (\d+)(.*)ppm(.*)SSID"
                #channel 01, bssid B0:95:8E:D3:48:44, rssi -61, ppm abs:rel  -4 :  -4, auth    WPA/WPA2-PSK SSID bl_test_011
                bssid = os.getenv('TEST_ROUTER_SSID')
                str_string = (regexp_str, bssid)
                search_str = " ".join(str_string)
                rst = re.search(search_str, list_all[0])
                if rst is not None:
                    break
                end_time = time.perf_counter()
                if int(end_time - start_time) >= 120:
                    break

            channel = rst.group(1)
            #wifi_ap_start
            if i == 0:
                ap_cmd = ("wifi_ap_start", channel)
            else:
                if 11 - int(channel) >= 5:
                    ap_cmd = ("wifi_ap_start", "11")
                    print("set ap in channel 11")
                else:
                    ap_cmd = ("wifi_ap_start", "1")
                    print("set ap in channel 1")
            start_ap_cmd = ' '.join(ap_cmd)
            dut.write(start_ap_cmd)
            ap_ssid = dut.expect(re.compile(r"\[WF\]\[SM\] start AP with ssid (.+);"), timeout=2)
            print('Started AP with SSID: {}'.format(ap_ssid[0]))
            #wifi_sta_connect
            bssid = os.getenv('TEST_ROUTER_SSID')
            pwd = os.getenv('TEST_ROUTER_PASSWORD')
            cmd = ("wifi_sta_connect", bssid, pwd)
            cmd_wifi_connect = ' '.join(cmd)
            dut.write(cmd_wifi_connect)
            dut.expect("Entering wifiConnected_IPOK state", timeout=50)

            time.sleep(30)
            dut.write('wifi_state')
            dut.expect("wifi state with ap connected ip got", timeout=1)
        dut.halt()
    except DUT.ExpectTimeout:
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 wifi test failed')
        raise


if __name__ == '__main__':
    bl602_demo_wifi_sta_ap_tc()
