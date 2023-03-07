from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_timer_tc')
def bl602_demo_timer_tc(env, extra_data):
    # first, flash dut
    # then, test
    dut = env.get_dut("port0", "fake app path")
    print('Flashing app')
    dut.flash_app(env.log_path, env.get_variable('flash'))
    print('Starting app')
    dut.start_app()
    timestamp_seq = []
    timediff_seq = []

    try:
        dut.expect("Booting BL602 Chip...", timeout=0.5)
        print('BL602 booted')
        dut.expect('Init CLI with event Driven', timeout=0.5)
        print('BL602 CLI init done')
        time.sleep(0.1)

        dut.write('timer_us')
        time.sleep(0.1)
        timestamp = dut.expect(re.compile(r"Timer2 value is (\d+)"), timeout=0.1)
        timestamp_seq.append(int(timestamp[0]))
        time.sleep(0.9)

        dut.write('timer_us')
        time.sleep(0.1)
        timestamp = dut.expect(re.compile(r"Timer2 value is (\d+)"), timeout=0.1)
        timestamp_seq.append(int(timestamp[0]))
        time.sleep(0.9)

        dut.write('timer_us')
        time.sleep(0.1)
        timestamp = dut.expect(re.compile(r"Timer2 value is (\d+)"), timeout=0.1)
        timestamp_seq.append(int(timestamp[0]))
        time.sleep(0.9)

        dut.write('timer_us')
        time.sleep(0.1)
        timestamp = dut.expect(re.compile(r"Timer2 value is (\d+)"), timeout=0.1)
        timestamp_seq.append(int(timestamp[0]))
        time.sleep(0.9)

        dut.write('timer_us')
        time.sleep(0.1)
        timestamp = dut.expect(re.compile(r"Timer2 value is (\d+)"), timeout=0.1)
        timestamp_seq.append(int(timestamp[0]))
        time.sleep(0.9)

        timestamp_last = 0
        for timestamp in timestamp_seq:
            if 0 == timestamp :
                raise
            if 0 == timestamp_last :
                timestamp_last = timestamp
                continue
            timediff_seq.append(timestamp - timestamp_last)
            timestamp_last = timestamp

        for timediff in timediff_seq:
            print(f'time diff is {timediff}')
        for timediff in timediff_seq:
            if timediff < 900000 or timediff > 1100000:
                print(f'time diff is NOT ok: {timediff}')
                raise
        dut.halt()
    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 example test failed')
        raise

if __name__ == '__main__':
    bl602_demo_timer_tc()
