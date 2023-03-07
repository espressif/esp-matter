from __future__ import print_function
from __future__ import unicode_literals
from bluepy import btle
import time
import re
import os
import subprocess
from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_event_RPI_ble_wifi_tc')
def bl602_demo_event_RPI_ble_wifi_tc(env, extra_data):
    # first, flash dut
    # then, test
    dut = env.get_dut("port0", "fake app path")
    print('Flashing app')
    dut.flash_app(env.log_path, env.get_variable('flash'))
    print('Starting app')
    dut.start_app()

    try:
        dut.expect("Booting BL602 Chip...", timeout=1.5)
        print('BL602 booted')
        dut.expect('Init CLI with event Driven', timeout=1.5)
        print('BL602 CLI init done')
        time.sleep(1.1)

        dut.write('stack_ble')
        time.sleep(1.5)
        bd_addr = dut.expect(re.compile(r"BD_ADDR:(.*)"), timeout=5)
        print(f'bd_addr is {bd_addr[0]}')
        dut.write('ble_init')
        dut.expect("Init successfully", timeout=5)
        dut.write('ble_auth')
        dut.expect("Register auth callback successfully", timeout=5)
        dut.write('ble_start_adv 0 0 0x80 0x80')
        dut.expect("Advertising started", timeout=5)
        
        dut.write('ble_read_local_address')
        local_addr = dut.expect(re.compile(r"Local public addr : (.*) "), timeout=5)
        print(f'Local public addr {local_addr[0]}')
        # scan bluetooth
        
        rst = scan_device(local_addr[0])
        if not rst:
            rst_flag = False
            for i in range(5):
                rst = scan_device(local_addr[0])
                if rst:
                    rst_flag = True
                    break
            if rst_flag:
                print("scan success!")
                connect_device(local_addr[0])
                time.sleep(1)
                dut.expect("Connected", timeout=5)
                print("connect success!")
            else:
                raise Exception
        else:
            print("scan success!")
            connect_device(local_addr[0])
            time.sleep(1)
            dut.expect("Connected", timeout=5)
            print("connect success!")
        dut.write('stack_wifi')
        time.sleep(1)
        bssid = os.getenv('TEST_ROUTER_SSID')
        pwd = os.getenv('TEST_ROUTER_PASSWORD')
        cmd = ("wifi_sta_connect", bssid, pwd)
        cmd_wifi_connect = ' '.join(cmd)
        dut.write(cmd_wifi_connect)
        #dut.write('wifi_sta_connect bl_test_027 12345678')
        dut.expect("Entering wifiConnected_IPOK state", timeout=20)

        print('To reboot BL602')
        dut.write('reboot')
        dut.expect("Booting BL602 Chip...", timeout=1.5)
        print('BL602 rebooted')
        time.sleep(1)

        dut.write('stack_wifi')
        time.sleep(1)
        bssid = os.getenv('TEST_ROUTER_SSID')
        pwd = os.getenv('TEST_ROUTER_PASSWORD')
        cmd = ("wifi_sta_connect", bssid, pwd)
        cmd_wifi_connect = ' '.join(cmd)
        dut.write(cmd_wifi_connect)
        #dut.write('wifi_sta_connect bl_test_027 12345678')
        dut.expect("Entering wifiConnected_IPOK state", timeout=20)

        dut.write('stack_ble')
        time.sleep(1)
        dut.write('ble_init')
        dut.expect("Init successfully", timeout=3)
        dut.write('ble_auth')
        dut.expect("Register auth callback successfully", timeout=3)
        dut.write('ble_start_adv 0 0 0x80 0x80')
        dut.expect("Advertising started", timeout=3)
        
        dut.write('ble_read_local_address')
        local_addr = dut.expect(re.compile(r"Local public addr : (.*) "), timeout=5)
        print(f'Local public addr is {local_addr[0]}')
        rst = scan_device(local_addr[0])
        if not rst:
            rst_flag = False
            for i in range(5):
                rst = scan_device(local_addr[0])
                if rst:
                    rst_flag = True
                    break
            if rst_flag:
                print("scan success!")
                connect_device(local_addr[0])
                time.sleep(1)
                dut.expect("Connected", timeout=5)
                print("connect success!")
            else:
                raise Exception
        else:
            print("scan success!")
            time.sleep(1)
            connect_device(local_addr[0])
            time.sleep(1)
            dut.expect("Connected", timeout=5)
            print("connect success!")
        dut.halt()
    except Exception:
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 ble_wifi test failed')
        raise


def scan_device(mac):
    rst = []
    result_list = []
    p = subprocess.Popen('timeout -s INT 10s hcitool lescan', shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    line_list = p.stdout.readlines()
    print(line_list)
    for line in line_list:
        if line != b'':
            try:
                result_list = line.decode('utf-8').split('\n')
            except:
                result_list = line.decode('gbk').split('\n')
            
            finally:
                rst = rst + result_list

    device_dict = {}
    for device in rst:
        if device.find(':') != -1:
            key = device[0:17].strip()
            value = device[18:].strip()
            device_dict[key] = value
    try:
        device_name = device_dict[mac]
        print("mac:{}, name:{}".format(mac, device_name))
        return True
    except KeyError:
        return False


def connect_device(mac):
    ble_connect_flag = False
    for i in range(5):
        try:
            conn = btle.Peripheral(mac, "public")
            ble_connect_flag = True
            print("BLE is connected")
            break
        except:
            time.sleep(10)
    if ble_connect_flag is False:
        print('Failed to connect to peripheral {}, addr type: public'.format(mac))
        raise
    #conn.disconnect()
    #print("BLE is disconnected")


if __name__ == '__main__':
    bl602_demo_event_RPI_ble_wifi_tc()
