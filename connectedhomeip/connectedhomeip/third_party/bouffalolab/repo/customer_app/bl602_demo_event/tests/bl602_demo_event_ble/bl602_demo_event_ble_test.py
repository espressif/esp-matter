from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_event_ble')
def bl602_demo_event_ble(env, extra_data):
    # first, flash dut
    # then, test
    dut1 = env.get_dut("port0", "fake app path")
    dut2 = env.get_dut("port1", "fake app path")

    # TODO: Do parallel?

    print('Flashing app to dut1')
    dut1.flash_app(env.log_path, env.get_variable('flash'))
    print('Flashing app to dut2')
    dut2.flash_app(env.log_path, env.get_variable('flash'))

    print('Starting dut1 app')
    dut1.start_app()
    print('Starting dut2 app')
    dut2.start_app()

    try:
        dut1.expect("Booting BL602 Chip...", timeout=0.5)
        print('[DUT1] booted')
        dut2.expect("Booting BL602 Chip...", timeout=0.5)
        print('[DUT2] booted')

        dut1.expect('Init CLI with event Driven', timeout=0.5)
        print('[DUT1] CLI init done')
        dut2.expect('Init CLI with event Driven', timeout=0.5)
        print('[DUT2] CLI init done')
        time.sleep(0.1)

        dut1.write('stack_ble')
        time.sleep(0.5)
        print('[DUT1] Ble INIT')
        dut2.write('stack_ble')
        time.sleep(0.5)
        print('[DUT2] Ble INIT')

        dut1.write('ble_init')
        dut1.expect('Init successfully', timeout=0.5)
        print('[DUT1] Register the connection callback function')
        dut2.write('ble_init')
        dut2.expect('Init successfully', timeout=0.5)
        print('[DUT2] Register the connection callback function')

        dut1.write('ble_auth')
        dut1.expect('Register auth callback successfully', timeout=0.5)
        print('[DUT1] Register auth callback function')
        dut2.write('ble_auth')
        dut2.expect('Register auth callback successfully', timeout=0.5)
        print('[DUT2] Register auth callback function')

        dut1.write('ble_start_adv 0 0 0x80 0x80')
        dut1.expect('Advertising started', timeout=0.5)
        print('[DUT1] Started adv')

        dut1.write('ble_read_local_address')
        a = dut1.expect(re.compile(r"Local public addr : ((\w{2}.){6})"), timeout=2)[0]
        addr = ''.join(a.split(":"))
        print(f'[DUT1] Get ble address: {addr}')

        dut2.write('ble_start_scan 0 0 0x80 0x50')
        dut2.expect('Start scan successfully', timeout=0.5)
        dut2.expect(re.compile(r'\[DEVICE\]: ' + str(a) + '\((\w{6})\)'), timeout=3)
        print('[DUT2] Start scan')

        dut2.write('ble_stop_scan')
        dut2.expect('Scan successfully stopped', timeout=0.5)

        dut2.write(f'ble_connect 0 {addr}')
        dut2.expect(re.compile(r'Connected: ' + str(a) + '\((\w{6})\)'), timeout=0.5)
        print('[DUT2] Connected')
        time.sleep(1)

        # Security
        dut2.write('ble_security 2')
        dut2.expect('Start security successfully', timeout=0.5)
        print('[DUT2] Start security')

        dut1.expect(re.compile(r'Confirm pairing for ((\w{2}.){6})\((\w{6})\)'), timeout=5)
        dut1.write('ble_auth_pairing_confirm')
        print('[DUT1] Auth pairing confirm')

        dut1.expect(re.compile(r'Bonded with ((\w{2}.){6})\((\w{6})\)'), timeout=5)
        print('[DUT1] Bonded')
        dut2.expect(re.compile(r'Bonded with ' + str(a) + '\((\w{6})\)'), timeout=5)
        print('[DUT2] Bonded')

        dut2.write('ble_exchange_mtu')
        mtu = dut2.expect(re.compile(r'Exchange successful MTU Size =(.*)'), timeout=5)[0]
        print('[DUT2] Mtu size = ', mtu)

        # Discover
        dut2.write('ble_discover 0 0 0x1 0xffff')
        dut2.expect(re.compile(r'Service ((\w{4})) found: start handle \w, end_handle \w'), timeout=5)
        time.sleep(0.5)
        print('[DUT2] Ble discover primary service')

        dut2.write('ble_discover 1 0 0x1 0xffff')
        dut2.expect('Discover complete', timeout=5)
        time.sleep(0.5)
        print('[DUT2] Ble discover secondary service')

        dut2.write('ble_discover 2 0 0x1 0xffff')
        dut2.expect('Discover complete', timeout=5)
        time.sleep(0.5)
        print('[DUT2] Ble discover include service')

        dut2.write('ble_discover 3 0 0x1 0xffff')
        dut2.expect(re.compile(r'Characteristic ((\w{4})) found: attr->handle \w  chrc->handle \w'), timeout=5)
        time.sleep(0.5)
        print('[DUT2] Ble discover characteristic')

        dut2.write('ble_discover 4 0 0x1 0xffff')
        descriptor = dut2.expect(re.compile(r'Descriptor (.*) found: handle \w'), timeout=5)[0]
        time.sleep(0.5)
        print('[DUT2] Ble discover descriptor')
        print('[DUT2] Descriptor', descriptor)


    except DUT.ExpectTimeout:
        dut1.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 example test failed')
        raise
    finally:
        # we should ALWAYS stop chip, free resources after test is done
        dut1.halt()
        dut2.halt()


if __name__ == '__main__':
    bl602_demo_event_ble()
