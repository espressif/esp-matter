from __future__ import print_function
from __future__ import unicode_literals
import socket,fcntl,struct,psutil
import time
import re
import os
import subprocess,signal
import hashlib
import lzma
from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_wifi_ota_tc')
def bl602_demo_wifi_ota_tc(env, extra_data):
    # first, flash dut
    # then, test
    dut = env.get_dut("port0", "fake app path")
    print('Flashing app')
    dut.flash_app(env.log_path, env.get_variable('flash'))
    print('Starting app')
    dut.start_app()

    try:
        nc = False
        cat = False
        dut.expect("Booting BL602 Chip...", timeout=5)
        print('BL602 booted')
        #dut.expect('Init CLI with event Driven', timeout=0.5)
        #print('BL602 CLI init done')
        time.sleep(1)
        list_all = dut.expect(re.compile(r'======= PtTable_Config([\s\S]*?)======= FlashCfg magiccode'), timeout=4)
        rst_str = 'FW  (.*)  (.*)  (.*)  (.*)  (.*)'
        dev1 = re.search(rst_str, list_all[0])
        if (dev1 != None) :
            old_age = int(dev1.group(5))
            print(old_age)

        print(dev1)

        bssid = os.getenv('TEST_ROUTER_SSID')
        pwd = os.getenv('TEST_ROUTER_PASSWORD')
        cmd = ("wifi_sta_connect", bssid, pwd)
        cmd_wifi_connect = ' '.join(cmd)
        print (cmd_wifi_connect)
        ip = get_ip_address(bytes('eth0', encoding = "utf8"))
        print(ip)
        path = os.getcwd()
        path = os.path.abspath('../../../../..')
        ota_path = path + '/tools/flash_tool/chips/bl602/ota'
        file_path = ota_path + "/" + 'FW_OTA.bin.xz.ota'
        print(file_path)

        for i in range (10):
            sequence = i + 1
            new_age = ota_update(dut, ip, cmd_wifi_connect, ota_path)
            if new_age != 2 * sequence:
                raise Exception("ota failed")
            else:
                print ("{} time ota update success".format(sequence))
            substitute_xz_ota_bin(file_path)

        dut.halt()
    except Exception:
        #os.killpg(os.getpgid(p.pid),9)
        """
        if (nc) :
            os.kill(int(nc), signal.SIGKILL)
        if (cat) :
            os.kill(int(cat), signal.SIGKILL)
        p.kill()
        """
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 ota test failed')
        raise

def ota_update(dut, ip, cmd_wifi_connect, ota_path):
    dut.write('stack_wifi')
    time.sleep(1)
    dut.write(cmd_wifi_connect)
    dut.expect("Entering wifiConnected_IPOK state", timeout=30)
    print ("connect wifi success")
    p = subprocess.Popen("timeout 60 sh -c 'cat FW_OTA.bin.xz.ota | nc -l 3333'", shell=True, cwd=ota_path)
    print(p.pid)
    time.sleep(5)
    cmd = 'ota_tcp' + ' ' + ip
    dut.write(cmd)
    time.sleep(0.5)
    print ("start update")
    dut.expect("Update PARTITION", timeout=100)
    dut.expect("Booting BL602 Chip...", timeout=20)
    list_all = dut.expect(re.compile(r'======= PtTable_Config([\s\S]*?)======= FlashCfg magiccode'), timeout=4)
    time.sleep(2)
    rst_str = 'FW  (.*)  (.*)  (.*)  (.*)  (.*)'
    dev1 = re.search(rst_str, list_all[0])
    if (dev1 != None) :
        new_age = int(dev1.group(5))
        print(new_age)
    else:
        raise Exception("no match") 
    return new_age
    
def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ip=fcntl.ioctl(s.fileno(),0x8915,struct.pack('256s', ifname[:15]))
    return socket.inet_ntoa(ip[20:24])

def bl60x_mfg_ota_patch_header(original_header, file_bytearray):
    header_len = 512
    header = bytearray()
    file_len = len(file_bytearray)
    m = hashlib.sha256()

    # 16 Bytes header
    data = b'BL60X_OTA_Ver1.0'
    for b in data:
        header.append(b)

    type_offset = 16
    for b in original_header[type_offset : type_offset + 4]:
        header.append(b)

    # 4 Bytes file length
    file_len_bytes = file_len.to_bytes(4, byteorder='little')
    for b in file_len_bytes:
        header.append(b)

    # 8 Bytes pad
    header.append(0x01)
    header.append(0x02)
    header.append(0x03)
    header.append(0x04)
    header.append(0x05)
    header.append(0x06)
    header.append(0x07)
    header.append(0x08)

    ver_hardware_offset = 16 + 4 + 4 + 8
    ver_software_offset = ver_hardware_offset + 16
    for b in original_header[ver_hardware_offset : ver_hardware_offset + 16]:
        header.append(b)
    for b in original_header[ver_software_offset : ver_software_offset + 16]:
        header.append(b)


    # 32 Bytes SHA256
    m.update(file_bytearray)
    hash_bytes = m.digest()
    for b in hash_bytes:
        header.append(b)
    header_len = header_len - len(header)
    while header_len > 0:
        header.append(0xFF)
        header_len = header_len - 1
    header = bytes(header)
    return header

def substitute_xz_ota_bin(filename):
    bl60x_xz_filters = [
        {
            "id": lzma.FILTER_LZMA2,
            "dict_size": 32768
        },
    ]

    orig = open(filename, 'rb')

    orig_header = orig.read(512)
    orig_payload = orig.read()
    orig.close()

    orig_payload_decompressed = bytearray(lzma.decompress(orig_payload))

    replace_str_bytes = b'Clearing and Disable all the pending IRQ001'
    dst_payload_decompressed = orig_payload_decompressed.replace(b'Clearing and Disable all the pending IRQ...', replace_str_bytes)
    if replace_str_bytes in orig_payload_decompressed:
        print ("replace success")
    dst_payload = lzma.compress(orig_payload_decompressed, check=lzma.CHECK_CRC32, filters=bl60x_xz_filters)
    dst_header = bl60x_mfg_ota_patch_header(orig_header, dst_payload)
    dst = open(filename, "wb")
    dst.write(dst_header)
    dst.write(dst_payload)
    dst.close()

if __name__ == '__main__':
    bl602_demo_wifi_ota_tc()
