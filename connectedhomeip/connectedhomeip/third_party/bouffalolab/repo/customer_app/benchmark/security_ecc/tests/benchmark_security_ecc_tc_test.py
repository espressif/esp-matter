from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='benchmark_security_ecc_tc')
def benchmark_security_ecc_tc(env, extra_data):
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
        
        #ecdh hw
        dut.write('test_ecdh_hw')
        speed = dut.expect(re.compile(r"A generate public time is (.+)\r"), timeout=10)
        print('ecdh hw test generate A public key time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"B generate public time is (.+)\r"), timeout=10)
        print('ecdh hw test generate B public key time : {}'.format(speed[0]))

        speed = dut.expect(re.compile(r"generate A secret time is (.+)\r"), timeout=10)
        print('ecdh hw test generate A secret key time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"generate B secret time is (.+)\r"), timeout=10)
        print('ecdh hw test generate B secret key time : {}'.format(speed[0]))

        speed = dut.expect(re.compile(r"ecdh successful total time (.+)\r"), timeout=2)
        print('ecdh hw total time : {}'.format(speed[0]))
        print('')

        #ecdh soft
        dut.write('test_ecdh_soft')
        speed = dut.expect(re.compile(r"A generate public time is (.+)\r"), timeout=10)
        print('ecdh soft test generate A public key time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"B generate public time is (.+)\r"), timeout=10)
        print('ecdh soft test generate B public key time : {}'.format(speed[0]))

        speed = dut.expect(re.compile(r"generate A secret time is (.+)\r"), timeout=10)
        print('ecdh soft test generate A secret key time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"generate B secret time is (.+)\r"), timeout=10)
        print('ecdh soft test generate B secret key time : {}'.format(speed[0]))

        speed = dut.expect(re.compile(r"ecdh checking secrets ok total time (.+)\r"), timeout=2)
        print('ecdh soft total time : {}'.format(speed[0]))
        print('')

        #ecdsa hw
        dut.write('test_ecdsa_hw')
        speed = dut.expect(re.compile(r"generate public time is (.+)\r"), timeout=10)
        print('ecdsa hw test generate public key time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"ecdsa sign time is (.+)\r"), timeout=10)
        print('ecdsa hw test generate sign time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"ecdsa verify time is (.+)\r"), timeout=10)
        print('ecdsa hw test verify sign time : {}'.format(speed[0]))

        speed = dut.expect(re.compile(r"Verify Success total time (.+)\r"), timeout=2)
        print('ecdsa hw total time : {}'.format(speed[0]))
        print('')

        #ecdsa soft
        dut.write('test_ecdsa_soft')
        speed = dut.expect(re.compile(r"generate public time is (.+)\r"), timeout=10)
        print('ecdsa soft test generate public key time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"ecdsa sign time is (.+)\r"), timeout=10)
        print('ecdsa soft test generate sign time : {}'.format(speed[0]))
        speed = dut.expect(re.compile(r"ecdsa verify time is (.+)\r"), timeout=10)
        print('ecdsa soft test verify sign time : {}'.format(speed[0]))

        speed = dut.expect(re.compile(r"check signature ok total time (.+)\r"), timeout=2)
        print('ecdsa soft total time : {}'.format(speed[0]))
        print('')

        dut.halt()

    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 example test failed')
        raise


if __name__ == '__main__':
    benchmark_security_ecc_tc()
