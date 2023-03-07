from __future__ import print_function
from __future__ import unicode_literals
import socket,fcntl,struct,psutil
import time,re,os
import subprocess
import psutil
import datetime
from tiny_test_fw import DUT, App, TinyFW
from ttfw_bl import BL602App, BL602DUT


@TinyFW.test_method(app=BL602App.BL602App, dut=BL602DUT.BL602TyMbDUT, test_suite_name='bl602_demo_wifi_RPI_open_iperf_tc')
def bl602_demo_wifi_RPI_open_iperf_tc(env, extra_data):
    # first, flash dut
    # then, test
    dut = env.get_dut("port0", "fake app path")
    print('Flashing app')
    dut.flash_app(env.log_path, env.get_variable('flash'))
    print('Starting app')
    dut.start_app()

    time.sleep(2)

    try:
        RPI_ip = get_ip_address(bytes('eth0', encoding = "utf8"))
        board_log_name = env.log_path + '/port0.log'
        test_cmd = ['ipc', 'ips', 'ipu', 'ipus']
        test_num = 0
        for action_cmd in test_cmd:
            default_cmd_list = ['stack_wifi', 'wifi_sta_connect']
            dut.start_app()
            time.sleep(1)
            for default_cmd in default_cmd_list:
                print("------Executing default command {}, please wait...".format(default_cmd))
                if default_cmd == 'wifi_sta_connect':
                    bssid = os.getenv('TEST_ROUTER_SSID')
                    pwd = os.getenv('TEST_ROUTER_PASSWORD')
                    cmd = ("wifi_sta_connect", bssid, pwd)
                    cmd_wifi_connect = ' '.join(cmd)
                    dut.write(cmd_wifi_connect)
                    ip = dut.expect(re.compile(r"IP: (\S+)"), timeout=30)
                    board_ip = ''.join(ip)
                    print(f'board ip is {board_ip}')
                else:
                    dut.write(default_cmd)
                time.sleep(1)

            print("----The default command has been executed and is being executed {}, please wait...".format(action_cmd))
            iperf_log_name = get_iperf_log_path(env, action_cmd)
            check_result = ()
            if action_cmd == 'ipu':
                implement_time = '20'
                board_cmd = "ipu {}".format(RPI_ip)

                result = subprocess.Popen('iperf -s -u -t ' + implement_time + ' -i 1 -f m', shell=True,
                                          stdout=subprocess.PIPE)
                time.sleep(1)
                dut.write(board_cmd)
                print("ipu_test {}".format(board_cmd))
                dut.expect('bind UDP socket successfully!', timeout=1)
                write_log(iperf_log_name, result, implement_time)
                check_result = check_iperf_log_result(env, action_cmd, iperf_log_name)
                if check_result == 'failed':
                    print("ipu failed!")
                    test_num += 1
                check_result = check_board_log_result(action_cmd, board_log_name)
                if check_result == 'failed':
                    print("ipu failed!")
                    test_num += 1
               
            elif action_cmd == 'ipc':
                implement_time = '20'
                board_cmd = "ipc {}".format(RPI_ip)

                result = subprocess.Popen('iperf -s -t ' + implement_time + ' -i 1 -f m', shell=True,
                                          stdout=subprocess.PIPE)
                time.sleep(1)

                dut.write(board_cmd)
                print("ipc_test {}".format(board_cmd))
                dut.expect('Connect to iperf server successful!', timeout=10)
                write_log(iperf_log_name, result, implement_time)
                check_result = check_iperf_log_result(env, action_cmd, iperf_log_name)
                if check_result == 'failed':
                    print("ipc failed!")
                    test_num += 1

                check_result = check_board_log_result(action_cmd, board_log_name)
                if check_result == 'failed':
                    print("ipc failed!")
                    test_num+=1

            elif action_cmd == 'ips':
                implement_time = '20'
                board_cmd = "ips"
                dut.write(board_cmd)
                print("ips_test {}".format(board_cmd))
                time.sleep(1)
                dut.expect('[NET] [IPC] [IPS] Starting iperf server on 0.0.0.0', timeout=1)

                result = subprocess.Popen('iperf -c ' + board_ip + ' -t ' + implement_time + ' -i 1 -f m',
                                          shell=True, stdout=subprocess.PIPE)
                time.sleep(1)

                write_log(iperf_log_name, result, implement_time)
                check_result = check_iperf_log_result(env, action_cmd, iperf_log_name)
                if check_result == 'failed':
                    print("ipc failed!")
                    test_num += 1

                check_result = check_board_log_result(action_cmd, board_log_name)
                if check_result == 'failed':
                    print("ipc failed!")
                    test_num+=1

            elif action_cmd == 'ipus':
                implement_time = '20'
                board_cmd = "ipus"

                result = subprocess.Popen('iperf -u -c ' + board_ip + ' -t ' + implement_time + ' -i 1 -f m',
                                          shell=True, stdout=subprocess.PIPE)
                time.sleep(1)

                dut.write(board_cmd)
                print("ips_test {}".format(board_cmd))
                dut.expect('[NET] [IPC] [IPUS] Connecting with default address 0.0.0.0', timeout=1)
                write_log(iperf_log_name, result, implement_time)
                check_result = check_iperf_log_result(env, action_cmd, iperf_log_name)
                if check_result == 'failed':
                    print("ipc failed!")
                    test_num+=1

                check_result = check_board_log_result(action_cmd, board_log_name)
                if check_result == 'failed':
                    print("ipc failed!")
                    test_num+=1

            else:
                print('endif')
            
        if test_num != 0:
            raise Exception

        dut.halt()
    except Exception:
        dut.write('p 0')
        result_text = dut.read()
        print(result_text)
        print('ENV_TEST_FAILURE: BL602 demo_wifi test failed')
        raise


def get_iperf_log_path(env, test_cmd):
    #cur_path = os.getcwd()
    #sdk_path = os.path.abspath('../../../../..')
    #log_path = sdk_path + '/TEST_LOGS'
    log_path = env.log_path
    local_time = time.strftime('%Y%m%d_%H%M%S', time.localtime(time.time()))
    log_name = 'iperf_{}_{}.log'.format(test_cmd, local_time)
    file_name = "{}/{}".format(log_path, log_name)
    return file_name


def find_iperf_speed_line(action_cmd, lines_data):
    # iperf_data = []
    iperf_flag = 0
    iperf_find = 0
    low_iperf_list = []
    ave_iperf_list = []
    high_iperf_list = []
    for data in lines_data:
        line = str(data)
        if line.find(action_cmd) != -1:
            iperf_flag = 1
            iperf_find = 1
        if iperf_flag == 1:
            if line.find('(') != -1 and line.find(')') != -1 and line.find('Mbps') != -1:
                a = line.split('(')[1]
                a = a.split(') ')[0]
                low_iperf_list.append(a.split(' ')[0])
                ave_iperf_list.append(a.split(' ')[1])
                high_iperf_list.append(a.split(' ')[2])
    num = 0
    for item in ave_iperf_list:
        num += float(item)
    if num != 0:
        min_data = min(low_iperf_list)
        max_data = max(high_iperf_list)
        average_data = round(num / len(ave_iperf_list), 2)
        print("min:{}, average:{}, max:{}".format(min_data, average_data, max_data))
        return min_data
    else:
        return 'failed'


def check_board_log_result(action_cmd, log_name):
    with open(log_name, 'rb') as f:
        lines_date = ''
        try:
            lines_data = f.readlines()
        except:
            print('device reports readiness to read but returned no data ')
        if action_cmd == 'ipc':
            find_iperf_speed_line('ipc ', lines_data) 
        elif action_cmd == 'ips':
            find_iperf_speed_line('ips', lines_data) 
        elif action_cmd == 'ipu':
            find_iperf_speed_line('ipu ', lines_data) 
        elif action_cmd == 'ipus':
            find_iperf_speed_line('ipus', lines_data) 
        else:
            pass
        # f.close()


def standard_output(env, action_cmd, average_):
    log_path = env.log_path
    log_path = os.path.abspath(os.path.join(log_path, ".."))
    file_name = log_path + '/' + 'standard_output'
    value_ = '{}={}'.format(action_cmd, average_)
    with open(file_name, 'a+') as f:
        f.write(value_ + ', ')


def check_iperf_log_result(env, action_cmd, log_name):
    time.sleep(5)
    with open(log_name, 'r') as f:
        lines_date = ''
        try:
            lines_date = f.readlines()
        except:
            print('device reports readiness to read but returned no data ')
        str_lines_data = str(lines_date)

        if action_cmd == 'ips' or action_cmd == 'ipc' or action_cmd == 'ipus':
            iperf_data = []
            for line in lines_date:
                if line.find('Mbits/sec') != -1:
                    temp_data = float(line.split(' ')[-2])
                    iperf_data.append(temp_data)
            num = 0
            for item in iperf_data:
                num += item
            # print(iperf_data)
            if num != 0:
                max_data = max(iperf_data)
                min_data = min(iperf_data)
                average_data = round(num / len(iperf_data), 2)
                print("min_data:{}, average_data:{}, max_data:{}".format(min_data, average_data, max_data))
                standard_output(env, action_cmd, average_data)
                #return min_data, average_data, max_data
                return min_data
            else:
                return "fail"
        elif action_cmd == 'ipu':
            iperf_data = []
            for line in lines_date:
                if line.find('Mbits/sec') != -1:
                    temp_list = line.split(' ')
                    temp_list = [i for i in temp_list if i != '']
                    data_str = ''
                    for i in range(0, len(temp_list)):
                        if temp_list[i] == 'Mbits/sec':
                            data_str = temp_list[i - 1]

                    temp_data = float(data_str)
                    iperf_data.append(temp_data)
            num = 0
            for item in iperf_data:
                num += item
            if num != 0:
                max_data = max(iperf_data)
                min_data = min(iperf_data)
                average_data = round(num / len(iperf_data), 2)
                print("min_data:{}, average_data:{}, max_data:{}".format(min_data, average_data, max_data))
                standard_output(env, action_cmd, average_data)
                #return min_data, average_data, max_data
                return min_data
            else:
                return "fail"


def write_log(iperf_log_name, result, timeout):
    with open(iperf_log_name, mode='w') as fhandle:
        start_time = time.perf_counter()
        #print('start_time = {}'.format(start_time))
        while True:  
            cur_time = time.perf_counter()
            #print('cur_time = {}'.format(cur_time))
            if cur_time - start_time < float(timeout):
                time.sleep(0.3)
                fhandle.write(result.stdout.readline().decode("gbk").strip() + "\n")
            else:
                fhandle.close()
                break


def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ip=fcntl.ioctl(s.fileno(),0x8915,struct.pack('256s', ifname[:15]))
    return socket.inet_ntoa(ip[20:24])


if __name__ == '__main__':
    bl602_demo_wifi_RPI_open_iperf_tc()
