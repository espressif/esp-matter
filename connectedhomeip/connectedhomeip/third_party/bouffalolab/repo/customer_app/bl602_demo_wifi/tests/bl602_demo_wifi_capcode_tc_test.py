from __future__ import print_function
from __future__ import unicode_literals
import time
import re
import math
import os

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_wifi_capcode_tc')
def bl602_demo_wifi_capcode_tc(env, extra_data):
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
        time.sleep(0.5)

        dut.write('wifi_capcode 34')
        time.sleep(0.5)
        
        dut.write('wifi_scan')
        list_all = dut.expect(re.compile(r"cached scan list([\s\S]*?)-----"), timeout=20)
        regexp_str = "ppm abs:rel(.*)(-?\d*) :(.*)(-?\d*), auth(.*)SSID"

        bssid1 = os.getenv('ROUTER_SSID_FOR_PPM1')
        bssid2 = os.getenv('ROUTER_SSID_FOR_PPM2')
        bssid3 = os.getenv('ROUTER_SSID_FOR_PPM3')
        
        str_str1 = (regexp_str, bssid1)
        str_str2 = (regexp_str, bssid2)
        str_str3 = (regexp_str, bssid3)
       
        search_str1 = " ".join(str_str1)
        search_str2 = " ".join(str_str2)
        search_str3 = " ".join(str_str3)
        
        dev1 = re.search(search_str1, list_all[0])
        dev2 = re.search(search_str2, list_all[0])
        dev3 = re.search(search_str3, list_all[0])
        #dev1 = re.search("ppm abs:rel   (\d+) :   (\d+), auth(.*)SSID bl_test_011", list_all[0])
        #dev2 = re.search("ppm abs:rel   (\d+) :   (\d+), auth(.*)SSID bl_test_042", list_all[0])
        #dev3 = re.search("ppm abs:rel   (\d+) :   (\d+), auth(.*)SSID bl_test_043", list_all[0])
        if (dev1 != None) :
            dev1_num = dev1.group(1)
            if (math.fabs(int(dev1_num)) > 5) :
                print(f'use dev1:{bssid1}, ppm need calibration {dev1_num}')
                raise Exception("ppm need calibration") 
            else:
                print(f'use dev1:{bssid1}, ppm is accurate {dev1_num}')
        elif (dev2 != None) :
            dev2_num = dev2.group(1)
            if (math.fabs(int(dev2_num)) > 5) :
                print(f'use dev2:{bssid2}, ppm need calibration {dev2_num}')
                raise Exception("ppm need calibration") 
            else:
                print(f'use dev2:{bssid2}, ppm is accurate {dev2_num}')
        elif (dev3 != None) :
            dev3_num = dev3.group(1)
            if (math.fabs(int(dev3_num)) > 5) :
                print(f'use dev3:{bssid3}, ppm need calibration {dev3_num}')
                raise Exception("ppm need calibration") 
            else:
                print(f'use dev3:{bssid3}, ppm is accurate {dev3_num}')
        else :
            raise Exception("No test equipment available") 

        dut.halt()

    except Exception:
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 wifi capcode test failed')
        raise


if __name__ == '__main__':
    bl602_demo_wifi_capcode_tc()
