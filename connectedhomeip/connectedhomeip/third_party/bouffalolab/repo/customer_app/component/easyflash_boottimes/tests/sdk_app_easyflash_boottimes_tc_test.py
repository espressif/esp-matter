from __future__ import print_function
from __future__ import unicode_literals
import time
import re
import csv

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT

REBOOT_TIMES = 30

@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='sdk_app_easyflash_boottimes_tc')
def sdk_app_easyflash_boottimes_tc(env, extra_data):
    # first, flash dut
    # then, test
    dut = env.get_dut("port0", "fake app path")
    print('Flashing app')
    dut.flash_app(env.log_path, env.get_variable('flash'))
    print('Starting app')
    dut.start_app()

    try:
        f = open('boot_times.csv', 'w', encoding='utf-8')
        csv_writer = csv.writer(f)
        csv_writer.writerow(["boot_times", "init_time", "read_time","write_time"])

        for boot_counts in range(REBOOT_TIMES):
            dut.expect("Booting BL602 Chip...", timeout=0.5)
            print('BL602 booted')
            time.sleep(2)
            list = []
            result_text = dut.read()
            print (result_text)
            s = re.search(r"easyflash init time us (.+)\r", result_text)
            init_time = s.groups()
            s = re.search(r"easyflash read boot_times us (.+)\r", result_text)
            read_time = s.groups()
            s = re.search(r"easyflash write boot_times us (.+)\r", result_text)
            write_time = s.groups()
            s = re.search(r"The system now boot times (.+)\r", result_text)
            boot_times = s.groups()
            list.append(str(boot_times[0]))
            list.append(str(init_time[0]))
            list.append(str(read_time[0]))
            list.append(str(write_time[0]))
            print('boot_times %s, init_time %s, read_time %s, write_time %s' 
            % (boot_times[0], init_time[0], read_time[0], write_time[0]))
            csv_writer.writerow(list)
            dut.write('reboot')
        f.close()
        dut.halt()
        
    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 example test failed')
        raise


if __name__ == '__main__':
    sdk_app_easyflash_boottimes_tc()
