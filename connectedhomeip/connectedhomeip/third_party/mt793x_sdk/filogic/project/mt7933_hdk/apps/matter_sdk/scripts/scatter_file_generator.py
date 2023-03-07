import argparse
import configparser

def main():
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('--output', type=str, required=True, help='input the path of output scatter file path')
    arg_parser.add_argument('--bl_path', type=str, required=True, help='input the path of bootloader')
    arg_parser.add_argument('--img_path', type=str, required=True, help='input the path of image')
    arg_parser.add_argument('--bt_path', type=str, required=True, help='input the path of bluetooth driver binary')
    arg_parser.add_argument('--wifi_path', type=str, required=True, help='input the path of wifi driver binary')
    args = arg_parser.parse_args()

    config = configparser.ConfigParser()
    config['ROM_BL'] = {
                'enable': 'y',
                'start_addr': '0x00000000',
                'partition_size': '0x00010000',
                'file_name': args.bl_path,
                'readback': 'n'
            }
    config['ROM_RBL'] = {
                'enable': 'n',
                'start_addr': '0x00010000',
                'partition_size': '0x00002000',
                'file_name': '',
                'readback': 'n'
            }
    config['ROM_TFM'] = {
                'enable': 'n',
                'start_addr': '0x00000000',
                'partition_size': '0x00000000',
                'file_name': '',
                'readback': 'n'
            }
    config['ROM_RTOS'] = {
                'enable': 'y',
                'start_addr': '0x00044000',
                'partition_size': '0x0020b000',
                'file_name': args.img_path,
                'readback': 'n'
            }
    config['ROM_FOTA'] = {
                'enable': 'n',
                'start_addr': '0x00000000',
                'partition_size': '0x00000000',
                'file_name': '',
                'readback': 'n'
            }
    config['ROM_NVDM'] = {
                'enable': 'y',
                'start_addr': '0x006e5000',
                'partition_size': '0x00010000',
                'file_name': 'erase only',
                'readback': 'n'
            }
    config['ROM_LOG'] = {
                'enable': 'n',
                'start_addr': '0x00000000',
                'partition_size': '0x00000000',
                'file_name': '',
                'readback': 'n'
            }
    config['ROM_BT'] = {
                'enable': 'y',
                'start_addr': '0x00705000',
                'partition_size': '0x0004c000',
                'file_name': args.bt_path,
                'readback': 'n'
            }
    config['ROM_WIFI_PWRTBL'] = {
                'enable': 'n',
                'start_addr': '0x00000000',
                'partition_size': '0x00000000',
                'file_name': '',
                'readback': 'n'
            }
    config['ROM_WIFI_EXT'] = {
                'enable': 'y',
                'start_addr': '0x00760000',
                'partition_size': '0x0009f000',
                'file_name': args.wifi_path,
                'readback': 'n'
            }
    config['ROM_BUFFER_BIN'] = {
                'enable': 'n',
                'start_addr': '0x007fff000',
                'partition_size': '0x00010000',
                'file_name': '',
                'readback': 'n'
            }
    config['EFUSE'] = {
                'enable': 'n',
                'start_addr': '0x00000000',
                'partition_size': '0x00000000',
                'file_name': '',
                'readback': 'n'
            }

    with open(args.output, 'w+') as configfile:
        # to strip ini file blank lines
        #TODO Lambda expression
        write_org = configfile.write
        def my_write(string):
            if string.strip():
                write_org(string)
        configfile.write = my_write
        config.write(configfile, space_around_delimiters=False)

if __name__ == "__main__":
    main()
