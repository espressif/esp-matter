from __future__ import print_function
from __future__ import unicode_literals
import time
import re

from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='benchmark_security_aes_tc')
def benchmark_security_aes_tc(env, extra_data):
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
        
        for aes in ['ecb', 'cbc', 'ctr', 'gcm'] :
            for key, keybit in enumerate(['128', '192', '256'], 1) :
                for ram, ramtype in enumerate(['dtcm', 'ocram', 'wram'], 0) :

                    dut.write('test_aes_{0} 32768 256 {1} 0 {2}'.format(aes, key, ram))
                    speed = dut.expect(re.compile(r"aes encrypt speed is (.+)\r"), timeout=30)
                    print('aes {0} {1}bit hw   {2} test done speed is : {3}'.format(aes, keybit, ramtype, speed[0]))

                    dut.write('test_aes_{0} 32768 48 {1} 1 {2}'.format(aes, key, ram))
                    speed = dut.expect(re.compile(r"aes encrypt speed is (.+)\r"), timeout=30)
                    print('aes {0} {1}bit soft {2} test done speed is : {3}'.format(aes, keybit, ramtype, speed[0]))

                    print('')

        # gmac hw test
        dut.write('test_aes_gmac 32768 256 0')
        speed = dut.expect(re.compile(r"aes encrypt speed is (.+)\r"), timeout=30)
        print('aes gmac hw dtcm test done speed is : {}'.format(speed[0]))
        
        dut.write('test_aes_gmac 32768 256 1')
        speed = dut.expect(re.compile(r"aes encrypt speed is (.+)\r"), timeout=30)
        print('aes gmac hw ocram test done speed is : {}'.format(speed[0]))
        
        dut.write('test_aes_gmac 32768 256 2')
        speed = dut.expect(re.compile(r"aes encrypt speed is (.+)\r"), timeout=30)
        print('aes gmac hw wram test done speed is : {}'.format(speed[0]))
        print('')

        # sha test
        for sha in ['sha1', 'sha224', 'sha256'] :
            for ram, ramtype in enumerate(['dtcm', 'ocram', 'wram'], 0) :

                dut.write('test_{0} 32768 256 0 {1}'.format(sha, ram))
                speed = dut.expect(re.compile(r"sha speed is (.+)\r"), timeout=30)
                print('{0} hw   {1} test done speed is : {2}'.format(sha, ramtype, speed[0]))

                dut.write('test_{0} 32768 48 1 {1}'.format(sha, ram))
                speed = dut.expect(re.compile(r"sha speed is (.+)\r"), timeout=30)
                print('{0} soft {1} test done speed is : {2}'.format(sha, ramtype, speed[0]))

                print('')

        dut.halt()

    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: BL602 example test failed')
        raise


if __name__ == '__main__':
    benchmark_security_aes_tc()
