from __future__ import print_function
from __future__ import unicode_literals
from pywifi import const
import pywifi
import time
import re
import os

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_event_RPI_wifi_ap_tc')
def bl602_demo_event_RPI_wifi_ap_tc(env, extra_data):
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

        dut.write('stack_wifi')
        time.sleep(2)
        dut.write('wifi_ap_start 1')
        ap_ssid = dut.expect(re.compile(r"\[WF\]\[SM\] start AP with ssid (.+);"), timeout=50)
        print('Started AP with SSID: {}'.format(ap_ssid[0]))
        rst = connect_device(ap_ssid[0])
        if rst != True:
            raise Exception

        dut.halt()
    except Exception:
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 ble_wifi test failed')
        raise

def connect_device(ssid):
    counter = 0
    wifi = pywifi.PyWiFi()
    iface = wifi.interfaces()[1]
    iface.disconnect()
    time.sleep(1)

    profile = pywifi.Profile()
    profile.ssid = ssid
    profile.auth = const.AUTH_ALG_OPEN
    profile.akm.append(const.AKM_TYPE_WPA2PSK)
    profile.cipher = const.CIPHER_TYPE_CCMP
    profile.key = '12345678'

    iface.remove_all_network_profiles()
    tmp_profile = iface.add_network_profile(profile)
    while counter < 20:
        counter = counter +1
        print ('counter:{}'.format(counter))
        iface.connect(tmp_profile)
        time.sleep(10)
        status = iface.status()
        if status == const.IFACE_SCANNING:
            print ('scan')
        elif status == const.IFACE_INACTIVE:
            print ('incative')
        elif status == const.IFACE_CONNECTING:
            print ('connecting')
        elif status == const.IFACE_DISCONNECTED:
            print ('disconnect')
            time.sleep(5)
        elif status == const.IFACE_CONNECTED:
            print ('connected')
            return True
        else:
            print ('error')
            return False
        iface.disconnect()
    return False

if __name__ == '__main__':
    bl602_demo_event_RPI_wifi_ap_tc()
